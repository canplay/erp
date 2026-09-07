#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 路由
///
class Route : public HttpController<Route> {
public:
  METHOD_LIST_BEGIN
  /// @brief 处理路由请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param method 请求方法
  /// @param path 请求路径
  METHOD_ADD(Route::route, "/route?auth={}&service={}&method={}&path={}",
             Options, Post, "common::LogFilter");

  /// @brief 处理GET请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param path 请求路径
  METHOD_ADD(Route::get, "/get?auth={}&service={}&path={}", Options, Get,
             "common::LogFilter");

  /// @brief 处理POST请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param path 请求路径
  METHOD_ADD(Route::post, "/post?auth={}&service={}&path={}", Options, Post,
             "common::LogFilter");
  METHOD_LIST_END

public:
  /// @brief 处理路由请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param method 请求方法
  /// @param path 请求路径
  void route(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback,
             const std::string &auth, const std::string &service,
             const std::string &method, const std::string &path) const;

  /// @brief 处理GET请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param path 请求路径
  void get(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback,
           const std::string &auth, const std::string &service,
           const std::string &path) const;

  /// @brief 处理POST请求
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param path 请求路径
  void post(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &auth, const std::string &service,
            const std::string &path) const;

private:
  /// @brief 获取服务信息
  /// @param service 服务名称
  /// @return 返回包含服务信息的Json::Value异步任务
  Task<Json::Value> get_service(const std::string &service) const;

  /// @brief 处理GET路由请求
  /// @param req HTTP请求指针
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param path 请求路径
  /// @return 返回包含处理结果的Json::Value异步任务
  Task<Json::Value> route_get(const HttpRequestPtr &req,
                              const std::string &auth,
                              const std::string &service,
                              const std::string &path) const;

  /// @brief 处理POST路由请求
  /// @param req HTTP请求指针
  /// @param auth 认证信息
  /// @param service 服务名称
  /// @param path 请求路径
  /// @return 返回包含处理结果的Json::Value异步任务
  Task<Json::Value> route_post(const HttpRequestPtr &req,
                               const std::string &auth,
                               const std::string &service,
                               const std::string &path) const;
};
} // namespace api