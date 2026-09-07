#include "customer.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void Customer::supplier_count(
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
      "SELECT COUNT(id) FROM customer_supplier WHERE \"delete\" = 0";

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("representative"))
    stmt = fmt::format("{} AND representative LIKE '%{}%'", stmt,
                       (*json)["representative"].asString());

  if ((*json).find("agent"))
    stmt = fmt::format("{} AND agent LIKE '%{}%'", stmt,
                       (*json)["agent"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

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

void Customer::supplier_list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM customer_supplier WHERE \"delete\" = 0";

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("representative"))
    stmt = fmt::format("{} AND representative LIKE '%{}%'", stmt,
                       (*json)["representative"].asString());

  if ((*json).find("agent"))
    stmt = fmt::format("{} AND agent LIKE '%{}%'", stmt,
                       (*json)["agent"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

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
          info["representative"] = it["representative"].as<std::string>();
          info["address"] = it["address"].as<std::string>();
          info["capital"] = it["capital"].as<int>();
          info["agent"] = it["agent"].as<Json::Value>();
          info["phone"] = it["phone"].as<std::string>();
          info["class1"] = it["class1"].as<std::string>();
          info["class2"] = it["class2"].as<std::string>();
          info["upload"] = it["upload"].as<Json::Value>();
          info["type1"] = it["type1"].as<std::string>();
          info["type2"] = it["type2"].as<std::string>();
          info["score"] = it["score"].as<int>();
          info["level"] = it["level"].as<std::string>();
          info["reward"] = it["reward"].as<std::string>();
          info["examine"] = it["examine"].as<std::string>();
          info["remark"] = it["remark"].as<std::string>();
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

void Customer::supplier_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    const std::string &id) const {
  auto timestamp = common::utility::timeNow();

  switch (req->getMethod()) {
  case drogon::Get: {
    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "SELECT * FROM customer_supplier WHERE \"delete\" = 0 AND \"id\" = "
        "$1",
        [=](const orm::Result &r) {
          Json::Value ret;
          Json::Value info;

          if (r.size() > 0) {
            auto row = r[0];
            info["id"] = row["id"].as<std::string>();
            info["name"] = row["name"].as<std::string>();
            info["representative"] = row["representative"].as<std::string>();
            info["address"] = row["address"].as<std::string>();
            info["capital"] = row["capital"].as<int>();
            info["agent"] = row["agent"].as<Json::Value>();
            info["phone"] = row["phone"].as<std::string>();
            info["class1"] = row["class1"].as<std::string>();
            info["class2"] = row["class2"].as<std::string>();
            info["upload"] = row["upload"].as<Json::Value>();
            info["type1"] = row["type1"].as<std::string>();
            info["type2"] = row["type2"].as<std::string>();
            info["score"] = row["score"].as<int>();
            info["level"] = row["level"].as<std::string>();
            info["reward"] = row["reward"].as<std::string>();
            info["examine"] = row["examine"].as<std::string>();
            info["remark"] = row["remark"].as<std::string>();
            info["create_date"] = row["create_date"].as<std::string>();
            info["create_user"] =
                User::get_user(row["create_user"].as<std::string>())
                    .coro_.promise()
                    .value->as<std::string>();
            info["update_date"] = row["update_date"].as<std::string>();
            info["update_user"] =
                User::get_user(row["update_user"].as<std::string>())
                    .coro_.promise()
                    .value->as<std::string>();
            info["delete"] = row["delete"].as<int>();
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

    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "INSERT INTO customer_supplier VALUES ($1, $2, $3, $4, $5, $6, $7, $8, "
        "$9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20, $21, 0)",
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
        common::utility::uuid(), (*json)["name"].asString(),
        (*json)["representative"].asString(), (*json)["address"].asString(),
        (*json)["capital"].asInt(), (*json)["agent"].asString(),
        (*json)["phone"].asDouble(), (*json)["class1"].asString(),
        (*json)["class2"].asString(), (*json)["upload"].toStyledString(),
        (*json)["type1"].asString(), (*json)["type2"].asString(),
        (*json)["score"].asInt(), (*json)["level"].asString(),
        (*json)["reward"].asString(), (*json)["examine"].asString(),
        (*json)["remark"].asString(), timestamp,
        (*json)["create_user"].asString(), timestamp,
        (*json)["update_user"].asString());
  } break;
  case drogon::Post: {
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    if (!json) {
      Json::Value ret;
      ret["message"] = "参数错误";

      ret["status"] = 0;
      return callback(HttpResponse::newHttpJsonResponse(ret));
    }

    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "UPDATE customer_supplier SET name = $1, representative = "
        "$2, address = $3, capital = $4, agent = $5, phone = $6, class1 = $7, "
        "class2 = $8, upload = $9, type1 = $10, type2 = $11, score = $12, "
        "level = $13, reward = $14, examine = $15, remark = $16, update_date = "
        "$17, update_user = $18, \"delete\" = 0 WHERE \"id\" = $19",
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
        (*json)["name"].asString(), (*json)["representative"].asString(),
        (*json)["address"].asString(), (*json)["capital"].asInt(),
        (*json)["agent"].asString(), (*json)["phone"].asString(),
        (*json)["class1"].asString(), (*json)["class2"].asString(),
        (*json)["upload"].toStyledString(), (*json)["type1"].asString(),
        (*json)["type2"].asString(), (*json)["score"].asInt(),
        (*json)["level"].asString(), (*json)["reward"].asString(),
        (*json)["examine"].asString(), (*json)["remark"].asString(), timestamp,
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
