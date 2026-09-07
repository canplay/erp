#include "invoice.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Invoice::count(
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
      "SELECT COUNT(id) FROM clean_invoice WHERE \"delete\" = FALSE";

  if ((*json)["id"].asString() != "")
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json)["no"].asString() != "")
    stmt = fmt::format("{} AND no LIKE '%{}%'", stmt, (*json)["no"].asString());

  if ((*json)["imposing_no"].asInt64() != 0)
    stmt = fmt::format("{} AND imposing_no = {}", stmt,
                       (*json)["imposing_no"].asInt64());

  if ((*json)["imposing_name"].asString() != "")
    stmt = fmt::format("{} AND imposing_name LIKE '%{}%'", stmt,
                       (*json)["imposing_name"].asString());

  if ((*json)["collection_name"].asString() != "")
    stmt = fmt::format("{} AND collection_name LIKE '%{}%'", stmt,
                       (*json)["collection_name"].asString());

  if ((*json)["fingerprint"].asString() != "")
    stmt = fmt::format("{} AND fingerprint LIKE '%{}%'", stmt,
                       (*json)["fingerprint"].asString());

  if ((*json)["create_date"].asString() != "")
    stmt = fmt::format("{} AND create_date LIKE '%{}%'", stmt,
                       (*json)["create_date"].asString());

  if ((*json)["zone"].asString() != "")
    stmt = fmt::format("{} AND zone LIKE '%{}%'", stmt,
                       (*json)["zone"].asString());

  if ((*json)["sum"].asString() != "")
    stmt =
        fmt::format("{} AND sum LIKE '%{}%'", stmt, (*json)["sum"].asString());

  if ((*json)["sum_capital"].asString() != "")
    stmt = fmt::format("{} AND sum_capital LIKE '%{}%'", stmt,
                       (*json)["sum_capital"].asString());

  if ((*json)["remark"].asString() != "")
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  if ((*json)["review"].asString() != "")
    stmt = fmt::format("{} AND review LIKE '%{}%'", stmt,
                       (*json)["review"].asString());

  if ((*json)["operator"].asString() != "")
    stmt = fmt::format("{} AND operator LIKE '%{}%'", stmt,
                       (*json)["operator"].asString());

  if ((*json)["create_user"].asString() != "")
    stmt = fmt::format("{} AND create_user LIKE '%{}%'", stmt,
                       (*json)["create_user"].asString());

  if ((*json)["sortBy"].asString() != "")
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

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

void Invoice::update(
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
  if ((*json)["id"].asString() == "") {
    db->execSqlAsync(
        "INSERT INTO clean_invoice VALUES ($1, $2, $3, $4, $5, $6, $7, "
        "$8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, false, "
        "false)",
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
        common::utility::uuid(), (*json)["no"].asString(),
        (*json)["imposing_no"].asInt64(), (*json)["imposing_name"].asString(),
        common::utility::uuid(), (*json)["create_date"].asString(),
        (*json)["zone"].asString(), (*json)["payer"].asString(),
        (*json)["sum"].asString(), (*json)["sum_capital"].asString(),
        (*json)["remark"].asString(), (*json)["review"].asString(),
        (*json)["operator"].asString(), (*json)["create_user"].asString(),
        (*json)["create_date"].asString(), (*json)["create_user"].asString(),
        (*json)["collection_name"].asString(), (*json)["print"].asInt64(),
        (*json)["project"].toStyledString());
  } else {
    db->execSqlAsync(
        "UPDATE clean_invoice SET no = $1, imposing_no = $2, imposing_name "
        "= $3, fingerprint = $4, zone = $5, payer = $6, sum = $7, sum_capital "
        "= $8, remark = $9, review = $10, operator = $11, collection_name = "
        "$12, print = $13, project = $14, invalid = $15, update_user = $16, "
        "update_date = $17, create_date = $18, \"delete\" = $19 WHERE \"id\" = "
        "$20",
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
        (*json)["no"].asString(), (*json)["imposing_no"].asInt64(),
        (*json)["imposing_name"].asString(), common::utility::uuid(),
        (*json)["zone"].asString(), (*json)["payer"].asString(),
        (*json)["sum"].asString(), (*json)["sum_capital"].asString(),
        (*json)["remark"].asString(), (*json)["review"].asString(),
        (*json)["operator"].asString(), (*json)["collection_name"].asString(),
        (*json)["print"].asInt64(), (*json)["project"].toStyledString(),
        (*json)["invalid"].asBool(), (*json)["update_user"].asString(),
        timestamp, (*json)["create_date"].asString(),
        (*json)["delete"].asBool(), (*json)["id"].asString());
  }
}

