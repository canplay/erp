#include "news.h"
#include "include/utility.h"
#include "pch.h"
#include "json/value.h"

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
          "GET qdzy_news:latest:%d", (*json)["type"].asInt());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");

        auto stmt = fmt::format("SELECT * FROM qdzy_news WHERE \"delete\" = 0");

        if ((*json).find("top"))
          stmt = fmt::format("{} AND top = {}", stmt, (*json)["top"].asInt());

        if ((*json).find("type"))
          stmt = fmt::format("{} AND type = {}", stmt, (*json)["type"].asInt());

        stmt = fmt::format("{} ORDER BY create_date DESC LIMIT 5", stmt);

        auto r = co_await db->execSqlCoro(stmt);
        Json::Value ret;
        Json::Value infos;

        for (auto it : r) {
          Json::Value info;
          info["id"] = it["id"].as<std::string>();
          info["title"] = it["title"].as<std::string>();
          info["preview"] = it["preview"].as<std::string>();
          info["content"] = it["content"].as<std::string>();
          info["top"] = it["top"].as<int>();
          info["type"] = it["type"].as<int>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["create_user"] = it["create_user"].as<std::string>();
          info["update_date"] = it["update_date"].as<std::string>();
          info["update_user"] = it["update_user"].as<std::string>();
          info["slide"] = it["slide"].as<Json::Value>();
          infos.append(info);
        }

        auto redisClient = app().getRedisClient("redis-master");
        co_await redisClient->execCommandCoro(
            "SET qdzy_news:latest:%d %s EX 600", (*json)["type"].asInt(),
            Json::FastWriter().write(infos).c_str());

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

  std::string stmt = "SELECT COUNT(id) FROM qdzy_news WHERE \"delete\" = 0";

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

  if ((*json).find("top"))
    stmt = fmt::format("{} AND top = {}", stmt, (*json)["top"].asInt());

  if ((*json).find("type"))
    stmt = fmt::format("{} AND type = {}", stmt, (*json)["type"].asInt());

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

void News::list(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM qdzy_news WHERE \"delete\" = 0";

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

  if ((*json).find("top"))
    stmt = fmt::format("{} AND top = {}", stmt, (*json)["top"].asInt());

  if ((*json).find("type"))
    stmt = fmt::format("{} AND type = {}", stmt, (*json)["type"].asInt());

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
          info["title"] = it["title"].as<std::string>();
          info["preview"] = it["preview"].as<std::string>();
          info["content"] = it["content"].as<std::string>();
          info["top"] = it["top"].as<int>();
          info["type"] = it["type"].as<int>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["create_user"] = it["create_user"].as<std::string>();
          info["update_date"] = it["update_date"].as<std::string>();
          info["update_user"] = it["update_user"].as<std::string>();
          info["slide"] = it["slide"].as<Json::Value>();
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

  auto db = app().getDbClient("postgresql-master");
  db->execSqlAsync(
      "SELECT * FROM qdzy_news WHERE \"delete\" = 0 AND \"id\" = $1",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info["id"] = row["id"].as<std::string>();
          info["title"] = row["title"].as<std::string>();
          info["preview"] = row["preview"].as<std::string>();
          info["content"] = row["content"].as<std::string>();
          info["top"] = row["top"].as<int>();
          info["type"] = row["type"].as<int>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["slide"] = row["slide"].as<Json::Value>();
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

void News::create(
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

  auto db = app().getDbClient("postgresql-master");

  db->execSqlAsync(
      "INSERT INTO qdzy_news VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, "
      "$10, 0, $11)",
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.affectedRows() > 0) {
          ret["message"] = "success";
          ret["status"] = 1;
        } else {
          ret["message"] = "新增失败";
          ret["status"] = 0;
        }

        return callback(HttpResponse::newHttpJsonResponse(ret));
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      common::utility::uuid(), (*json)["title"].asString(),
      (*json)["preview"].asString(), (*json)["content"].asString(),
      (*json)["top"].asInt(), (*json)["type"].asInt(), timestamp,
      (*json)["create_user"].asString(), timestamp,
      (*json)["update_user"].asString(), (*json)["slide"].toStyledString());
}

void News::update(
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

  auto db = app().getDbClient("postgresql-master");

  switch (req->getMethod()) {
  case Put: {
    db->execSqlAsync(
        "UPDATE qdzy_news SET title = $1, preview = $2, content = $3, top "
        "= $4, \"type\" = $5, update_date = $6, update_user = $7, slide = $8 "
        "WHERE \"id\" = $9",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "更新失败";
            ret["status"] = 0;
          }

          return callback(HttpResponse::newHttpJsonResponse(ret));
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
        (*json)["title"].asString(), (*json)["preview"].asString(),
        (*json)["content"].asString(), (*json)["top"].asInt(),
        (*json)["type"].asInt(), timestamp, (*json)["update_user"].asString(),
        (*json)["slide"].toStyledString(), (*json)["id"].asString());
  } break;
  case Delete: {
    db->execSqlAsync(
        "UPDATE qdzy_news SET delete = 1 WHERE \"id\" = $1",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "删除失败";
            ret["status"] = 0;
          }

          return callback(HttpResponse::newHttpJsonResponse(ret));
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
  } break;
  default: {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  } break;
  }
}
} // namespace api
