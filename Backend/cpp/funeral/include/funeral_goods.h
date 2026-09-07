#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 商超管理
///
class FuneralGoods : public HttpController<FuneralGoods> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(FuneralGoods::count, "/count", Options, Post, "common::JwtFilter");
  METHOD_ADD(FuneralGoods::add, "/add", Options, Post, "common::JwtFilter");
  METHOD_ADD(FuneralGoods::update, "/update", Options, Post,
             "common::JwtFilter");
  METHOD_ADD(FuneralGoods::info, "/info", Options, Post, "common::JwtFilter");
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
