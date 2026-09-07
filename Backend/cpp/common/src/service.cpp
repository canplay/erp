#include "include/service.h"
#include "include/utility.h"
#include "pch.h"

namespace common {
int g_Retry = 5;

void Service::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string service = (*json)["service"].asString();
  std::string url = (*json)["url"].asString();
  std::string id = (*json)["id"].asString();
  std::string secret = (*json)["secret"].asString();
  std::string access_token = (*json)["access_token"].asString();

  if (service.empty()) {
    Json::Value ret;
    ret["message"] = "没有可用服务节点";
    ret["status"] = 0;

    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM service WHERE name = $1",
      [=, this](const orm::Result &r) {
        auto timestamp = common::utility::timeNow();

        if (r.size() > 0) {
          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "UPDATE service SET url = $1, status = 'enable', update_date = "
              "$2, client_id = $3, client_secret = $4, access_token = $5 WHERE "
              "name = $6",
              [=, this](const orm::Result &r) {
                if (r.affectedRows() > 0) {
                  Json::Value ret;
                  ret["message"] = "success";
                  ret["status"] = 1;

                  async_run([=, this]() -> Task<> {
                    check(service, url);
                    co_return;
                  });
                  return callback(HttpResponse::newHttpJsonResponse(ret));
                }

                Json::Value ret;
                ret["message"] = "更新失败";
                ret["status"] = 0;
                callback(HttpResponse::newHttpJsonResponse(ret));
              },
              [=](const orm::DrogonDbException &e) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "{} - {}:{}", e.base().what(), __FILE__,
                                     __LINE__);

                Json::Value ret;
                ret["message"] = e.base().what();
                ret["status"] = 0;

                callback(HttpResponse::newHttpJsonResponse(ret));
              },
              url, timestamp, id, secret, access_token, service);
        } else {
          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "INSERT INTO service VALUES ($1, $2, 'enable', $3, $4, $5, $6, "
              "$7, $8)",
              [=, this](const orm::Result &r) {
                if (r.affectedRows() > 0) {
                  Json::Value ret;
                  ret["message"] = "success";
                  ret["status"] = 1;

                  async_run([=, this]() -> Task<> {
                    check(service, url);
                    co_return;
                  });
                  return callback(HttpResponse::newHttpJsonResponse(ret));
                }

                Json::Value ret;
                ret["message"] = "新增失败";
                ret["status"] = 0;

                callback(HttpResponse::newHttpJsonResponse(ret));
              },
              [=](const orm::DrogonDbException &e) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "{} - {}:{}", e.base().what(), __FILE__,
                                     __LINE__);

                Json::Value ret;
                ret["message"] = e.base().what();
                ret["status"] = 0;

                callback(HttpResponse::newHttpJsonResponse(ret));
              },
              common::utility::uuid(), service, timestamp, timestamp, url, id,
              secret, access_token);
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
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

void Service::check(std::string service, std::string url) const {
  async_run([=, this]() -> Task<> {
    auto timestamp = common::utility::timeNow();

    common::utility::log(common::utility::LOGLEVEL::info, false,
                         "check service: {} - {}", service, url);

    auto ar = cpr::GetAsync(
        cpr::Url{fmt::format("{}/api/service/heartbeat", url)},
        cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
        cpr::Bearer{app().getCustomConfig()["auth"]["m2m"]["key"].asString()},
        cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
    auto res = ar.get();

    if (res.status_code == 200) {
      Json::Reader reader;
      Json::Value body;
      if (reader.parse(res.text, body)) {
        if (body["msg"].asString() == "success") {
          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "UPDATE service SET status = 'enable', update_date = $1 WHERE "
              "name = $2",
              [=](const orm::Result &r) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "enable service: {} - {}", service, url);
              },
              [=](const orm::DrogonDbException &e) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "{} - {}:{}", e.base().what(), __FILE__,
                                     __LINE__);
              },
              timestamp, service);

          async_run([=, this]() -> Task<> {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            check(service, url);
            co_return;
          });
          co_return;
        }
      }
    }

    auto db = app().getDbClient("postgresql-master");
    db->execSqlAsync(
        "UPDATE service SET status = 'disable', update_date = $1 "
        "WHERE name = $2",
        [=, this](const orm::Result &r) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "disable service: {} - {}", service, url);

          if (g_Retry > 0) {
            g_Retry -= 1;

            async_run([=, this]() -> Task<> {
              check(service, url);
              co_return;
            });
          }
        },
        [=](const orm::DrogonDbException &e) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);
        },
        timestamp, service);

    co_return;
  });
}
} // namespace common
