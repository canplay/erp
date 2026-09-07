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

  std::string stmt = "SELECT COUNT(id) FROM sf_payment_info";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND `id` = '{}'", stmt, (*json)["id"].asString());

  if ((*json).find("serial_number"))
    stmt = fmt::format("{} AND serial_number = '{}'", stmt,
                       (*json)["serial_number"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering = '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("cashier"))
    stmt = fmt::format("{} AND cashier = '{}'", stmt,
                       (*json)["cashier"].asString());

  if ((*json).find("payment_time"))
    stmt = fmt::format("{} AND payment_time = '{}'", stmt,
                       (*json)["payment_time"].asString());

  auto db = app().getDbClient("jc");
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

  std::string stmt = "SELECT * FROM sf_payment_info";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND `id` = '{}'", stmt, (*json)["id"].asString());

  if ((*json).find("serial_number"))
    stmt = fmt::format("{} AND serial_number = '{}'", stmt,
                       (*json)["serial_number"].asString());

  if ((*json).find("numbering"))
    stmt = fmt::format("{} AND numbering = '{}'", stmt,
                       (*json)["numbering"].asString());

  if ((*json).find("cashier"))
    stmt = fmt::format("{} AND cashier = '{}'", stmt,
                       (*json)["cashier"].asString());

  if ((*json).find("payment_time"))
    stmt = fmt::format("{} AND payment_time = '{}'", stmt,
                       (*json)["payment_time"].asString());

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
          info["no"] = row["no"].as<std::string>();
          info["imposing_no"] = row["imposing_no"].as<std::string>();
          info["imposing_name"] = row["imposing_name"].as<std::string>();
          info["fingerprint"] = row["fingerprint"].as<std::string>();
          info["zone"] = row["zone"].as<std::string>();
          info["payer"] = row["payer"].as<std::string>();
          info["project_code"] = row["project_code"].as<std::string>();
          info["project_name"] = row["project_name"].as<std::string>();
          info["unit"] = row["unit"].as<std::string>();
          info["num"] = row["num"].as<std::string>();
          info["criteria"] = row["criteria"].as<std::string>();
          info["sum"] = row["sum"].as<std::string>();
          info["sum_capital"] = row["sum_capital"].as<std::string>();
          info["remark"] = row["remark"].as<std::string>();
          info["review"] = row["review"].as<std::string>();
          info["operator"] = row["operator"].as<std::string>();
          info["collection_name"] = row["collection_name"].as<std::string>();
          info["print"] = row["print"].as<int8_t>();
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
      },
      false);
}

void Order::legacy_count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      "SELECT COUNT(id) FROM sf_formal_bill WHERE numbering = ? AND `status` = "
      "'N'",
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
      },
      (*json)["numbering"].asString());
}

void Order::legacy_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      "SELECT `id` FROM basis_charge_objcet WHERE numbering = ?",
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          db->execSqlAsync(
              "SELECT * FROM sf_formal_bill WHERE numbering = ? AND `status` = "
              "'N'",
              [=](const orm::Result &r) {
                Json::Value ret;
                Json::Value infos;

                for (auto &row : r) {
                  Json::Value info;

                  info["id"] = row["id"].as<std::string>();
                  info["name"] = row["name"].as<std::string>();
                  info["principal"] = row["principal"].as<std::string>();
                  info["address"] = row["address"].as<std::string>();
                  info["telephone"] = row["telephone"].as<std::string>();
                  info["bill_year"] = row["bill_year"].as<std::string>();
                  info["bill_month"] = row["bill_month"].as<std::string>();
                  info["amount"] = row["amount"].as<std::string>();
                  info["price"] = row["price"].as<std::string>();
                  info["money"] = row["money"].as<int64_t>();

                  infos.append(info);
                }

                ret["data"] = infos;
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
              (*json)["numbering"].asString());
        } else {
          ret["message"] = "无有效户号";
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["numbering"].asString());
}

