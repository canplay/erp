#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 车辆管理
///
class Car : public HttpController<Car> {
public:
  METHOD_LIST_BEGIN
  /// @brief 查询车辆数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Car::count, "/count", Options, Post, "common::JwtFilter");

  /// @brief 查询车辆列表
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Car::list, "/list", Options, Post, "common::JwtFilter");

  /// @brief 更新车辆信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Car::update, "/update", Options, Post, "common::JwtFilter");

  /// @brief 扣留车辆
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Car::detain, "/detain", Options, Post, "common::LogFilter");
  METHOD_LIST_END

public:
  /// @brief 查询车辆数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 查询车辆列表
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 更新车辆信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 扣留车辆
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void detain(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api