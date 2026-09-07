#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 人员管理相关接口类
 */
class Staff : public HttpController<Staff> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询人员信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含人员信息的JSON对象
   */
  METHOD_ADD(Staff::info, "/info", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询人员信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api