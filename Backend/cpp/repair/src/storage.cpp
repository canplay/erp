#include "storage.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Storage::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt =
      "SELECT COUNT(id) FROM repair_storage WHERE \"delete\" = 0";

  if ((*json).find("code"))
    stmt = fmt::format("{} AND code LIKE '%{}%'", stmt,
                       (*json)["code"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND type LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

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

void Storage::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM repair_storage WHERE \"delete\" = 0";

  if ((*json).find("code"))
    stmt = fmt::format("{} AND code LIKE '%{}%'", stmt,
                       (*json)["code"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND type LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("sortBy")) {
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

    if ((*json)["descending"].asBool())
      stmt = fmt::format("{} DESC", stmt);
    else
      stmt = fmt::format("{} ASC", stmt);
  }

  stmt =
      fmt::format("{} LIMIT {} OFFSET {}", stmt, (*json)["maxPage"].asString(),
                  (*json)["curPage"].asString());

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<std::string>();
          info["code"] = row["code"].as<std::string>();
          info["name"] = row["name"].as<std::string>();
          info["tag"] = row["tag"].as<std::string>();
          info["num"] = row["num"].as<int>();
          info["unit"] = row["unit"].as<std::string>();
          info["price"] = row["price"].as<double>();
          info["date_in"] = row["date_in"].as<std::string>();
          info["date_out"] = row["date_out"].as<std::string>();
          info["status"] = row["status"].as<std::string>();
          info["remark"] = row["remark"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();

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

void Storage::info(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   const std::string &id) const {
  switch (req->getMethod()) {
  case drogon::Get: {
    std::string stmt = "SELECT * FROM repair_storage WHERE \"delete\" = 0";

    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "SELECT * FROM repair_storage WHERE \"id\" = $1 AND \"delete\" = 0",
        [=](const orm::Result &r) {
          Json::Value ret;
          Json::Value info;

          if (r.size() > 0) {
            auto row = r[0];
            info["id"] = row["id"].as<std::string>();
            info["code"] = row["code"].as<std::string>();
            info["name"] = row["name"].as<std::string>();
            info["tag"] = row["tag"].as<std::string>();
            info["num"] = row["num"].as<int>();
            info["unit"] = row["unit"].as<std::string>();
            info["price"] = row["price"].as<double>();
            info["date_in"] = row["date_in"].as<std::string>();
            info["date_out"] = row["date_out"].as<std::string>();
            info["status"] = row["status"].as<std::string>();
            info["remark"] = row["remark"].as<std::string>();
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
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = e.base().what();
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        },
        id);
  } break;
  case drogon::Put: {
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    if (!json) {
      Json::Value ret;
      ret["message"] = "参数错误";
      ret["status"] = 0;
      return callback(HttpResponse::newHttpJsonResponse(ret));
    }

    auto timestamp = common::utility::timeNow();

    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "INSERT INTO repair_storage VALUES ($1, $2, $3, $4, $5, $6, $7, $8, "
        "$9, $10, $11, $12, $13, $14, $15, $16, 0)",
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
        common::utility::uuid(), (*json)["code"].asString(),
        (*json)["name"].asString(), (*json)["tag"].asString(),
        (*json)["num"].asInt(), (*json)["unit"].asString(),
        (*json)["price"].asDouble(), (*json)["date_in"].asString(),
        (*json)["date_out"].asString(), (*json)["status"].asString(),
        (*json)["remark"].asString(), timestamp,
        (*json)["create_user"].asString(), timestamp,
        (*json)["update_user"].asString(), (*json)["img"].asString());
  } break;
  case drogon::Post: {
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    if (!json) {
      Json::Value ret;
      ret["message"] = "参数错误";
      ret["status"] = 0;
      return callback(HttpResponse::newHttpJsonResponse(ret));
    }

    auto timestamp = common::utility::timeNow();

    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "UPDATE repair_storage SET code = $1, name = "
        "$2, tag = $3, num = $4, unit = $5, price = $6, img = $7, "
        "date_in = $8, date_out = $9, status = $10, remark = $11, "
        "update_date = $12, update_user = $13, \"delete\" = 0 WHERE \"id\" = "
        "$14",
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
        (*json)["code"].asString(), (*json)["name"].asString(),
        (*json)["tag"].asString(), (*json)["num"].asInt(),
        (*json)["unit"].asString(), (*json)["price"].asDouble(),
        (*json)["img"].asString(), (*json)["date_in"].asString(),
        (*json)["date_out"].asString(), (*json)["status"].asString(),
        (*json)["remark"].asString(), timestamp,
        (*json)["update_user"].asString(), (*json)["id"].asString());
  } break;
  default:
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
    break;
  }
}
} // namespace api
