#pragma once

#include <drogon/drogon.h>

using namespace drogon;

namespace api {
class Detail : public HttpController<Detail> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Detail::count, "/count/{id}", ::Options, Get, "common::LogFilter");
  METHOD_ADD(Detail::list, "/list", ::Options, Post, "common::LogFilter");
  METHOD_ADD(Detail::update, "/update", ::Options, Post, Put, Delete,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback,
             const std::string id) const;

  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api