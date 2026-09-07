#pragma once

#include <drogon/drogon.h>

using namespace drogon;

namespace api {
class Comments : public HttpController<Comments> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Comments::latest, "/latest", ::Options, Get, "common::LogFilter");
  METHOD_ADD(Comments::update, "/update", ::Options, Post, Put, Delete,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  void latest(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api