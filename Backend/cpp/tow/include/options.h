#pragma once

#include <drogon/drogon.h>

using namespace drogon;

namespace api {
///
/// @brief 系统设置
///
class Options : public HttpController<Options> {
public:
  METHOD_LIST_BEGIN
  /// @brief 获取车辆分类选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Options::CarClass, "/car_class", ::Options, Get,
             "common::JwtFilter");

  /// @brief 获取车辆类型选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Options::CarType, "/car_type", ::Options, Get,
             "common::JwtFilter");

  /// @brief 获取车辆颜色选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Options::CarColor, "/car_color", ::Options, Get,
             "common::JwtFilter");

  /// @brief 获取原因类型选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Options::CausesType, "/causes_type", ::Options, Get,
             "common::JwtFilter");

  /// @brief 获取原因选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Options::Causes, "/causes", ::Options, Get, "common::JwtFilter");
  METHOD_LIST_END

public:
  /// @brief 获取车辆分类选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void CarClass(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取车辆类型选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void CarType(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取车辆颜色选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void CarColor(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取原因类型选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  CausesType(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取原因选项
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void Causes(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api