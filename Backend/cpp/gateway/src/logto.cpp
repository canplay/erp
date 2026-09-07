#include "logto.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Logto::access(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Value client_req;
  client_req["grant_type"] = "client_credentials";
  client_req["resource"] = "http://localhost:51530";
  client_req["scope"] = "all";
  client_req["client_id"] =
      app().getCustomConfig()["auth"]["logto"]["id"].asString();
  client_req["client_secret"] =
      app().getCustomConfig()["auth"]["logto"]["secret"].asString();

  auto ar = cpr::PostAsync(
      cpr::Url{fmt::format(
          "{}/oidc/token",
          app().getCustomConfig()["auth"]["logto"]["url"].asString())},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  common::utility::log(common::utility::LOGLEVEL::info, false, "validate: {}",
                       res.text);

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      Json::Value ret;
      ret["data"] = body;
      ret["message"] = "success";
      ret["status"] = 1;

      return callback(HttpResponse::newHttpJsonResponse(ret));
    }
  }

  Json::Value ret;
  ret["message"] = "访问用户中心失败";
  ret["status"] = 0;

  callback(HttpResponse::newHttpJsonResponse(ret));
}

void Logto::refresh(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Value client_req;
  client_req["grant_type"] = "refresh_token";
  client_req["client_id"] =
      app().getCustomConfig()["auth"]["logto"]["id"].asString();
  client_req["client_secret"] =
      app().getCustomConfig()["auth"]["logto"]["secret"].asString();
  client_req["refresh_token"] = (*json)["refresh_token"].asString();
  client_req["scope"] = (*json)["scope"].asString();

  auto ar = cpr::PostAsync(
      cpr::Url{fmt::format(
          "{}/api/login/oauth/refresh_token",
          app().getCustomConfig()["auth"]["logto"]["url"].asString())},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
      cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      Json::Value ret;
      ret["data"] = body;
      ret["message"] = "success";
      ret["status"] = 1;

      return callback(HttpResponse::newHttpJsonResponse(ret));
    }
  }

  Json::Value ret;
  ret["message"] = "访问用户中心失败";
  ret["status"] = 0;

  callback(HttpResponse::newHttpJsonResponse(ret));
}

void Logto::callback(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {}

void Logto::test(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  Json::Value ret;
  ret["message"] = "success";
  ret["status"] = 1;

  callback(HttpResponse::newHttpJsonResponse(ret));
}
} // namespace api
