#include "ums.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
std::string Ums::get_access_token() const {
  Json::Reader reader;
  Json::Value config = app().getCustomConfig()["pay"]["ums"];

  auto now = std::chrono::system_clock::now();
  time_t time = std::chrono::system_clock::to_time_t(now);

  auto redisClient = app().getRedisClient("redis-slave");

  redisClient->execCommandAsync(
      [=](const nosql::RedisResult &r) { return r.asString(); },
      [=](const std::exception &e) {
        auto uid = common::utility::uuid();
        std::regex express("-");
        std::vector<std::string> v(
            std::sregex_token_iterator(uid.begin(), uid.end(), express, -1),
            std::sregex_token_iterator());
        std::string nonce;
        for (auto &&s : v)
          nonce = fmt::format("{}{}", nonce, s);

        auto tm = *std::localtime(&time);
        auto timestamp = fmt::format("{:%Y%m%d%H%M%S}", tm);

        Json::Value client_req;
        client_req["appId"] = config["appid"];
        client_req["timestamp"] = timestamp;
        client_req["nonce"] = nonce;
        client_req["signMethod"] = "SHA256";
        client_req["signature"] = common::utility::sha256(
            fmt::format("{}{}{}{}", config["appid"].asString(), timestamp,
                        nonce, config["appkey"].asString()));

        auto ar = cpr::PostAsync(
            cpr::Url{"https://api-mop.chinaums.com/v1/token/access"},
            cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
            cpr::Header{{"Content-type", "application/json"}},
            cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
            cpr::Body{client_req.toStyledString()});
        auto res = ar.get();

        if (res.status_code == 200) {
          Json::Reader reader;
          Json::Value json_token;
          reader.parse(res.text, json_token);

          redisClient->execCommandAsync(
              [=](const nosql::RedisResult &r) { return r.asString(); },
              [=](const std::exception &e) {}, "SET ums:key %s EX 600",
              Json::FastWriter().write(json_token["accessToken"]).c_str());
        }
      },
      "GET ums:key");

  return "";
}

