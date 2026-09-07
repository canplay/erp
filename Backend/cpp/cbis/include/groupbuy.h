#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 集采订单相关接口类
 */
class GroupBuy : public HttpController<GroupBuy> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询集采订单数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含集采订单数量的JSON对象
   */
  METHOD_ADD(GroupBuy::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询集采订单列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含集采订单列表的JSON对象
   */
  METHOD_ADD(GroupBuy::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询单个集采订单信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 订单ID（路径参数）
   * @return 返回包含单个集采订单信息的JSON对象
   */
  METHOD_ADD(GroupBuy::info, "/info/{id}", Options, Get, "common::JwtFilter");

  /**
   * @brief 更新集采订单信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(GroupBuy::update, "/update", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询集采订单数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询集采订单列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个集采订单信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 订单ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 更新集采订单信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api