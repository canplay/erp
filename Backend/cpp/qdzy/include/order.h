#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 订单相关接口类
 */
class Order : public HttpController<Order> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询订单数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含订单数量的JSON对象
   */
  METHOD_ADD(Order::count, "/count", Options, Post, "common::LogtoFilter");

  /**
   * @brief 查询订单列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含订单列表的JSON对象
   */
  METHOD_ADD(Order::list, "/list", Options, Post, "common::LogtoFilter");

  /**
   * @brief 查询单个订单信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含单个订单信息的JSON对象
   */
  METHOD_ADD(Order::info, "/info", Options, Post, "common::LogtoFilter");

  /**
   * @brief 创建订单
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Order::create, "/create", Options, Post, "common::LogFilter");

  /**
   * @brief 更新或删除订单信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Order::update, "/update", Options, Put, Delete,
             "common::LogtoFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询订单数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询订单列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个订单信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 创建订单的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新或删除订单信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /**
   * @brief 插入订单数据到数据库
   * @param json 包含订单信息的JSON对象
   * @return 返回插入结果的JSON对象
   */
  Task<Json::Value> insert(const Json::Value json) const;
};
} // namespace api