void Order::legacy_pay(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  if (req->getParameter("billStatus") != "PAID") {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto timestamp = common::utility::timeNow();

  Json::Value client_req;
  client_req["time"] = req->getParameter("billDate");
  client_req["no"] = req->getParameter("billNo");
  client_req["mid"] = req->getParameter("mid");
  client_req["requestTimestamp"] = timestamp;
  client_req["tid"] = req->getParameter("tid");

  auto ar = cpr::PostAsync(
      cpr::Url{
          "https://erp.example.com/api/route/post?auth=public&service=pay&path=/"
          "api/ums/query",
      },
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib", "br"}},
      cpr::Header{{"Content-type", "application/json"}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      if (body["billStatus"].asString() == "PAID") {
        Json::Value client_req;
        client_req["order"] = req->getParameter("billNo");

        auto ar = cpr::PostAsync(
            cpr::Url{"https://erp.example.com/api/route/"
                     "post?auth=key&service=pay&path=/api/ums/info"},
            cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
            cpr::Header{
                {"Content-type", "application/json"},
                {"Authorization",
                 app().getCustomConfig()["auth"]["m2m"]["key"].asString()}},
            cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
            cpr::Body{client_req.toStyledString()});
        auto res = ar.get();

        if (res.status_code == 200) {
          Json::Reader reader;
          Json::Value body;
          if (reader.parse(res.text, body)) {
            Json::Value order = body["msg"];
            auto ar = cpr::PostAsync(
                cpr::Url{"https://erp.example.com/api/route/"
                         "post?auth=key&service=pay&path=/"
                         "api/ums/notify"},
                cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                cpr::Header{
                    {"Content-type", "application/json"},
                    {"Authorization",
                     app().getCustomConfig()["auth"]["m2m"]["key"].asString()}},
                cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                cpr::Body{client_req.toStyledString()});
            res = ar.get();

            auto uid = common::utility::uuid();
            std::regex express("-");
            std::vector<std::string> v(
                std::sregex_token_iterator(uid.begin(), uid.end(), express, -1),
                std::sregex_token_iterator());
            std::string payment_info_id;
            for (auto &&s : v)
              payment_info_id = fmt::format("{}{}", payment_info_id, s);

            for (auto id : order["id"]) {
              auto uid1 = common::utility::uuid();
              std::vector<std::string> v1(
                  std::sregex_token_iterator(uid1.begin(), uid1.end(), express,
                                             -1),
                  std::sregex_token_iterator());
              std::string payment_detail_id;
              for (auto &&s : v1)
                payment_detail_id = fmt::format("{}{}", payment_detail_id, s);

              update_formal_bill(id.asString(), [=, this](const bool result) {
                if (result) {
                  insert_payment_detail(payment_detail_id, payment_info_id,
                                        id.asString());
                }
              });
            }

            insert_payment_web(
                order["id"][0].asString(), payment_info_id,
                std::stoi(req->getParameter("receiptAmount")),
                req->getParameter("billNo"), [=](const bool result) {
                  if (result) {
                    Json::Value ret;
                    ret["message"] = "SUCCESS";
                    ret["status"] = 1;
                    callback(HttpResponse::newHttpJsonResponse(ret));
                  } else {
                    Json::Value ret;
                    ret["message"] = "FAILED";
                    ret["status"] = 0;
                    callback(HttpResponse::newHttpJsonResponse(ret));
                  }
                });
          } else {
            Json::Value ret;
            ret["message"] = fmt::format("error: {}", res.text);
            ret["status"] = 0;
            callback(HttpResponse::newHttpJsonResponse(ret));
          }
        } else {
          Json::Value ret;
          ret["message"] = fmt::format("error: {}", res.text);
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      } else {
        Json::Value ret;
        ret["message"] = "success";
        ret["status"] = 1;
        callback(HttpResponse::newHttpJsonResponse(ret));
      }
    } else {
      Json::Value ret;
      ret["message"] = fmt::format("error: {}", res.text);
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } else {
    Json::Value ret;
    ret["message"] = fmt::format("error: {}", res.text);
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Order::legacy_pay_pda(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  if (req->getParameter("billStatus") != "PAID") {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto timestamp = common::utility::timeNow();

  Json::Value client_req;
  client_req["time"] = req->getParameter("billDate");
  client_req["no"] = req->getParameter("billNo");
  client_req["mid"] = req->getParameter("mid");
  client_req["requestTimestamp"] = timestamp;
  client_req["tid"] = req->getParameter("tid");

  auto ar = cpr::PostAsync(
      cpr::Url{"https://erp.example.com/api/route/"
               "post?auth=public&service=pay&path=/api/ums/query"},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Header{{"Content-type", "application/json"}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      if (body["billStatus"].asString() == "PAID") {
        Json::Value client_req;
        client_req["order"] = req->getParameter("billNo");

        auto ar = cpr::PostAsync(
            cpr::Url{"https://erp.example.com/api/route/"
                     "post?auth=key&service=pay&path=/api/ums/notify"},
            cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
            cpr::Header{
                {"Content-type", "application/json"},
                {"Authorization",
                 app().getCustomConfig()["auth"]["m2m"]["key"].asString()}},
            cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
            cpr::Body{client_req.toStyledString()});

        Json::Value ret;
        ret["message"] = "SUCCESS";
        ret["status"] = 1;

        return callback(HttpResponse::newHttpJsonResponse(ret));
      }
    }
  }

  Json::Value ret;
  ret["message"] = "FAILED";
  ret["status"] = 0;
  callback(HttpResponse::newHttpJsonResponse(ret));
}

void Order::legacy_payment(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      "SELECT * FROM sf_payment_detail WHERE formal_bill = ?",
      [=](const orm::Result &r) {
        if (r.size() > 0) {
          auto row = r[0];

          auto timestamp = common::utility::timeNow();

          std::string src = "终端二维码 - " + (*json)["source"].asString();

          db->execSqlAsync(
              "UPDATE sf_payment_info SET payment_type = 'ZZ', update_name = "
              "'管理员', update_by = 'admin', update_date = ?, derate_status "
              "= ?, derate_cause = ?, derate_amount = ?, source = ? WHERE `id` "
              "= ?",
              [=](const orm::Result &r) {
                if (r.affectedRows() > 0) {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "update sf_payment_info: {}",
                                       row["payment_info"].as<std::string>());

                  Json::Value ret;
                  ret["message"] = "success";
                  ret["status"] = 1;
                  callback(HttpResponse::newHttpJsonResponse(ret));
                } else {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "no update sf_payment_info: {}",
                                       row["payment_info"].as<std::string>());

                  Json::Value ret;
                  ret["message"] = "更新失败";
                  ret["status"] = 0;
                  callback(HttpResponse::newHttpJsonResponse(ret));
                }
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
              timestamp, (*json)["derate_status"].asString(),
              (*json)["derate_cause"].asString(),
              (*json)["derate_amount"].asInt(), src,
              row["payment_info"].as<std::string>());
        } else {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "select sf_payment_detail error: {}",
                               (*json)["id"].asString());

          Json::Value ret;
          ret["message"] = "更新失败";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["id"].asString());
}

void Order::update_formal_bill(
    const std::string &id, std::function<void(const bool)> &&callback) const {
  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      "UPDATE sf_formal_bill SET `status` = 'Y' WHERE `id` = ?",
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.affectedRows() > 0) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "update sf_formal_bill: {}", id);
          callback(true);
        } else {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "no update sf_formal_bill: {}", id);
          callback(false);
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);
        callback(false);
      },
      id);
}

