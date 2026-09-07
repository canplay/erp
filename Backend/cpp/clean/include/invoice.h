#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 非税收据管理
///
class Invoice : public HttpController<Invoice> {
public:
  METHOD_LIST_BEGIN
  /// @brief 查询非税收据数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Invoice::count, "/count", Options, Post, "common::JwtFilter");

  /// @brief 更新非税收据信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Invoice::update, "/update", Options, Post, "common::JwtFilter");

  /// @brief 获取非税收据详情
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Invoice::info, "/info", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /// @brief 查询非税收据数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 更新非税收据信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取非税收据详情
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api