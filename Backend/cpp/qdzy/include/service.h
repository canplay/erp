#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 服务相关接口类
 */
class Service : public HttpController<Service> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询服务数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含服务数量的JSON对象
   */
  METHOD_ADD(Service::count, "/count", Options, Post, "common::LogFilter");

  /**
   * @brief 查询服务列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含服务列表的JSON对象
   */
  METHOD_ADD(Service::list, "/list", Options, Post, "common::LogFilter");

  /**
   * @brief 查询单个服务信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含单个服务信息的JSON对象
   */
  METHOD_ADD(Service::info, "/info", Options, Post, "common::LogFilter");

  /**
   * @brief 创建服务
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Service::create, "/create", Options, Post, "common::LogtoFilter");

  /**
   * @brief 更新或删除服务信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Service::update, "/update", Options, Put, Delete,
             "common::LogtoFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询服务数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询服务列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个服务信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 创建服务的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新或删除服务信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /**
   * @brief 插入服务数据到数据库
   * @param json 包含服务信息的JSON对象
   * @return 返回插入结果的JSON对象
   */
  Task<Json::Value> insert(const Json::Value json) const;
};
} // namespace api