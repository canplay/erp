#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace common {

/**
 * @brief 服务管理类
 *
 * 该类继承自 Drogon 框架的 HttpController，用于处理 HTTP 请求。
 * 提供了服务更新和健康检查的功能接口。
 */
class Service : public HttpController<Service> {
public:
  METHOD_LIST_BEGIN
  /**
   * @brief 注册服务更新接口
   *
   * 路径: /update
   * 方法: POST
   * 过滤器: common::M2mFilter
   */
  METHOD_ADD(Service::update, "/update", Options, Post, "common::M2mFilter");

  /**
   * @brief 注册健康检查接口
   *
   * 路径: /health
   * 方法: GET
   * 过滤器: common::M2mFilter
   */
  METHOD_ADD(Service::health, "/health", Options, Get, "common::M2mFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 处理服务更新请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 处理健康检查请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void health(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /**
   * @brief 检查服务状态
   *
   * @param service 服务名称
   * @param url 服务地址
   */
  void check(std::string service, std::string url) const;
};
} // namespace common