#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 仓储管理
///
class Storage : public HttpController<Storage> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Storage::count, "/count", Options, Post, "common::JwtFilter");
  METHOD_ADD(Storage::list, "/list", Options, Post, "common::JwtFilter");
  METHOD_ADD(Storage::info, "/info/{id}", Options, Get, Put, Post,
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
};
} // namespace api
