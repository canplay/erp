#include "funeral_order.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void FuneralOrder::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM funeral_order WHERE "
                     "\"delete\" = 0";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  if ((*json).find("create_date"))
    stmt = fmt::format("{} AND create_date LIKE '%{}%'", stmt,
                       (*json)["create_date"].asString());

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

void FuneralOrder::add(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Value goods;
  goods = (*json)["goods"];

  auto timestamp = common::utility::timeNow();

  auto db = app().getDbClient("postgresql-master");
  db->execSqlAsync(
      "INSERT INTO funeral_order VALUES ($1, $2, $3, $4, $5, "
      "$6, $7, $8, $9, $10)",
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
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["id"].asString(), common::utility::jsonToString(goods),
      (*json)["money"].asString(), (*json)["remark"].asString(),
      (*json)["status"].asString(), timestamp,
      (*json)["create_user"].asString(), timestamp,
      (*json)["update_user"].asString(), false);
}

void FuneralOrder::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Value goods;
  goods = (*json)["goods"];

  auto timestamp = common::utility::timeNow();

  auto db = app().getDbClient("postgresql-master");
  db->execSqlAsync(
      "UPDATE funeral_order SET goods = $1, money = $2, remark = $3, status = "
      "$4, update_user = $5, update_date = $6, \"delete\" = $7 WHERE \"id\" = "
      "$8",
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
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      common::utility::jsonToString(goods), (*json)["money"].asInt64(),
      (*json)["remark"].asString(), (*json)["status"].asString(),
      (*json)["update_user"].asString(), timestamp, (*json)["delete"].asBool(),
      (*json)["id"].asString());
}

void FuneralOrder::info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM funeral_order WHERE \"delete\" = 0";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  if ((*json).find("create_date"))
    stmt = fmt::format("{} AND create_date LIKE '%{}%'", stmt,
                       (*json)["create_date"].asString());

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
          info["goods"] = row["goods"].as<Json::Value>();
          info["money"] = row["money"].as<int64_t>();
          info["remark"] = row["remark"].as<std::string>();
          info["status"] = row["status"].as<std::string>();
          info["delete"] = row["delete"].as<bool>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();

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
} // namespace api
