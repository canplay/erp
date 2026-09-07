#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 订单类
///
class Order : public HttpController<Order> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Order::count, "/count", Options, Post, "common::JwtFilter");
  METHOD_ADD(Order::list, "/list", Options, Post, "common::JwtFilter");
  METHOD_ADD(Order::info, "/info/{id}", Options, Get, Put, Post,
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

private:
  Task<Json::Value> insert(const Json::Value json) const;
};
} // namespace api
