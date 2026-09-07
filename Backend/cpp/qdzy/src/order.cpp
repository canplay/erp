#include "order.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Order::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM qdzy_order WHERE \"delete\" = 0";

  if ((*json).find("zone"))
    stmt = fmt::format("{} AND zone = {}", stmt, (*json)["zone"].asString());

  if ((*json).find("service"))
    stmt =
        fmt::format("{} AND service = {}", stmt, (*json)["service"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

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

void Order::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM qdzy_order WHERE \"delete\" = 0";

  if ((*json).find("zone"))
    stmt = fmt::format("{} AND zone = {}", stmt, (*json)["zone"].asString());

  if ((*json).find("service"))
    stmt =
        fmt::format("{} AND service = {}", stmt, (*json)["service"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

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
          info["zone"] = it["zone"].as<std::string>();
          info["service"] = it["service"].as<std::string>();
          info["name"] = it["name"].as<std::string>();
          info["phone"] = it["phone"].as<std::string>();
          info["address"] = it["address"].as<std::string>();
          info["remark"] = it["remark"].as<std::string>();
          info["src"] = it["src"].as<std::string>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["create_user"] = it["create_user"].as<std::string>();
          info["update_date"] = it["update_date"].as<std::string>();
          info["update_user"] = it["update_user"].as<std::string>();
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

void Order::info(
    const HttpRequestPtr &req,
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
      "SELECT * FROM qdzy_order WHERE \"delete\" = 0 AND \"id\" = $1",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info["id"] = row["id"].as<std::string>();
          info["zone"] = row["zone"].as<std::string>();
          info["service"] = row["service"].as<std::string>();
          info["name"] = row["name"].as<std::string>();
          info["phone"] = row["phone"].as<std::string>();
          info["address"] = row["address"].as<std::string>();
          info["remark"] = row["remark"].as<std::string>();
          info["src"] = row["src"].as<std::string>();
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

void Order::create(
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
      "INSERT INTO qdzy_order VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, "
      "$10, 0, $11, $12)",
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
      common::utility::uuid(), (*json)["zone"].asString(),
      (*json)["service"].asString(), (*json)["name"].asString(),
      (*json)["phone"].asString(), (*json)["address"].asString(),
      (*json)["create_user"].asString(), timestamp,
      (*json)["update_user"].asString(), timestamp,
      (*json)["remark"].asString(), (*json)["src"].asString());
}

void Order::update(
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
        "UPDATE qdzy_order SET zone = $1, service = $2, name = $3, phone "
        "= $4, \"address\" = $5, remark = $6, src = $7, update_date = $8, "
        "update_user = $9 WHERE \"id\" = $10",
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
        (*json)["zone"].asString(), (*json)["service"].asString(),
        (*json)["name"].asString(), (*json)["phone"].asString(),
        (*json)["address"].asString(), (*json)["remark"].asString(),
        (*json)["src"].asString(), timestamp, (*json)["update_user"].asString(),
        (*json)["id"].asString());
  } break;
  case Delete: {
    db->execSqlAsync(
        "UPDATE qdzy_order SET delete = 1 WHERE \"id\" = $1",
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
