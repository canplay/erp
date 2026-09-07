#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 订单管理
///
class FuneralOrder : public HttpController<FuneralOrder> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(FuneralOrder::count, "/count", Options, Post, "common::JwtFilter");
  METHOD_ADD(FuneralOrder::add, "/add", Options, Post, "common::JwtFilter");
  METHOD_ADD(FuneralOrder::update, "/update", Options, Post,
             "common::JwtFilter");
  METHOD_ADD(FuneralOrder::info, "/info", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  void add(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback) const;

  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api
