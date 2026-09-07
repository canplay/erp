#include "healthex.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void HealthEx::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM healthex WHERE \"delete\" = 0";

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
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

void HealthEx::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM healthex WHERE \"delete\" = 0";

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

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
          info["name"] = it["name"].as<std::string>();
          info["sex"] = it["sex"].as<std::string>();
          info["idcard"] = it["idcard"].as<std::string>();
          info["appointment"] = it["appointment"].as<std::string>();
          info["age"] = it["age"].as<std::string>();
          info["birthday"] = it["birthday"].as<std::string>();
          info["marital"] = it["marital"].as<std::string>();
          info["phone"] = it["phone"].as<std::string>();
          info["org1"] = it["org1"].as<std::string>();
          info["org2"] = it["org2"].as<std::string>();
          info["remark"] = it["remark"].as<std::string>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["create_user"] =
              co_await User::get_user(it["create_user"].as<std::string>());
          info["update_date"] = it["update_date"].as<std::string>();
          info["update_user"] =
              co_await User::get_user(it["update_user"].as<std::string>());
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

void HealthEx::info(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &id) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM healthex "
                                        "WHERE \"id\" = $1 AND \"delete\" = 0",
                                        id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["name"] = row["name"].as<std::string>();
        info["sex"] = row["sex"].as<std::string>();
        info["idcard"] = row["idcard"].as<std::string>();
        info["appointment"] = row["appointment"].as<std::string>();
        info["age"] = row["age"].as<std::string>();
        info["birthday"] = row["birthday"].as<std::string>();
        info["marital"] = row["marital"].as<std::string>();
        info["phone"] = row["phone"].as<std::string>();
        info["org1"] = row["org1"].as<std::string>();
        info["org2"] = row["org2"].as<std::string>();
        info["remark"] = row["remark"].as<std::string>();
        info["create_date"] = row["create_date"].as<std::string>();
        info["create_user"] =
            co_await User::get_user(row["create_user"].as<std::string>());
        info["update_date"] = row["update_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(row["update_user"].as<std::string>());
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

void HealthEx::update(
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
        "INSERT INTO healthex VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, "
        "$11, $12, $13, $14, $15, $16, 0)",
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
        id, (*json)["name"].asString(), (*json)["sex"].asString(),
        (*json)["idcard"].asString(), (*json)["appointment"].asString(),
        (*json)["age"].asString(), (*json)["birthday"].asString(),
        (*json)["marital"].asString(), (*json)["phone"].asString(),
        (*json)["org1"].asString(), (*json)["org2"].asString(),
        (*json)["remark"].asString(), (*json)["user"].asString(), timestamp,
        (*json)["user"].asString(), timestamp);
  } else if ((*json)["method"].asString() == "update") {
    db->execSqlAsync(
        "UPDATE healthex SET name = $1, sex = $2, idcard = $3, appointment = "
        "$4, age = $5, birthday = $6, marital = $7, phone = $8, org1 = $9, "
        "org2 = $10, remark = $11, update_user = $12, update_date = $13 WHERE "
        "\"id\" = $14",
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
        (*json)["name"].asString(), (*json)["sex"].asString(),
        (*json)["idcard"].asString(), (*json)["appointment"].asString(),
        (*json)["age"].asString(), (*json)["birthday"].asString(),
        (*json)["marital"].asString(), (*json)["phone"].asString(),
        (*json)["org1"].asString(), (*json)["org2"].asString(),
        (*json)["remark"].asString(), (*json)["user"].asString(), timestamp,
        (*json)["id"].asString());
  } else if ((*json)["method"].asString() == "delete") {
    db->execSqlAsync(
        "UPDATE healthex SET \"delete\" = 1, update_user "
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
