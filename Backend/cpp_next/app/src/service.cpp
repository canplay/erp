#include "service.h"
#include "logger.h"
#include "utils/time.h"
#include "utils/uuid.h"

#include <cpr/cpr.h>

#include <memory>
#include <unordered_map>

namespace common {
// 静态成员初始化
std::unordered_map<std::string, int> Service::serviceRetryCount_;
std::mutex Service::retryCountMutex_;

namespace {
// 创建一个辅助函数来发送标准JSON响应
inline void
sendJsonResponse(const std::function<void(const HttpResponsePtr &)> &callback,
                 const std::string &message, int status,
                 Json::Value data = Json::nullValue) {
  Json::Value ret;
  ret["data"] = data;
  ret["message"] = message;
  ret["status"] = status;
  callback(HttpResponse::newHttpJsonResponse(ret));
}

// 创建一个辅助函数来处理数据库错误
inline void handleDatabaseError(
    const orm::DrogonDbException &e,
    const std::function<void(const HttpResponsePtr &)> &callback,
    const std::string &operation) {
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, true,
                                    "数据库{}错误: {} - {}:{}", operation,
                                    e.base().what(), __FILE__, __LINE__);

  sendJsonResponse(callback, "服务器内部错误", 0);
}
} // namespace

void Service::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    return sendJsonResponse(callback, "参数错误", 0);
  }

  std::string service = (*json)["service"].asString();
  if (!validateRequestParams(*json, callback)) {
    return;
  }

  std::string url = (*json)["url"].asString();
  std::string id = (*json)["id"].asString();
  std::string secret = (*json)["secret"].asString();
  std::string access_token = (*json)["access_token"].asString();

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM service WHERE name = $1",
      [=, this](const orm::Result &r) {
        auto timestamp = utils::Time::now();

        if (r.size() > 0) {
          // 更新现有服务
          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "UPDATE service SET url = $1, status = 'enable', update_date = "
              "$2, client_id = $3, client_secret = $4, access_token = $5 WHERE "
              "name = $6",
              [=, this](const orm::Result &r) {
                if (r.affectedRows() > 0) {
                  sendJsonResponse(callback, "success", 1);

                  // 使用更简洁的方式启动协程
                  std::thread([this, service, url]() {
                    drogon::sync_wait(this->checkAsync(service, url));
                  }).detach();
                } else {
                  sendJsonResponse(callback, "更新失败", 0);
                }
              },
              [&](const orm::DrogonDbException &e) {
                handleDatabaseError(e, callback, "更新");
              },
              url, timestamp, id, secret, access_token, service);
        } else {
          // 插入新服务
          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "INSERT INTO service VALUES ($1, $2, 'enable', $3, $4, $5, $6, "
              "$7, $8)",
              [=, this](const orm::Result &r) {
                if (r.affectedRows() > 0) {
                  sendJsonResponse(callback, "success", 1);

                  // 初始化服务重试计数
                  initServiceRetryCount(service);

                  // 使用更简洁的方式启动协程
                  std::thread([this, service, url]() {
                    drogon::sync_wait(this->checkAsync(service, url));
                  }).detach();
                } else {
                  sendJsonResponse(callback, "新增失败", 0);
                }
              },
              [&](const orm::DrogonDbException &e) {
                handleDatabaseError(e, callback, "插入");
              },
              common::utils::Uuid::uuid(), service, timestamp, timestamp, url,
              id, secret, access_token);
        }
      },
      [&](const orm::DrogonDbException &e) {
        handleDatabaseError(e, callback, "查询");
      },
      service);
}

void Service::health(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  Json::Value json;
  json["msg"] = "success";
  json["status"] = 1;
  callback(HttpResponse::newHttpJsonResponse(json));
}