void Ums::query(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Value ret;
  Json::Reader reader;
  Json::Value config = app().getCustomConfig()["pay"]["ums"];

  auto timestamp = common::utility::timeNow();

  Json::Value client_req;
  client_req["billDate"] = (*json)["time"].asString();
  client_req["billNo"] = (*json)["no"].asString();
  client_req["instMid"] = "QRPAYDEFAULT";
  client_req["mid"] = config["mid"].asString();
  client_req["requestTimestamp"] = timestamp;
  client_req["tid"] = config["tid"].asString();

  auto ar = cpr::PostAsync(
      cpr::Url{"https://api-mop.chinaums.com/v1/netpay/bills/query"},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Header{
          {"Content-type", "application/json"},
          {"Authorization",
           fmt::format("OPEN-ACCESS-TOKEN AccessToken=\"{}\", AppId=\"{}\"",
                       get_access_token(), config["appid"].asString())}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      ret = body;

      callback(HttpResponse::newHttpJsonResponse(ret));
    } else {
      ret["message"] = res.text;
      ret["status"] = 0;

      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } else {
    ret["message"] = "调用失败";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Ums::order(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Reader reader;
  Json::Value config = app().getCustomConfig()["pay"]["ums"];

  auto timestamp = common::utility::timeNow();

  Json::Value client_req;
  client_req["billDate"] = (*json)["time"].asString();
  client_req["billDesc"] = (*json)["desc"].asString();
  client_req["billNo"] = (*json)["no"].asString();
  client_req["counterNo"] = (*json)["desc"].asString();
  client_req["instMid"] = "QRPAYDEFAULT";
  client_req["mid"] = config["mid"].asString();
  client_req["requestTimestamp"] = timestamp;
  client_req["tid"] = config["tid"].asString();
  client_req["totalAmount"] = int((*json)["amount"].asFloat() * 100);
  client_req["returnUrl"] = (*json)["return"].asString();
  client_req["notifyUrl"] = (*json)["notify"].asString();

  if ((*json)["zone"].asString() == "ysjc") {
    client_req["divisionFlag"] = true;
    client_req["platformAmount"] = 0;

    Json::Value sub_order;
    sub_order["mid"] = config["ysjc"].asString();
    sub_order["merOrderId"] = fmt::format("{}SUB", (*json)["no"].asString());
    sub_order["totalAmount"] = int((*json)["amount"].asFloat() * 100);

    Json::Value sub_orders;
    sub_orders.append(sub_order);

    client_req["subOrders"] = sub_orders;
  }

  auto ar = cpr::PostAsync(
      cpr::Url{"https://api-mop.chinaums.com/v1/netpay/bills/get-qrcode"},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Header{
          {"Content-type", "application/json"},
          {"Authorization",
           fmt::format("OPEN-ACCESS-TOKEN AccessToken=\"{}\", AppId=\"{}\"",
                       get_access_token(), config["appid"].asString())}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      if (body["errCode"].asString() == "SUCCESS" ||
          std::strcmp(body["errMsg"].asString().c_str(), "账单号重复\0") == 0) {

        auto db = app().getDbClient("postgresql-slave");
        db->execSqlAsync(
            "SELECT * FROM pay WHERE \"order\" = $1",
            [=](const orm::Result &r) {
              Json::Value ret;

              if (r.size() > 0) {
                auto db = app().getDbClient("postgresql-master");
                db->execSqlAsync(
                    "UPDATE pay SET order_pay = $1, status = 'generate', "
                    "\"type\" = 'ums', amount = $2, remark = $3, "
                    "create_service "
                    "= 'pay', create_params = $4, create_date = $5, "
                    "update_date = $6 WHERE \"order\" = $7",
                    [=](const orm::Result &r) {
                      Json::Value ret;

                      if (r.affectedRows() > 0) {
                        ret = body;
                      } else {
                        ret["message"] = "更新失败";
                        ret["status"] = 0;
                      }

                      callback(HttpResponse::newHttpJsonResponse(ret));
                    },
                    [=](const orm::DrogonDbException &e) {
                      common::utility::log(common::utility::LOGLEVEL::info,
                                           false, "{} - {}:{}", e.base().what(),
                                           __FILE__, __LINE__);

                      Json::Value ret;
                      ret["message"] = e.base().what();
                      ret["status"] = 0;

                      callback(HttpResponse::newHttpJsonResponse(ret));
                    },
                    (*json)["no_pay"].toStyledString(),
                    int((*json)["amount"].asFloat() * 100),
                    (*json)["desc"].asString(), client_req.toStyledString(),
                    (*json)["time"].asString(), timestamp,
                    (*json)["no"].asString());
              } else {
                auto db = app().getDbClient("postgresql-master");
                db->execSqlAsync(
                    "INSERT INTO pay VALUES ($1, $2, 'generate', 'ums', $3, "
                    "$4, $5, 'pay', $6, $7, $8)",
                    [=](const orm::Result &r) {
                      Json::Value ret;

                      if (r.affectedRows() > 0) {
                        ret = body;
                      } else {
                        ret["message"] = "新增失败";
                        ret["status"] = 0;
                      }

                      callback(HttpResponse::newHttpJsonResponse(ret));
                    },
                    [=](const orm::DrogonDbException &e) {
                      common::utility::log(common::utility::LOGLEVEL::info,
                                           false, "{} - {}:{}", e.base().what(),
                                           __FILE__, __LINE__);

                      Json::Value ret;
                      ret["message"] = e.base().what();
                      ret["status"] = 0;

                      callback(HttpResponse::newHttpJsonResponse(ret));
                    },
                    common::utility::uuid(), (*json)["no"].asString(),
                    (*json)["no_pay"].toStyledString(),
                    int((*json)["amount"].asFloat() * 100),
                    (*json)["desc"].asString(), client_req.toStyledString(),
                    (*json)["time"].asString(), timestamp);
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
            (*json)["no"].asString());
      } else {
        Json::Value ret;
        ret = body;

        callback(HttpResponse::newHttpJsonResponse(ret));
      }
    } else {
      Json::Value ret;
      ret["message"] = res.text;
      ret["status"] = 0;

      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } else {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} {} {}",
                         res.status_code, res.text, res.error.message);

    Json::Value ret;
    ret["message"] = "调用失败";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Ums::notify(
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
      "UPDATE pay SET status = 'paid', update_date = $1 WHERE "
      "\"order\" = $2",
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.affectedRows() > 0) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "update pay: {}", (*json)["order"].asString());

          ret["message"] = "success";
          ret["status"] = 1;
        } else {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "no update pay: {}",
                               (*json)["order"].asString());

          ret["message"] = "更新支付失败";
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
      timestamp, (*json)["order"].asString());
}

void Ums::close(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Value ret;
  Json::Reader reader;
  Json::Value config = app().getCustomConfig()["pay"]["ums"];

  auto timestamp = common::utility::timeNow();

  Json::Value client_req;
  client_req["qrCodeId"] = (*json)["no"].asString();
  client_req["instMid"] = "QRPAYDEFAULT";
  client_req["mid"] = config["mid"].asString();
  client_req["requestTimestamp"] = timestamp;
  client_req["tid"] = config["tid"].asString();

  auto ar = cpr::PostAsync(
      cpr::Url{"https://api-mop.chinaums.com/v1/netpay/bills/close-qrcode"},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Header{
          {"Content-type", "application/json"},
          {"Authorization",
           fmt::format("OPEN-ACCESS-TOKEN AccessToken=\"{}\", AppId=\"{}\"",
                       get_access_token(), config["appid"].asString())}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      ret = body;

      callback(HttpResponse::newHttpJsonResponse(ret));
    } else {
      ret["message"] = res.text;
      ret["status"] = 0;

      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } else {
    ret["message"] = "调用失败";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Ums::refund(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Value ret;
  Json::Reader reader;
  Json::Value config = app().getCustomConfig()["pay"]["ums"];

  auto timestamp = common::utility::timeNow();

  Json::Value client_req;
  client_req["billDate"] = (*json)["time"].asString();
  client_req["refundDesc"] = (*json)["desc"].asString();
  client_req["billNo"] = (*json)["no"].asString();
  client_req["counterNo"] = (*json)["desc"].asString();
  client_req["instMid"] = "QRPAYDEFAULT";
  client_req["mid"] = config["mid"].asString();
  client_req["requestTimestamp"] = timestamp;
  client_req["tid"] = config["tid"].asString();
  client_req["refundAmount"] = int((*json)["amount"].asFloat() * 100);
  client_req["refundOrderId"] = (*json)["refundno"].asString();

  if ((*json)["zone"].asString() == "ysjc") {
    client_req["platformAmount"] = int((*json)["amount"].asFloat() * 100);

    Json::Value sub_order;
    sub_order["mid"] = config["ysjc"].asString();
    sub_order["merOrderId"] = fmt::format("{}SUB", (*json)["no"].asString());
    sub_order["refundOrderId"] =
        fmt::format("{}SUB", (*json)["refundno"].asString());
    sub_order["totalAmount"] = int((*json)["amount"].asFloat() * 100);

    Json::Value sub_orders;
    sub_orders.append(sub_order);

    client_req["subOrders"] = sub_orders;
  }

  auto ar = cpr::PostAsync(
      cpr::Url{"https://api-mop.chinaums.com/v1/netpay/bills/refund"},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Header{
          {"Content-type", "application/json"},
          {"Authorization",
           fmt::format("OPEN-ACCESS-TOKEN AccessToken=\"{}\", AppId=\"{}\"",
                       get_access_token(), config["appid"].asString())}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      ret = body;

      callback(HttpResponse::newHttpJsonResponse(ret));
    } else {
      ret["message"] = res.text;
      ret["status"] = 0;

      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } else {
    ret["message"] = "调用失败";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Ums::info(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM pay WHERE \"order\" = $1",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info = row["order_pay"].as<Json::Value>();
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
        ret["message"] = "获取支付记录失败";
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["order"].asString());
}
} // namespace api
