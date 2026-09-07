#include "groupbuy.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void GroupBuy::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM groupbuy WHERE \"delete\" = 0";

  if ((*json).find("project"))
    stmt = fmt::format("{} AND project LIKE '%{}%'", stmt,
                       (*json)["project"].asString());

  if ((*json).find("company"))
    stmt = fmt::format("{} AND company LIKE '%{}%'", stmt,
                       (*json)["company"].asString());

  if ((*json).find("contact"))
    stmt = fmt::format("{} AND contact LIKE '%{}%'", stmt,
                       (*json)["contact"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

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

void GroupBuy::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM groupbuy WHERE \"delete\" = 0";

  if ((*json).find("project"))
    stmt = fmt::format("{} AND project LIKE '%{}%'", stmt,
                       (*json)["project"].asString());

  if ((*json).find("company"))
    stmt = fmt::format("{} AND company LIKE '%{}%'", stmt,
                       (*json)["company"].asString());

  if ((*json).find("contact"))
    stmt = fmt::format("{} AND contact LIKE '%{}%'", stmt,
                       (*json)["contact"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

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

  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(stmt);

      Json::Value ret;
      Json::Value infos;

      if (r.size() > 0) {
        for (auto it : r) {
          Json::Value info;
          info["id"] = it["id"].as<std::string>();
          info["project"] = it["project"].as<std::string>();
          info["contact"] = it["contact"].as<std::string>();
          info["phone"] = it["phone"].as<std::string>();
          info["company"] = it["company"].as<std::string>();
          info["items"] = it["items"].as<Json::Value>();
          info["amount"] = it["amount"].as<int>();
          info["delivery"] = it["delivery"].as<std::string>();
          info["status"] = it["status"].as<std::string>();
          info["remark"] = it["remark"].as<std::string>();
          info["price_range"] = it["price_range"].as<std::string>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["create_user"] =
              co_await User::get_user(it["create_user"].as<std::string>());
          info["update_date"] = it["update_date"].as<std::string>();
          info["update_user"] =
              co_await User::get_user(it["update_user"].as<std::string>());
          info["delete"] = it["delete"].as<int>();
          infos.append(info);
        }
      }

      ret["data"] = infos;
      ret["message"] = "success";
      ret["status"] = 1;
      callback(HttpResponse::newHttpJsonResponse(ret));
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

void GroupBuy::info(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &id) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM groupbuy "
                                        "WHERE \"id\" = $1 AND \"delete\" = 0",
                                        id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["project"] = row["project"].as<std::string>();
        info["contact"] = row["contact"].as<std::string>();
        info["phone"] = row["phone"].as<std::string>();
        info["company"] = row["company"].as<std::string>();
        info["items"] = row["items"].as<Json::Value>();
        info["amount"] = row["amount"].as<int>();
        info["delivery"] = row["delivery"].as<std::string>();
        info["status"] = row["status"].as<std::string>();
        info["remark"] = row["remark"].as<std::string>();
        info["price_range"] = row["price_range"].as<std::string>();
        info["create_date"] = row["create_date"].as<std::string>();
        info["create_user"] =
            co_await User::get_user(row["create_user"].as<std::string>());
        info["update_date"] = row["update_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(row["update_user"].as<std::string>());
        info["delete"] = row["delete"].as<int>();
      }

      ret["data"] = info;
      ret["message"] = "success";
      ret["status"] = 1;
      callback(HttpResponse::newHttpJsonResponse(ret));
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

void GroupBuy::update(
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

  if ((*json)["method"].asString() == "add") {
    auto id = common::utility::uuid();

    db->execSqlAsync(
        "INSERT INTO groupbuy VALUES ($1, $2, $3, $4, $5, $6, $7, $8, "
        "'未接单', $9, $10, $11, $12, $13, $14, 0)",
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
        id, (*json)["project"].asString(), (*json)["contact"].asString(),
        (*json)["phone"].asString(), (*json)["company"].asString(),
        (*json)["items"].toStyledString(), (*json)["amount"].asInt(),
        (*json)["delivery"].asString(), (*json)["remark"].asString(),
        (*json)["price_rage"].asString(), (*json)["user"].asString(), timestamp,
        (*json)["user"].asString(), timestamp);
  } else if ((*json)["method"].asString() == "update") {
    db->execSqlAsync(
        "UPDATE groupbuy SET project = $1, contact = $2, phone = $3, company = "
        "$4, items = $5, amount = $6, delivery = $7, status = $8, remark = $9, "
        "price_rage = $10, update_user = $11, update_date = $12 WHERE \"id\" = "
        "$13",
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
        (*json)["project"].asString(), (*json)["contact"].asString(),
        (*json)["phone"].asString(), (*json)["company"].asString(),
        (*json)["items"].toStyledString(), (*json)["amount"].asInt(),
        (*json)["delivery"].asString(), (*json)["status"].asString(),
        (*json)["remark"].asString(), (*json)["price_rage"].asString(),
        (*json)["user"].asString(), timestamp, (*json)["id"].asString());
  } else if ((*json)["method"].asString() == "delete") {
    db->execSqlAsync(
        "UPDATE groupbuy SET \"delete\" = 1, update_user "
        "= $1, update_date = $2 WHERE \"id\" = $3",
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
        (*json)["user"].asString(), timestamp, (*json)["id"].asString());
  } else {
    Json::Value ret;
    ret["message"] = "方法无效";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}
} // namespace api