Task<> Service::checkAsync(std::string service, std::string url) const {
  auto timestamp = utils::Time::now();

  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "检查服务: {} - {}", service, url);

  auto ar = cpr::GetAsync(
      cpr::Url{fmt::format("{}/api/service/heartbeat", url)},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Bearer{app().getCustomConfig()["auth"]["m2m"]["key"].asString()},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});

  try {
    auto res = ar.get();

    bool serviceOk = (res.status_code == 200);
    if (serviceOk) {
      Json::Reader reader;
      Json::Value body;
      if (reader.parse(res.text, body)) {
        serviceOk = (body["msg"].asString() == "success");
      } else {
        serviceOk = false;
      }
    }

    auto db = app().getDbClient("postgresql-master");
    if (serviceOk) {
      db->execSqlAsync(
          "UPDATE service SET status = 'enable', update_date = $1 WHERE "
          "name = $2",
          [=](const orm::Result &r) {
            common::Logger::getInstance().log(common::Logger::LOGLEVEL::info,
                                              false, "服务已启用: {} - {}",
                                              service, url);
          },
          [=](const orm::DrogonDbException &e) {
            common::Logger::getInstance().log(
                common::Logger::LOGLEVEL::error, true,
                "更新服务状态失败: {} - {}:{}", e.base().what(), __FILE__,
                __LINE__);
          },
          timestamp, service);

      // 30秒后再次检查
      co_await sleepCoro(drogon::app().getLoop(), 30.0);
      co_await checkAsync(service, url);
    } else {
      db->execSqlAsync(
          "UPDATE service SET status = 'disable', update_date = $1 "
          "WHERE name = $2",
          [=, this](const orm::Result &r) {
            common::Logger::getInstance().log(common::Logger::LOGLEVEL::info,
                                              false, "服务已禁用: {} - {}",
                                              service, url);

            // 检查是否需要重试
            if (decrementServiceRetryCount(service)) {
              // 在新的协程中安排重试
              std::thread([this, service, url]() {
                drogon::sync_wait(this->checkAsync(service, url));
              }).detach();
            }
          },
          [=](const orm::DrogonDbException &e) {
            common::Logger::getInstance().log(
                common::Logger::LOGLEVEL::error, true,
                "更新服务状态失败: {} - {}:{}", e.base().what(), __FILE__,
                __LINE__);
          },
          timestamp, service);
    }
  } catch (const std::exception &e) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, true,
                                      "检查服务时发生异常: {} - {} - {}:{}",
                                      e.what(), service, url, __FILE__,
                                      __LINE__);

    // 即使检查失败也更新数据库状态
    auto db = app().getDbClient("postgresql-master");
    db->execSqlAsync(
        "UPDATE service SET status = 'disable', update_date = $1 "
        "WHERE name = $2",
        [=, this](const orm::Result &r) {
          common::Logger::getInstance().log(common::Logger::LOGLEVEL::info,
                                            false, "服务已禁用(异常): {} - {}",
                                            service, url);

          // 检查是否需要重试
          if (decrementServiceRetryCount(service)) {
            std::thread([this, service, url]() {
              drogon::sync_wait(this->checkAsync(service, url));
            }).detach();
          }
        },
        [=](const orm::DrogonDbException &e) {
          common::Logger::getInstance().log(
              common::Logger::LOGLEVEL::error, true,
              "更新服务状态失败: {} - {}:{}", e.base().what(), __FILE__,
              __LINE__);
        },
        timestamp, service);
  }

  co_return;
}

void Service::initServiceRetryCount(const std::string &service) const {
  std::lock_guard<std::mutex> lock(retryCountMutex_);
  serviceRetryCount_[service] = kDefaultRetryCount;
}

bool Service::decrementServiceRetryCount(const std::string &service) const {
  std::lock_guard<std::mutex> lock(retryCountMutex_);
  auto it = serviceRetryCount_.find(service);
  if (it != serviceRetryCount_.end() && it->second > 0) {
    it->second--;
    return true;
  }
  return false;
}

bool Service::validateRequestParams(
    const Json::Value &json,
    std::function<void(const HttpResponsePtr &)> &callback) const {
  std::string service = json["service"].asString();
  if (service.empty()) {
    sendJsonResponse(callback, "没有可用服务节点", 0);
    return false;
  }
  return true;
}

} // namespace common