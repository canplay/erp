#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 通用支付
///
class Pay : public HttpController<Pay> {
public:
  METHOD_LIST_BEGIN
  /// @brief 查询支付订单数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Pay::count, "/count", Options, Post, "common::JwtFilter");

  /// @brief 查询支付订单列表
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Pay::list, "/list", Options, Post, "common::JwtFilter");

  /// @brief 查询最近已支付订单信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param id 用户ID
  METHOD_ADD(Pay::latest, "/latest/{id}", Options, Get, "common::JwtFilter");
  METHOD_LIST_END

public:
  /// @brief 查询支付订单数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 查询支付订单列表
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 查询最近已支付订单信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param id 用户ID
  void latest(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback,
              const std::string &id) const;
};
} // namespace api