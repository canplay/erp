#include "log.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Log::count(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM log WHERE \"delete\" = 0";

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

void Log::list(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt =
      fmt::format("SELECT * FROM log WHERE class = {} AND \"delete\" = 0 "
                  "LIMIT {} OFFSET {}",
                  (*json)["class"].asInt(), (*json)["maxPage"].asString(),
                  (*json)["curPage"].asString());

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
            info["class"] = it["class"].as<int>();
            info["url"] = it["url"].as<std::string>();
            info["operator"] = it["operator"].as<std::string>();
            info["create_date"] = it["create_date"].as<std::string>();
            info["create_user"] = it["create_user"].as<std::string>();
            info["update_date"] = it["update_date"].as<std::string>();
            info["update_user"] = it["update_user"].as<std::string>();
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

void Log::info(const HttpRequestPtr &req,
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
      "SELECT * FROM log WHERE \"delete\" = 0 AND \"id\" = $1",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info["id"] = row["id"].as<std::string>();
          info["title"] = row["title"].as<std::string>();
          info["class"] = row["class"].as<int>();
          info["url"] = row["url"].as<std::string>();
          info["operator"] = row["operator"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
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

void Log::update(
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

  switch ((*req).method()) {
  case Post: {
    auto id = common::utility::uuid();

    db->execSqlAsync(
        "INSERT INTO log VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
        [=](const orm::Result &r) {
          Json::Value ret;
          ret["message"] = "success";
          ret["status"] = 1;

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
        id, (*json)["title"].asString(), (*json)["class"].asInt(),
        (*json)["url"].asString(), (*json)["operator"].asString(), timestamp,
        (*json)["create_user"].asString(), timestamp,
        (*json)["create_user"].asString());
    break;
  }
  case Put:
    db->execSqlAsync(
        "UPDATE log SET title = $1, class = $2, url = $3, operator = $4, "
        "update_user = $5, update_date = $6 WHERE \"id\" = $7",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
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
        (*json)["title"].asString(), (*json)["class"].asInt(),
        (*json)["url"].asString(), (*json)["operator"].asString(),
        (*json)["update_user"].asString(), timestamp, (*json)["id"].asString());
    break;
  default:
    Json::Value ret;
    ret["message"] = "方法无效";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
    break;
  }
}
} // namespace api
