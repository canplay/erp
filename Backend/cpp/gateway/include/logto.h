#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief Logto统一认证
///
class Logto : public HttpController<Logto> {
public:
  METHOD_LIST_BEGIN
  /// @brief 获取访问令牌
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Logto::access, "/access", Options, Post, "common::LogFilter");

  /// @brief 刷新访问令牌
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Logto::refresh, "/refresh", Options, Post, "common::LogFilter");

  /// @brief 处理回调请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Logto::callback, "/callback", Options, Post, "common::LogFilter");

  /// @brief 测试接口
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Logto::test, "/test", Options, Get, "common::LogtoFilter");
  METHOD_LIST_END

public:
  /// @brief 获取访问令牌
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void access(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 刷新访问令牌
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void refresh(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 处理回调请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void callback(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 测试接口
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void test(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api