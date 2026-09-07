#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
class Exec : public HttpController<Exec> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Exec::status, "/status", Options, Post, "common::M2mFilter");
  METHOD_LIST_END

public:
  void status(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  std::string get_access_token(bool refrash = false) const;
  void check_pay(std::function<void(const Json::Value &)> &&callback,
                 int nStart = 0) const;
};
} // namespace api
