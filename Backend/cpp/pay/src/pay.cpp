#include "pay.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Pay::count(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM pay WHERE \"id\" IS NOT NULL";

  if ((*json).find("status"))
    stmt =
        fmt::format("{} AND status = '{}'", stmt, (*json)["status"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND \"type\" = '{}'", stmt, (*json)["type"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row[0].as<int>();
          ret["message"] = "success";
          ret["status"] = 1;
        } else {
          ret["data"] = 0;
          ret["message"] = "success";
          ret["status"] = 1;
        }

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      });
}

void Pay::list(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM pay WHERE \"id\" IS NOT NULL";

  if ((*json).find("status"))
    stmt =
        fmt::format("{} AND status = '{}'", stmt, (*json)["status"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND \"type\" = '{}'", stmt, (*json)["type"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  if ((*json).find("sortBy")) {
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

    if ((*json)["descending"].asBool())
      stmt = fmt::format("{} DESC", stmt);
    else
      stmt = fmt::format("{} ASC", stmt);
  }

  stmt =
      fmt::format("{} LIMIT {} OFFSET {}", stmt, (*json)["maxPage"].asInt64(),
                  (*json)["curPage"].asInt64());

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (auto it : r) {
          Json::Value info;
          info["id"] = it["id"].as<std::string>();
          info["order"] = it["order"].as<std::string>();
          info["status"] = it["status"].as<std::string>();
          info["type"] = it["type"].as<std::string>();
          info["order_pay"] = it["order_pay"].as<Json::Value>();
          info["amount"] = it["amount"].as<int>();
          info["remark"] = it["remark"].as<std::string>();
          info["create_service"] = it["create_service"].as<std::string>();
          info["create_params"] = it["create_params"].as<Json::Value>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["update_date"] = it["update_date"].as<std::string>();
          infos.append(info);
        }

        ret["data"] = infos;
        ret["message"] = "success";
        ret["status"] = 1;
        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      });
}

void Pay::latest(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback,
                 const std::string &id) const {
  async_run([=]() -> Task<> {
    try {
      auto redisClient = app().getRedisClient("redis-slave");
      auto redis_result =
          co_await redisClient->execCommandCoro("GET pay:%s", id.c_str());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(fmt::format(
            "SELECT * FROM pay WHERE \"id\" IS NOT NULL AND status = 'paid' "
            "AND remark LIKE '%{}%' ORDER BY update_date DESC LIMIT 1",
            id));

        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info["id"] = row["id"].as<std::string>();
          info["order"] = row["order"].as<std::string>();
          info["status"] = row["status"].as<std::string>();
          info["type"] = row["type"].as<std::string>();
          info["order_pay"] = row["order_pay"].as<Json::Value>();
          info["amount"] = row["amount"].as<int>();
          info["remark"] = row["remark"].as<std::string>();
          info["create_service"] = row["create_service"].as<std::string>();
          info["create_params"] = row["create_params"].as<Json::Value>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();

          ret["data"] = info;
          ret["message"] = "success";
          ret["status"] = 1;

          auto redisClient = app().getRedisClient("redis-master");
          co_await redisClient->execCommandCoro(
              "SET pay:%s %s EX 600", row["id"].as<std::string>().c_str(),
              Json::FastWriter().write(info).c_str());
        } else {
          ret["data"] = info;
          ret["message"] = "success";
          ret["status"] = 1;
        }

        callback(HttpResponse::newHttpJsonResponse(ret));
      } else {
        Json::Value info;
        Json::Reader reader;
        if (reader.parse(redis_result.asString(), info)) {
          Json::Value ret;
          ret["data"] = info;
          ret["message"] = "success";
          ret["status"] = 1;
          callback(HttpResponse::newHttpJsonResponse(ret));
        } else {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "pay latest redis error - {}:{}", __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = "获取最新支付订单缓存失败";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      }
    } catch (const std::exception &e) {
      common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                           e.what(), __FILE__, __LINE__);

      Json::Value ret;
      ret["message"] = e.what();
      ret["status"] = 0;

      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  });
}
} // namespace api
