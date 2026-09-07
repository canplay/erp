#include "news.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void News::latest(
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
          "GET news:latest:%d", (*json)["class"].asInt());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(
            "SELECT * FROM news WHERE class = $1 AND \"delete\" = 0 ORDER BY "
            "create_date DESC LIMIT 5",
            (*json)["class"].asInt());

        Json::Value ret;
        Json::Value infos;

        if (r.size() > 0) {
          for (auto it : r) {
            Json::Value info;
            info["id"] = it["id"].as<std::string>();
            info["title"] = it["title"].as<std::string>();
            info["date"] = it["date"].as<std::string>();
            info["author"] = it["author"].as<std::string>();
            info["content"] = it["content"].as<std::string>();
            info["create_date"] = it["create_date"].as<std::string>();
            info["create_user"] = it["create_user"].as<std::string>();
            info["update_date"] = it["update_date"].as<std::string>();
            info["update_user"] = it["update_user"].as<std::string>();
            info["preview"] = it["preview"].as<std::string>();
            info["class"] = it["class"].as<int>();
            info["hot"] = it["hot"].as<bool>();
            infos.append(info);
          }

          auto redisClient = app().getRedisClient("redis-master");
          co_await redisClient->execCommandCoro(
              "SET news:latest:%d %s EX 600", (*json)["class"].asInt(),
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
                               "news latest redis error - {}:{}", __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = "获取新闻缓存失败";
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

void News::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT COUNT(id) FROM news WHERE class = $1 AND \"delete\" = 0",
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
      (*json)["class"].asInt());
}

void News::list(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM news WHERE \"delete\" = 0";

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

  if ((*json).find("author"))
    stmt = fmt::format("{} AND author LIKE '%{}%'", stmt,
                       (*json)["author"].asString());

  if ((*json).find("date"))
    stmt = fmt::format("{} AND date LIKE '%{}%'", stmt,
                       (*json)["date"].asString());

  if ((*json).find("class"))
    stmt = fmt::format("{} AND class = {}", stmt, (*json)["class"].asInt());

  if ((*json).find("hot"))
    stmt = fmt::format("{} AND hot = {}", stmt, (*json)["hot"].asInt());

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

        if (r.size() > 0) {
          for (auto it : r) {
            Json::Value info;
            info["id"] = it["id"].as<std::string>();
            info["title"] = it["title"].as<std::string>();
            info["date"] = it["date"].as<std::string>();
            info["author"] = it["author"].as<std::string>();
            info["content"] = it["content"].as<std::string>();
            info["create_date"] = it["create_date"].as<std::string>();
            info["create_user"] = it["create_user"].as<std::string>();
            info["update_date"] = it["update_date"].as<std::string>();
            info["update_user"] = it["update_user"].as<std::string>();
            info["preview"] = it["preview"].as<std::string>();
            info["class"] = it["class"].as<int>();
            info["hot"] = it["hot"].as<bool>();
            infos.append(info);
          }
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

void News::info(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string id = (*json)["id"].asString();

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM news WHERE \"delete\" = 0 AND \"id\" = $1",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info["id"] = row["id"].as<std::string>();
          info["title"] = row["title"].as<std::string>();
          info["date"] = row["date"].as<std::string>();
          info["author"] = row["author"].as<std::string>();
          info["content"] = row["content"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["preview"] = row["preview"].as<std::string>();
          info["class"] = row["class"].as<int>();
          info["hot"] = row["hot"].as<bool>();
        }

        ret["data"] = info;
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
      },
      id);
}
} // namespace api