void Order::insert_payment_detail(const std::string &id,
                                  const std::string &payment_info,
                                  const std::string &formal_bill) const {
  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      "INSERT INTO sf_payment_detail VALUES (?, ?, ?)",
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.affectedRows() > 0) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "insert sf_payment_detail: {}", formal_bill);
        } else {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "no insert sf_payment_detail: {}", formal_bill);
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);
      },
      id, payment_info, formal_bill);
}

void Order::insert_payment_web(
    const std::string &formal_bill, const std::string &payment_info, int amount,
    const std::string &ums_order,
    std::function<void(const bool)> &&callback) const {
  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      "SELECT `id` FROM sf_payment_web WHERE openid = ?",
      [=](const orm::Result &r) {
        if (r.size() > 0) {
          auto row = r[0];

          auto now = std::chrono::system_clock::now();
          time_t time = std::chrono::system_clock::to_time_t(now);
          auto tm = *std::localtime(&time);
          auto timestamp =
              fmt::format("{:%Y-%m-%d %H:%M:%S}", tm);
          auto timestamp1 = fmt::format("{:%Y-%m-%d}", tm);
          auto timestamp2 = fmt::format("{:%Y%m%d}", tm);

          std::string serial_number = timestamp2 + payment_info.substr(8, 4);
          std::string receipt_number = payment_info.substr(0, 7);

          db->execSqlAsync(
              "UPDATE sf_payment_web SET update_name = '管理员', update_by = "
              "'admin', update_date = ?, payment_time = ?, payment_amount = "
              "?, `status` = 'Y', openid = ? WHERE `id` = ?",
              [=](const orm::Result &r) {
                Json::Value ret;

                if (r.affectedRows() > 0) {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "update sf_payment_web: {}",
                                       row["id"].as<std::string>());
                  callback(true);
                } else {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "no update sf_payment_web: {}",
                                       row["id"].as<std::string>());
                  callback(false);
                }
              },
              [=](const orm::DrogonDbException &e) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "{} - {}:{}", e.base().what(), __FILE__,
                                     __LINE__);
                callback(false);
              },
              timestamp, timestamp, amount / 100, ums_order,
              row["id"].as<std::string>());
        } else {
          db->execSqlAsync(
              "SELECT * FROM sf_formal_bill WHERE `id` = ?",
              [=](const orm::Result &r) {
                Json::Value ret;

                if (r.size() > 0) {
                  auto row = r[0];

                  auto now = std::chrono::system_clock::now();
                  time_t time = std::chrono::system_clock::to_time_t(now);
                  auto tm = *std::localtime(&time);
                  auto timestamp =
                      fmt::format("{:%Y-%m-%d %H:%M:%S}", tm);
                  auto timestamp1 = fmt::format("{:%Y-%m-%d}", tm);
                  auto timestamp2 = fmt::format("{:%Y%m%d}", tm);

                  std::string serial_number =
                      timestamp2 + payment_info.substr(8, 4);
                  std::string receipt_number = payment_info.substr(0, 7);

                  db->execSqlAsync(
                      "INSERT INTO sf_payment_web VALUES (?, '管理员', "
                      "'admin', ?, '管理员', 'admin', ?, ?, ?, ?, ?, ?, "
                      "'WX', ?, ?, ?, ?, ?, '', 'Y', ?, ?)",
                      [=](const orm::Result &r) {
                        Json::Value ret;

                        if (r.affectedRows() > 0) {
                          common::utility::log(common::utility::LOGLEVEL::info,
                                               false,
                                               "insert sf_payment_web: {}",
                                               row["id"].as<std::string>());
                          callback(true);
                        } else {
                          common::utility::log(common::utility::LOGLEVEL::info,
                                               false,
                                               "no insert sf_payment_web: {}",
                                               row["id"].as<std::string>());
                          callback(false);
                        }
                      },
                      [=](const orm::DrogonDbException &e) {
                        common::utility::log(
                            common::utility::LOGLEVEL::info, false,
                            "{} - {}:{}", e.base().what(), __FILE__, __LINE__);
                        callback(false);
                      },
                      payment_info, timestamp, timestamp,
                      row["sys_org_code"].as<std::string>(),
                      row["sys_company_code"].as<std::string>(), serial_number,
                      timestamp, amount / 100,
                      row["numbering"].as<std::string>(),
                      row["name"].as<std::string>(),
                      row["principal"].as<std::string>(),
                      row["telephone"].as<std::string>(),
                      row["address"].as<std::string>(),
                      row["charge_objcet_id"].as<std::string>(), ums_order);
                } else {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "no select sf_formal_bill: {}",
                                       formal_bill);
                  callback(false);
                }
              },
              [=](const orm::DrogonDbException &e) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "{} - {}:{}", e.base().what(), __FILE__,
                                     __LINE__);
                callback(false);
              },
              formal_bill);
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);
        callback(false);
      },
      ums_order);
}
} // namespace api
