#include "statistics.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Statistics::formal_count(
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
      "SELECT COUNT(id) as count FROM sf_formal_bill WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("fzr"))
    stmt =
        fmt::format("{} AND fzr_id LIKE '{}'", stmt, (*json)["fzr"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row[0].as<int64_t>();
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

void Statistics::formal_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM sf_formal_bill WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("fzr"))
    stmt =
        fmt::format("{} AND fzr_id LIKE '{}'", stmt, (*json)["fzr"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  if ((*json).find("sortBy")) {
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

    if ((*json)["descending"].asBool())
      stmt = fmt::format("{} DESC", stmt);
    else
      stmt = fmt::format("{} ASC", stmt);
  }

  stmt = fmt::format("{} LIMIT {} OFFSET {}", stmt, (*json)["maxPage"].asInt(),
                     (*json)["curPage"].asInt());

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["numbering"] = row["numbering"].as<std::string>();
          info["name"] = row["name"].as<std::string>();
          info["address"] = row["address"].as<std::string>();
          info["principal"] = row["principal"].as<std::string>();
          info["telephone"] = row["telephone"].as<std::string>();
          info["bill_year"] = row["bill_year"].as<std::string>();
          info["bill_month"] = row["bill_month"].as<std::string>();
          info["amount"] = row["amount"].as<std::string>();
          info["price"] = row["price"].as<std::string>();
          info["money"] = row["money"].as<int>();
          info["status"] = row["status"].as<std::string>();
          info["fzr_id"] = row["fzr_id"].as<std::string>();

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

void Statistics::formal_total(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT\n"
                     "	SUM( money ) AS total \n"
                     "FROM\n"
                     "	sf_formal_bill \n"
                     "WHERE\n"
                     "	id != '' \n"
                     "	AND ( create_date >= '2022-10-01 00:00:00' AND "
                     "create_date <= '2022-10-31 23:59:59' ) UNION ALL\n"
                     "SELECT\n"
                     "	SUM( money ) AS total \n"
                     "FROM\n"
                     "	sf_formal_bill \n"
                     "WHERE\n"
                     "	id != '' \n"
                     "	AND \"status\" = 'Y' \n"
                     "	AND (\n"
                     "	create_date >= '2022-10-01 00:00:00' \n"
                     "	AND create_date <= '2022-10-31 23:59:59')";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("fzr"))
    stmt =
        fmt::format("{} AND fzr_id LIKE '{}'", stmt, (*json)["fzr"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row[0].as<int64_t>();
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

  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row["total"].as<int64_t>();
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

void Statistics::payment_info_count(
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
      "SELECT COUNT(id) as count FROM sf_payment_info WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("payment_date_start"))
    stmt = fmt::format("{} AND payment_time > '{}'", stmt,
                       (*json)["payment_date_start"].asString());

  if ((*json).find("payment_date_end"))
    stmt = fmt::format("{} AND payment_time < '{}'", stmt,
                       (*json)["payment_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("fzr"))
    stmt = fmt::format("{} AND cashier LIKE '{}'", stmt,
                       (*json)["fzr"].asString());

  if ((*json).find("source"))
    stmt = fmt::format("{} AND source LIKE '{}'", stmt,
                       (*json)["source"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row[0].as<int64_t>();
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

void Statistics::payment_info_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM sf_payment_info WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("payment_date_start"))
    stmt = fmt::format("{} AND payment_time > '{}'", stmt,
                       (*json)["payment_date_start"].asString());

  if ((*json).find("payment_date_end"))
    stmt = fmt::format("{} AND payment_time < '{}'", stmt,
                       (*json)["payment_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("fzr"))
    stmt = fmt::format("{} AND cashier LIKE '{}'", stmt,
                       (*json)["fzr"].asString());

  if ((*json).find("source"))
    stmt = fmt::format("{} AND source LIKE '{}'", stmt,
                       (*json)["source"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["bill_year"] = row["bill_year"].as<std::string>();
          info["receipt_number"] = row["receipt_number"].as<std::string>();
          info["serial_number"] = row["serial_number"].as<std::string>();
          info["payment_time"] = row["payment_time"].as<std::string>();
          info["payment_amount"] = row["payment_amount"].as<int>();
          info["reality_amount"] = row["reality_amount"].as<int>();
          info["payment_type"] = row["payment_type"].as<std::string>();
          info["numbering"] = row["numbering"].as<std::string>();
          info["name"] = row["name"].as<std::string>();
          info["principal"] = row["principal"].as<std::string>();
          info["telephone"] = row["telephone"].as<std::string>();
          info["address"] = row["address"].as<std::string>();
          info["derate_status"] = row["derate_status"].as<std::string>();
          info["derate_cause"] = row["derate_cause"].as<std::string>();
          info["derate_amount"] = row["derate_amount"].as<int>();
          info["cashier"] = row["cashier"].as<std::string>();
          info["receipt_status"] = row["receipt_status"].as<std::string>();
          info["receipt_code"] = row["receipt_code"].as<std::string>();
          info["remarks"] = row["remarks"].as<std::string>();
          info["status"] = row["status"].as<std::string>();
          info["receipt_title"] = row["receipt_title"].as<std::string>();
          info["source"] = row["source"].as<std::string>();
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

void Statistics::payment_info_total(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = (*json)["reality"].asBool() == true
                         ? "SELECT SUM(reality_amount) as total FROM "
                           "sf_payment_info WHERE id != ''"
                         : "SELECT SUM(payment_amount) as total FROM "
                           "sf_payment_info WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("payment_date_start"))
    stmt = fmt::format("{} AND payment_time > '{}'", stmt,
                       (*json)["payment_date_start"].asString());

  if ((*json).find("payment_date_end"))
    stmt = fmt::format("{} AND payment_time < '{}'", stmt,
                       (*json)["payment_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("fzr"))
    stmt = fmt::format("{} AND cashier LIKE '{}'", stmt,
                       (*json)["fzr"].asString());

  if ((*json).find("source"))
    stmt = fmt::format("{} AND source LIKE '{}'", stmt,
                       (*json)["source"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row["total"].as<int64_t>();
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

void Statistics::payment_web_count(
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
      "SELECT COUNT(id) as count FROM sf_payment_web WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("payment_date_start"))
    stmt = fmt::format("{} AND payment_time > '{}'", stmt,
                       (*json)["payment_date_start"].asString());

  if ((*json).find("payment_date_end"))
    stmt = fmt::format("{} AND payment_time < '{}'", stmt,
                       (*json)["payment_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row[0].as<int64_t>();
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

void Statistics::payment_web_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM sf_payment_web WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("payment_date_start"))
    stmt = fmt::format("{} AND payment_time > '{}'", stmt,
                       (*json)["payment_date_start"].asString());

  if ((*json).find("payment_date_end"))
    stmt = fmt::format("{} AND payment_time < '{}'", stmt,
                       (*json)["payment_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["orderform_code"] = row["orderform_code"].as<std::string>();
          info["payment_time"] = row["payment_time"].as<std::string>();
          info["payment_amount"] = row["payment_amount"].as<std::string>();
          info["payment_type"] = row["payment_type"].as<std::string>();
          info["numbering"] = row["numbering"].as<std::string>();
          info["name"] = row["name"].as<std::string>();
          info["principal"] = row["principal"].as<std::string>();
          info["telephone"] = row["telephone"].as<std::string>();
          info["address"] = row["address"].as<std::string>();
          info["receipt_status"] = row["receipt_status"].as<std::string>();
          info["status"] = row["status"].as<std::string>();
          info["openid"] = row["openid"].as<std::string>();
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

void Statistics::payment_web_total(
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
      "SELECT SUM(payment_amount) as total FROM sf_payment_web WHERE id != ''";

  if ((*json).find("create_date_start"))
    stmt = fmt::format("{} AND create_date > '{}'", stmt,
                       (*json)["create_date_start"].asString());

  if ((*json).find("create_date_end"))
    stmt = fmt::format("{} AND create_date < '{}'", stmt,
                       (*json)["create_date_end"].asString());

  if ((*json).find("payment_date_start"))
    stmt = fmt::format("{} AND payment_time > '{}'", stmt,
                       (*json)["payment_date_start"].asString());

  if ((*json).find("payment_date_end"))
    stmt = fmt::format("{} AND payment_time < '{}'", stmt,
                       (*json)["payment_date_end"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering LIKE '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("type"))
    stmt =
        fmt::format("{} AND type LIKE '{}'", stmt, (*json)["type"].asString());

  if (std::strcmp((*json)["status"].asString().c_str(), "已支付\0") == 0) {
    stmt = fmt::format("{} AND status = 'Y'", stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "未支付\0") ==
             0) {
    stmt = fmt::format("{} AND status = 'N'", stmt);
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row["total"].as<int64_t>();
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
} // namespace api
