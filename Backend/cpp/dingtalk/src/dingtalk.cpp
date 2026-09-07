#include "dingtalk.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Dingtalk::auth(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &type) const {

  Json::Value ret;
  ret["message"] = "";
  ret["status"] = 0;

  callback(HttpResponse::newHttpJsonResponse(ret));
}
} // namespace api
