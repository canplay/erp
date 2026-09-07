#include "detail.h"
#include "drogon/HttpTypes.h"
#include "fmt/format.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Detail::count(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   const std::string id) const {
  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT COUNT(id) FROM chat_comments_detail WHERE parent = $1 AND "
      "\"delete\" = 0",
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
      },
      id);
}

void Detail::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  async_run([=]() -> Task<> {
    try {
      auto redisClient = app().getRedisClient("redis-slave");
      auto redis_result = co_await redisClient->execCommandCoro(
          "GET chat:detail:%d", (*json)["parent"].asString());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        std::string stmt =
            fmt::format("SELECT * FROM chat_comments_detail WHERE "
                        "parent = '{}' AND \"delete\" = 0",
                        (*json)["parent"].asString());

        if ((*json).find("sortBy")) {
          stmt =
              fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

          if ((*json)["descending"].asBool())
            stmt = fmt::format("{} DESC", stmt);
          else
            stmt = fmt::format("{} ASC", stmt);
        }

        stmt = fmt::format("{} LIMIT {} OFFSET {}", stmt,
                           (*json)["maxPage"].asInt64(),
                           (*json)["curPage"].asInt64());

        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(stmt);

        Json::Value ret;
        Json::Value infos;

        if (r.size() > 0) {
          for (auto it : r) {
            Json::Value info;
            info["id"] = it["id"].as<std::string>();
            info["content"] = it["content"].as<std::string>();
            info["parent"] = it["parent"].as<std::string>();
            info["like"] = it["like"].as<int>();
            info["dislike"] = it["dislike"].as<int>();
            info["create_user"] = it["create_user"].as<std::string>();
            info["create_date"] = it["create_date"].as<std::string>();
            info["update_user"] = it["update_user"].as<std::string>();
            info["update_date"] = it["update_date"].as<std::string>();
            info["delete"] = it["delete"].as<int>();
            infos.append(info);
          }

          auto redisClient = app().getRedisClient("redis-master");
          co_await redisClient->execCommandCoro(
              "SET chat:detail:%d %s EX 600", (*json)["parent"].asString(),
              Json::FastWriter().write(infos).c_str());
        }

        ret["data"] = infos;
        ret["message"] = "success";
        ret["status"] = 1;
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
                               "chat detail latest redis error - {}:{}",
                               __FILE__, __LINE__);

          Json::Value ret;
          ret["message"] = "获取留言板附言失败";
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

void Detail::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";

    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto timestamp = common::utility::timeNow();

  auto db = app().getDbClient("postgresql-slave");

  switch (req->getMethod()) {
  case Post:
    db->execSqlAsync(
        "INSERT INTO chat_comments_detail VALUES ($1, $2, $3, NULL, NULL, $4, "
        "$5, $6, $7, 0)",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "新增失败";
            ret["status"] = 0;
          }

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
        common::utility::uuid(), (*json)["content"].asString(),
        (*json)["parent"].asString(), (*json)["create_user"].asString(),
        timestamp, (*json)["update_user"].asString(), timestamp);
    break;
  case Put:
    db->execSqlAsync(
        "UPDATE chat_comments_detail SET content = $1, like = $2, dislike = "
        "$3, update_user = $4, update_date = $5 WHERE \"id\" = $6",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            auto row = r[0];
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "更新失败";
            ret["status"] = 0;
          }

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
        (*json)["content"].asString(), (*json)["like"].asInt(),
        (*json)["dislike"].asInt(), (*json)["update_user"].asString(),
        (*json)["update_date"].asString(), (*json)["id"].asString());
    break;
  case Delete:
    db->execSqlAsync(
        "UPDATE chat_comments_detail SET \"delete\" = 1 WHERE \"id\" = $1",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            auto row = r[0];
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "删除失败";
            ret["status"] = 0;
          }

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
        (*json)["id"].asString());
    break;
  default:
    Json::Value ret;
    ret["message"] = "方法无效";
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}
} // namespace api
