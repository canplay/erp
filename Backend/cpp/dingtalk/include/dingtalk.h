#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
class Dingtalk : public HttpController<Dingtalk> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Dingtalk::auth, "/auth/{type}", Options, Get, "common::LogFilter");
  METHOD_LIST_END

public:
  void auth(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &type) const;
};
} // namespace api
