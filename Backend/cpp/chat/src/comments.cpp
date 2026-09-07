#include "comments.h"
#include "drogon/HttpTypes.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Comments::latest(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  async_run([=]() -> Task<> {
    try {
      auto redisClient = app().getRedisClient("redis-slave");
      auto redis_result =
          co_await redisClient->execCommandCoro("GET chat:comments:latest");

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(
            "WITH filtered_data AS (SELECT * FROM chat_comments WHERE "
            "end_date> CURRENT_DATE), ranked_data AS (SELECT *, ROW_NUMBER() "
            "OVER (PARTITION BY create_user ORDER BY create_date DESC) AS "
            "dup_rank FROM filtered_data) SELECT * FROM ranked_data WHERE "
            "dup_rank = 1 ORDER BY create_date DESC;");

        Json::Value ret;
        Json::Value infos;

        if (r.size() > 0) {
          for (auto it : r) {
            Json::Value info;
            info["id"] = it["id"].as<std::string>();
            info["content"] = it["content"].as<std::string>();
            info["like"] = it["like"].as<int>();
            info["dislike"] = it["dislike"].as<int>();
            info["create_user"] = it["create_user"].as<std::string>();
            info["create_date"] = it["create_date"].as<std::string>();
            info["update_user"] = it["update_user"].as<std::string>();
            info["update_date"] = it["update_date"].as<std::string>();
            info["delete"] = it["delete"].as<int>();
            info["lng"] = it["lng"].as<std::string>();
            info["lat"] = it["lat"].as<std::string>();
            info["end_date"] = it["end_date"].as<std::string>();
            infos.append(info);
          }

          auto redisClient = app().getRedisClient("redis-master");
          co_await redisClient->execCommandCoro(
              "SET chat:comments:latest %s EX 600",
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
                               "chat comments latest redis error - {}:{}",
                               __FILE__, __LINE__);

          Json::Value ret;
          ret["message"] = "获取留言板失败";
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

void Comments::update(
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
        "INSERT INTO chat_comments VALUES ($1, $2, NULL, NULL, $3, $4, "
        "$5, $6, 0, $7, $8, $9)",
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
        (*json)["create_user"].asString(), timestamp,
        (*json)["update_user"].asString(), timestamp, (*json)["lng"].asString(),
        (*json)["lat"].asString(), (*json)["end_date"].asString());
    break;
  case Put:
    db->execSqlAsync(
        "UPDATE chat_comments SET content = $1, like = $2, dislike = $3, "
        "update_user = $4, update_date = $5, end_date = $6 WHERE \"id\" = $7",
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
        (*json)["update_date"].asString(), (*json)["end_date"].asString(),
        (*json)["id"].asString());
    break;
  case Delete:
    db->execSqlAsync(
        "UPDATE chat_comments SET \"delete\" = 1 WHERE \"id\" = $1",
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