void Invoice::info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM clean_invoice WHERE \"delete\" = FALSE";

  if ((*json)["id"].asString() != "")
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json)["no"].asString() != "")
    stmt = fmt::format("{} AND no LIKE '%{}%'", stmt, (*json)["no"].asString());

  if ((*json)["imposing_no"].asInt64() != 0)
    stmt = fmt::format("{} AND imposing_no = {}", stmt,
                       (*json)["imposing_no"].asInt64());

  if ((*json)["imposing_name"].asString() != "")
    stmt = fmt::format("{} AND imposing_name LIKE '%{}%'", stmt,
                       (*json)["imposing_name"].asString());

  if ((*json)["collection_name"].asString() != "")
    stmt = fmt::format("{} AND collection_name LIKE '%{}%'", stmt,
                       (*json)["collection_name"].asString());

  if ((*json)["fingerprint"].asString() != "")
    stmt = fmt::format("{} AND fingerprint LIKE '%{}%'", stmt,
                       (*json)["fingerprint"].asString());

  if ((*json)["create_date"].asString() != "")
    stmt = fmt::format("{} AND create_date LIKE '%{}%'", stmt,
                       (*json)["create_date"].asString());

  if ((*json)["zone"].asString() != "")
    stmt = fmt::format("{} AND zone LIKE '%{}%'", stmt,
                       (*json)["zone"].asString());

  if ((*json)["sum"].asString() != "")
    stmt =
        fmt::format("{} AND sum LIKE '%{}%'", stmt, (*json)["sum"].asString());

  if ((*json)["sum_capital"].asString() != "")
    stmt = fmt::format("{} AND sum_capital LIKE '%{}%'", stmt,
                       (*json)["sum_capital"].asString());

  if ((*json)["remark"].asString() != "")
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  if ((*json)["review"].asString() != "")
    stmt = fmt::format("{} AND review LIKE '%{}%'", stmt,
                       (*json)["review"].asString());

  if ((*json)["operator"].asString() != "")
    stmt = fmt::format("{} AND operator LIKE '%{}%'", stmt,
                       (*json)["operator"].asString());

  if ((*json)["create_user"].asString() != "")
    stmt = fmt::format("{} AND create_user LIKE '%{}%'", stmt,
                       (*json)["create_user"].asString());

  if ((*json)["sortBy"].asString() != "") {
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

        if (r.size() == 0) {
          ret["message"] = "没有可用数据";
          ret["status"] = 1;
          return callback(HttpResponse::newHttpJsonResponse(ret));
        }

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<std::string>();
          info["no"] = row["no"].as<std::string>();
          info["imposing_no"] = row["imposing_no"].as<int>();
          info["imposing_name"] = row["imposing_name"].as<std::string>();
          info["fingerprint"] = row["fingerprint"].as<std::string>();
          info["zone"] = row["zone"].as<std::string>();
          info["payer"] = row["payer"].as<std::string>();
          info["sum"] = row["sum"].as<std::string>();
          info["sum_capital"] = row["sum_capital"].as<std::string>();
          info["remark"] = row["remark"].as<std::string>();
          info["review"] = row["review"].as<std::string>();
          info["operator"] = row["operator"].as<std::string>();
          info["collection_name"] = row["collection_name"].as<std::string>();
          info["print"] = row["print"].as<int>();
          info["project"] = row["project"].as<Json::Value>();
          info["invalid"] = row["invalid"].as<bool>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["delete"] = row["delete"].as<bool>();

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
