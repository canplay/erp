#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 消息通知
///
class Notify : public HttpController<Notify> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Notify::count, "/count", Options, Post, "common::JwtFilter");
  METHOD_ADD(Notify::list, "/list", Options, Post, "common::JwtFilter");
  METHOD_ADD(Notify::info, "/info/{id}", Options, Get, "common::JwtFilter");
  METHOD_ADD(Notify::public_count, "/public/count", Options, Post,
             "common::JwtFilter");
  METHOD_ADD(Notify::public_list, "/public/list", Options, Post,
             "common::JwtFilter");
  METHOD_ADD(Notify::public_info, "/public/info/{id}", Options, Get,
             "common::JwtFilter");
  METHOD_ADD(Notify::update, "/update", Options, Post, Put, Delete,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  void
  public_count(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;

  void
  public_list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  void public_info(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   const std::string &id) const;

  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  Task<Json::Value> get_user(const std::string &id) const;

  std::string get_wx_access_token() const;
  void wx_send(std::string token, Json::Value data, std::string time,
               std::string location, std::string reason, std::string remark,
               std::string next_openid) const;
};
} // namespace api
