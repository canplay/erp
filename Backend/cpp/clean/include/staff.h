#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 人员管理
///
class Staff : public HttpController<Staff> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Staff::info, "/info", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api