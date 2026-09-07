#include "exec.h"
#include "include/utility.h"
#include "pch.h"
#include <ctime>

namespace api {
std::string Exec::get_access_token(bool refrash) const {
  Json::Reader reader;
  Json::Value config = app().getCustomConfig()["pay"]["ums"];

  auto now = std::chrono::system_clock::now();
  time_t time = std::chrono::system_clock::to_time_t(now);

  auto redisClient = app().getRedisClient("redis-slave");

  if (refrash) {
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
        fmt::format("{}{}{}{}", config["appid"].asString(), timestamp, nonce,
                    config["appkey"].asString()));

    auto ar =
        cpr::PostAsync(cpr::Url{"https://api-mop.chinaums.com/v1/token/access"},
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
  } else {
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

          auto tm = std::localtime(&time);
          auto timestamp = fmt::format("{:%Y%m%d%H%M%S}", *tm);

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
  }

  return "";
}

void Exec::status(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  if ((*json)["type"].asString() == "check_pay") {
    check_pay([=](const Json::Value &r) {
      callback(HttpResponse::newHttpJsonResponse(r));
    });
  } else {
    Json::Value ret;
    ret["message"] = "error";
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Exec::check_pay(std::function<void(const Json::Value &)> &&callback,
                     int nStart) const {
  Json::Value ret;

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT \"id\", \"order\", amount, create_date FROM pay ORDER BY "
      "create_date DESC",
      [=, this](const orm::Result &r) {
        std::string msgs;

        for (int i = nStart; i < r.size(); i++) {
          auto row = r[i];

          Json::Reader reader;
          Json::Value config = app().getCustomConfig()["pay"]["ums"];

          auto timestamp = common::utility::timeNow();

          Json::Value client_req;
          client_req["billDate"] = row[3].as<std::string>();
          client_req["billNo"] = row[1].as<std::string>();
          client_req["instMid"] = "QRPAYDEFAULT";
          client_req["mid"] = config["mid"].asString();
          client_req["requestTimestamp"] = timestamp;
          client_req["tid"] = config["tid"].asString();
          client_req["amount"] = row[2].as<int>();

          auto ar = cpr::PostAsync(
              cpr::Url{"https://api-mop.chinaums.com/v1/netpay/bills/query"},
              cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
              cpr::Header{
                  {"Content-type", "application/json"},
                  {"Authorization",
                   fmt::format(
                       "OPEN-ACCESS-TOKEN AccessToken=\"{}\", AppId =\"{}\"",
                       get_access_token(), config["appid"].asString())}},
              cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
              cpr::Body{client_req.toStyledString()});
          auto res = ar.get();

          if (res.status_code == 200) {
            Json::Reader reader;
            Json::Value body;
            if (reader.parse(res.text, body)) {
              msgs.append(fmt::format("{} - {}\n",
                                      client_req["billNo"].asString(),
                                      body["billStatus"].asString()));

              if (body["billStatus"].asString() == "PAID") {
                auto ar1 = cpr::PostAsync(
                    cpr::Url{"http://100.100.100.100:51530/api/order/"
                             "legacy/pay"},
                    cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                    cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                    cpr::Parameters{
                        {"billStatus", "PAID"},
                        {"billDate", client_req["billDate"].asString()},
                        {"billNo", client_req["billNo"].asString()},
                        {"mid", client_req["mid"].asString()},
                        {"tid", client_req["tid"].asString()},
                        {"receiptAmount",
                         std::to_string(client_req["amount"].asInt())},
                    });
                auto res1 = ar1.get();

                common::utility::log(
                    common::utility::LOGLEVEL::info, false,
                    "http://100.100.100.100:51530/api/order/legacy/"
                    "pay?billStatus=PAID&billDate={}&billNo={}&"
                    "mid={}&tid={}&receiptAmount={}",
                    client_req["billDate"].asString(),
                    client_req["billNo"].asString(),
                    client_req["mid"].asString(), client_req["tid"].asString(),
                    client_req["amount"].asInt());

                if (res1.status_code == 200) {
                  Json::Reader reader1;
                  Json::Value body1;
                  if (reader1.parse(res1.text, body1)) {
                    common::utility::log(common::utility::LOGLEVEL::info, false,
                                         "{} - {}",
                                         client_req["billNo"].asString(),
                                         body1["msg"].asString());

                    msgs.append(fmt::format("{} - {}\n",
                                            client_req["billNo"].asString(),
                                            body1["msg"].asString()));
                    continue;
                  } else {

                    common::utility::log(common::utility::LOGLEVEL::info, false,
                                         "parse error: {} - {}",
                                         client_req["billNo"].asString(),
                                         common::utility::utf8ToGBK(res1.text));

                    msgs.append(
                        fmt::format("parse error: {} - {}\n",
                                    client_req["billNo"].asString(),
                                    common::utility::utf8ToGBK(res1.text)));
                  }
                } else {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "cpr2 error: {} - {}: {}",
                                       client_req["billNo"].asString(),
                                       res1.status_code,
                                       common::utility::utf8ToGBK(res1.text));

                  msgs.append(fmt::format(
                      "cpr2 error: {} - {}: {}\n",
                      client_req["billNo"].asString(), res1.status_code,
                      common::utility::utf8ToGBK(res1.text)));
                }
              } else {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "UNPAID: {}",
                                     client_req["billNo"].asString());

                msgs.append(fmt::format("UNPAID: {}\n",
                                        client_req["billNo"].asString()));
              }
            } else {
              common::utility::log(common::utility::LOGLEVEL::info, false,
                                   "parse error: {} - {}\n",
                                   client_req["billNo"].asString(),
                                   common::utility::utf8ToGBK(res.text));

              msgs.append(fmt::format("parse error: {} - {}\n",
                                      client_req["billNo"].asString(),
                                      common::utility::utf8ToGBK(res.text)));
            }
          } else {
            common::utility::log(
                common::utility::LOGLEVEL::info, false,
                "cpr1 error: {} - {}: {}", client_req["billNo"].asString(),
                res.status_code, common::utility::utf8ToGBK(res.text));

            Json::Reader reader2;
            Json::Value body2;
            if (reader2.parse(res.text, body2)) {
              if (body2["errInfo"].asString() == "认证失败") {
                get_access_token(true);
                check_pay(
                    [=](const Json::Value &) {
                      Json::Value ret;
                      ret["message"] = msgs;
                      ret["status"] = "1";
                      callback(ret);
                    },
                    i - 1);

                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "auth: {}",
                                     client_req["billNo"].asString());
              }
            }

            msgs.append(fmt::format(
                "cpr1 error: {} - {}: {}\n", client_req["billNo"].asString(),
                res.status_code, common::utility::utf8ToGBK(res.text)));
          }
        }

        Json::Value ret;
        ret["message"] = msgs;
        ret["status"] = "1";
        callback(ret);
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);
        Json::Value ret;
        ret["message"] = "error";
        ret["status"] = "0";
        callback(ret);
      });
}
} // namespace api
