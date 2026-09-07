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

  std::string stmt = "SELECT COUNT(id) FROM repair_order WHERE "
                     "\"delete\" = 0";

  if ((*json).find("plate"))
    stmt = fmt::format("{} AND plate LIKE '{}'", stmt,
                       (*json)["plate"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if ((*json).find("class"))
    stmt = fmt::format("{} AND class LIKE '{}'", stmt,
                       (*json)["class"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '{}'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("date_in"))
    stmt = fmt::format("{} AND date_in LIKE '{}'", stmt,
                       (*json)["date_in"].asString());

  if ((*json).find("date_settle"))
    stmt = fmt::format("{} AND date_settle LIKE '{}'", stmt,
                       (*json)["date_settle"].asString());

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

  std::string stmt = "SELECT * FROM repair_order WHERE \"delete\" = 0";

  if ((*json).find("plate"))
    stmt = fmt::format("{} AND plate LIKE '{}'", stmt,
                       (*json)["plate"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if ((*json).find("class"))
    stmt = fmt::format("{} AND class LIKE '{}'", stmt,
                       (*json)["class"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '{}'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("date_in"))
    stmt = fmt::format("{} AND date_in LIKE '{}'", stmt,
                       (*json)["date_in"].asString());

  if ((*json).find("date_settle"))
    stmt = fmt::format("{} AND date_settle LIKE '{}'", stmt,
                       (*json)["date_settle"].asString());

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
          info["plate"] = row["plate"].as<std::string>();
          info["type"] = row["type"].as<std::string>();
          info["date_in"] = row["date_in"].as<std::string>();
          info["date_settle"] = row["date_settle"].as<std::string>();
          info["class"] = row["class"].as<std::string>();
          info["vin"] = row["vin"].as<std::string>();
          info["mileage"] = row["mileage"].as<std::string>();
          info["phone"] = row["phone"].as<std::string>();
          info["project"] = row["project"].as<Json::Value>();
          info["parts"] = row["parts"].as<Json::Value>();
          info["cost_manhour"] = row["cost_manhour"].as<Json::Value>();
          info["cost_materials"] = row["cost_materials"].as<Json::Value>();
          info["cost_other"] = row["cost_other"].as<Json::Value>();
          info["amount_total"] = row["amount_total"].as<double>();
          info["amount_receipt"] = row["amount_receipt"].as<double>();
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

void Order::info(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback,
                 const std::string &id) const {
  switch (req->getMethod()) {
  case drogon::Get: {
    std::string stmt = "SELECT * FROM repair_order WHERE \"delete\" = 0";

    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "SELECT * FROM repair_order WHERE \"id\" = $1 AND \"delete\" = 0",
        [=](const orm::Result &r) {
          Json::Value ret;
          Json::Value info;

          if (r.size() > 0) {
            auto row = r[0];
            info["id"] = row["id"].as<std::string>();
            info["plate"] = row["plate"].as<std::string>();
            info["type"] = row["type"].as<std::string>();
            info["date_in"] = row["date_in"].as<std::string>();
            info["date_settle"] = row["date_settle"].as<std::string>();
            info["class"] = row["class"].as<std::string>();
            info["vin"] = row["vin"].as<std::string>();
            info["mileage"] = row["mileage"].as<std::string>();
            info["phone"] = row["phone"].as<std::string>();
            info["project"] = row["project"].as<Json::Value>();
            info["parts"] = row["parts"].as<Json::Value>();
            info["cost_manhour"] = row["cost_manhour"].as<Json::Value>();
            info["cost_materials"] = row["cost_materials"].as<Json::Value>();
            info["cost_other"] = row["cost_other"].as<Json::Value>();
            info["amount_total"] = row["amount_total"].as<double>();
            info["amount_receipt"] = row["amount_receipt"].as<double>();
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
        "INSERT INTO repair_order VALUES ($1, $2, $3, $4, $5, $6, $7, $8, "
        "$9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20, $21, "
        "$22, 0)",
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
        common::utility::uuid(), (*json)["plate"].asString(),
        (*json)["type"].asString(), (*json)["date_in"].asString(),
        (*json)["date_settle"].asString(), (*json)["class"].asString(),
        (*json)["vin"].asString(), (*json)["mileage"].asString(),
        (*json)["phone"].asString(), (*json)["project"].toStyledString(),
        (*json)["parts"].toStyledString(),
        (*json)["cost_manhour"].toStyledString(),
        (*json)["cost_materials"].toStyledString(),
        (*json)["cost_other"].toStyledString(),
        (*json)["amount_total"].asDouble(),
        (*json)["amount_receipt"].asDouble(), (*json)["status"].asString(),
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

    auto timestamp = common::utility::timeNow();

    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "UPDATE repair_order SET plate = $1, \"type\" = $2, date_in = "
        "$3, date_settle = $4, class = $5, vin = $6, mileage = $7, phone = $8, "
        "project = $9, parts = $10, cost_manhour = $11, cost_materials = $12, "
        "cost_other = $13, amount_total = $14, amount_receipt = $15, status = "
        "$16, remark = $17, update_date = $18, update_user = $19, \"delete\" = "
        "0 WHERE \"id\" = $20",
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
        (*json)["plate"].asString(), (*json)["type"].asString(),
        (*json)["date_in"].asString(), (*json)["date_settle"].asString(),
        (*json)["class"].asString(), (*json)["vin"].asString(),
        (*json)["mileage"].asString(), (*json)["phone"].asString(),
        (*json)["project"].toStyledString(), (*json)["parts"].toStyledString(),
        (*json)["cost_manhour"].toStyledString(),
        (*json)["cost_materials"].toStyledString(),
        (*json)["cost_other"].toStyledString(),
        (*json)["amount_total"].asDouble(),
        (*json)["amount_receipt"].asDouble(), (*json)["status"].asString(),
        (*json)["remark"].asString(), timestamp,
        (*json)["update_user"].asString(), (*json)["update_id"].asString(),
        (*json)["id"].asString());
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
