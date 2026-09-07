#include "hik.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Hik::exec(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  async_run([=, this]() -> Task<> {
    try {
      std::string token = co_await getToken();

      if (token.empty()) {
        Json::Value ret;
        ret["message"] = "令牌错误";
        ret["status"] = 0;
        co_return callback(HttpResponse::newHttpJsonResponse(ret));
      }

      std::string path;
      Json::Value client_req;

      if ((*json)["method"].asString() == "driver") {
        path = "/artemis/api/v1/driver/driverInfo";
        client_req["phone"] = (*json)["phone"].asInt64();
      } else if ((*json)["method"].asString() == "plates") {
        path = "/artemis/api/v1/driver/plates";
        client_req["driverId"] = (*json)["driverId"].asString();
      } else if ((*json)["method"].asString() == "register") {
        path = "/artemis/api/v1/driver/register";
        client_req["phone"] = (*json)["phone"].asInt64();
      } else if ((*json)["method"].asString() == "addPlate") {
        path = "/artemis/api/v1/driver/addPlate";
        client_req["driverId"] = (*json)["driverId"].asString();
        client_req["plateNo"] = (*json)["plateNo"].asString();
        client_req["plateColor"] = (*json)["plateColor"].asString();
      } else if ((*json)["method"].asString() == "delPlate") {
        path = "/artemis/api/v1/driver/delPlate";
        client_req["driverId"] = (*json)["driverId"].asString();
        client_req["plateNo"] = (*json)["plateNo"].asString();
        client_req["plateColor"] = (*json)["plateColor"].asString();
      } else if ((*json)["method"].asString() == "parkOrders") {
        path = "/artemis/api/v1/charge/parkOrders";
        client_req["plateNo"] = (*json)["plateNo"].asString();
        client_req["phone"] = (*json)["phone"].asInt64();
        client_req["pageNo"] = (*json)["pageNo"].asString();
        client_req["pageSize"] = (*json)["pageSize"].asString();
        client_req["requestType"] = (*json)["requestType"].asString();
      } else if ((*json)["method"].asString() == "parkParking") {
        path = "/artemis/api/v1/charge/order/parking";
        client_req["plateNo"] = (*json)["plateNo"].asString();
      } else if ((*json)["method"].asString() == "parkArrears") {
        path = "/artemis/api/v1/charge/getArrearsRecords";
        client_req["plateNo"] = (*json)["plateNo"].asString();
      } else if ((*json)["method"].asString() == "parkOrdersDetail") {
        path = "/artemis/api/v1/charge/parkOrders/detail";
        client_req["phone"] = (*json)["phone"].asInt64();
        client_req["uniqueId"] = (*json)["uniqueId"].asString();
        client_req["parkCode"] = (*json)["parkCode"].asString();
        client_req["isUseCoupon"] = (*json)["isUseCoupon"].asString();
      } else if ((*json)["method"].asString() == "report") {
        path = "/artemis/api/v1/charge/parkOrders/appeal";
        client_req["phone"] = (*json)["phone"].asInt64();
        client_req["uniqueId"] = (*json)["uniqueId"].asString();
        client_req["parkCode"] = (*json)["parkCode"].asString();
        client_req["appealType"] = (*json)["appealType"].asString();
        client_req["appealRemark"] = (*json)["appealRemark"].asString();
        client_req["appealInTime"] = (*json)["appealInTime"].asString();
        client_req["appealOutTime"] = (*json)["appealOutTime"].asString();
        client_req["appealSource"] = (*json)["appealSource"].asString();
      } else if ((*json)["method"].asString() == "payBatch") {
        path = "/artemis/api/v1/charge/payment/batch";
        client_req["arrearsIds"] = (*json)["arrearsIds"].asString();
        client_req["payType"] = 5;
        client_req["payChannel"] = 4;
        client_req["phone"] = (*json)["phone"].asInt64();
      } else if ((*json)["method"].asString() == "parkinfos") {
        path = "/artemis/api/v1/dealer/parkinfos";
        client_req["pageNo"] = (*json)["pageNo"].asInt();
        client_req["pageSize"] = (*json)["pageSize"].asInt();
        client_req["parkName"] = (*json)["parkName"].asString();
      } else if ((*json)["method"].asString() == "parkinfo") {
        path = fmt::format("/artemis/api/v1/dealer/parkinfo/{}",
                           (*json)["parkCode"].asString());
      } else {
        Json::Value ret;
        ret["message"] = "方法无效";
        ret["status"] = 0;
        co_return callback(HttpResponse::newHttpJsonResponse(ret));
      }

      auto ar = cpr::PostAsync(
          cpr::Url{fmt::format(
              "{}{}", app().getCustomConfig()["third"]["hik"]["url"].asString(),
              path)},
          cpr::Header{{"access_token", token},
                      {"Content-Type", "application/json"}},
          cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
          cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
          cpr::Body{client_req.toStyledString()});
      auto res = ar.get();

      if (res.status_code == 200) {
        Json::Reader reader;
        Json::Value body;
        if (reader.parse(res.text, body)) {
          Json::Value json;
          json["data"] = body;
          json["message"] = "success";
          json["status"] = 1;
          co_return callback(HttpResponse::newHttpJsonResponse(json));
        }
      }

      Json::Value json;
      json["message"] = "failure";
      json["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(json));
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

void Hik::coupon(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback,
                 const std::string &method, const std::string &type) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  async_run([=, this]() -> Task<> {
    try {
      if (method == "send") {
        if (type == "car") {
          std::string token = co_await getToken();

          std::string path = "/artemis/api/v1/dealer/parkinfos";
          Json::Value client_req;
          client_req["pageNo"] = 1;
          client_req["pageSize"] = 50;

          auto ar = cpr::PostAsync(
              cpr::Url{fmt::format(
                  "{}{}",
                  app().getCustomConfig()["third"]["hik"]["url"].asString(),
                  path)},
              cpr::Header{{"access_token", token},
                          {"Content-Type", "application/json"}},
              cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
              cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
              cpr::Body{client_req.toStyledString()});
          auto res = ar.get();

          if (res.status_code == 200) {
            Json::Reader reader;
            Json::Value body;
            if (reader.parse(res.text, body)) {
              std::string path = "/artemis/api/v1/sendCoupon";
              Json::Value client_req;

              std::string parkCodes;
              for (auto it : body["data"]["results"]) {
                parkCodes += it["parkCode"].asString();
                if (it != body["data"]["results"].back())
                  parkCodes += ",";
              }

              client_req["parkCodes"] = parkCodes;
              client_req["generateObj"] = 2;
              client_req["telephone"] = (*json)["phone"].asString();
              client_req["couponType"] = 1;
              client_req["deductContent"] = (*json)["amount"].asInt();
              client_req["startTime"] = (*json)["start"].asInt64();
              client_req["endTime"] = (*json)["end"].asString();
              client_req["couponSource"] = "1001";

              auto ar = cpr::PostAsync(
                  cpr::Url{fmt::format(
                      "{}{}",
                      app().getCustomConfig()["third"]["hik"]["url"].asString(),
                      path)},
                  cpr::Header{{"access_token", token},
                              {"Content-Type", "application/json"}},
                  cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                  cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                  cpr::Body{client_req.toStyledString()});
              auto res = ar.get();

              if (res.status_code == 200) {
                Json::Reader reader;
                Json::Value body;
                if (reader.parse(res.text, body)) {
                  Json::Value json;
                  json["data"] = body;
                  json["message"] = "success";
                  json["status"] = 1;
                  co_return callback(HttpResponse::newHttpJsonResponse(json));
                }
              }
            }
          }

          Json::Value ret;
          ret["message"] = "发放优惠券失败";
          ret["status"] = 0;
          co_return callback(HttpResponse::newHttpJsonResponse(ret));
        } else if (type == "clean") {
          Json::Value ret;
          ret["message"] = "success";
          ret["status"] = 0;
          co_return callback(HttpResponse::newHttpJsonResponse(ret));
        } else if (type == "water") {
          Json::Value ret;
          ret["message"] = "success";
          ret["status"] = 0;
          co_return callback(HttpResponse::newHttpJsonResponse(ret));
        }

        Json::Value ret;
        ret["message"] = "failure";
        ret["status"] = 0;
        callback(HttpResponse::newHttpJsonResponse(ret));
      } else {
        Json::Value ret;
        ret["message"] = "failure";
        ret["status"] = 0;
        callback(HttpResponse::newHttpJsonResponse(ret));
      }
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

Task<std::string> Hik::getToken() const {
  try {
    auto redisClient = app().getRedisClient("redis-slave");
    auto redis_result = co_await redisClient->execCommandCoro("GET hik_token");

    if (redis_result.isNil() || redis_result.asString().empty()) {
      auto ar = cpr::PostAsync(
          cpr::Url{fmt::format(
              "{}/artemis/oauth/token",
              app().getCustomConfig()["third"]["hik"]["url"].asString())},
          cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
          cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
          cpr::Payload{
              {"client_id",
               app().getCustomConfig()["third"]["hik"]["id"].asString()},
              {"client_secret",
               app().getCustomConfig()["third"]["hik"]["key"].asString()}});
      auto res = ar.get();

      if (res.status_code == 200) {
        Json::Reader reader;
        Json::Value body;
        if (reader.parse(res.text, body)) {
          if (body.isMember("access_token")) {
            auto redisClient = app().getRedisClient("redis-master");
            co_await redisClient->execCommandCoro(
                fmt::format("SET hik_token {} EX {}",
                            body["access_token"].asString().c_str(),
                            body["expires_in"].asInt()));

            co_return body["access_token"].asString();
          }
        }

        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "get token error - {}:{}", __FILE__, __LINE__);
        co_return "";
      } else {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "get token error - {}:{}", __FILE__, __LINE__);
        co_return "";
      }
    } else {
      co_return redis_result.asString();
    }
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);

    co_return "";
  };
}
} // namespace api
