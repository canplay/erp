#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
class Sms : public HttpController<Sms> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Sms::count, "/count", Options, Post, "common::JwtFilter");
  METHOD_ADD(Sms::list, "/list", Options, Post, "common::JwtFilter");
  METHOD_ADD(Sms::verify, "/verify", Options, Post, "common::LogFilter");
  METHOD_ADD(Sms::send, "/send", Options, Post, "common::JwtFilter");
  METHOD_ADD(Sms::send, "/custom_send", Options, Post, "common::M2mFilter");
  METHOD_LIST_END

public:
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  void verify(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  void send(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  Task<Json::Value> get_user(const std::string &id) const;
};
} // namespace api
