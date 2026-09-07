#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 商城管理相关接口类
 */
class Shop : public HttpController<Shop> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询商城商品数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含商城商品数量的JSON对象
   */
  METHOD_ADD(Shop::count, "/count", Options, Post, "common::LogFilter");

  /**
   * @brief 查询商城商品列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含商城商品列表的JSON对象
   */
  METHOD_ADD(Shop::list, "/list", Options, Post, "common::LogFilter");

  /**
   * @brief 查询单个商城商品信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 商品ID（路径参数）
   * @return 返回包含单个商城商品信息的JSON对象
   */
  METHOD_ADD(Shop::info, "/info/{id}", Options, Get, "common::LogFilter");

  /**
   * @brief 更新商城商品信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Shop::update, "/update", Options, Post, "common::JwtFilter");

  /**
   * @brief 处理商城订单
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Shop::order, "/order", Options, Post, "common::JwtFilter");

  /**
   * @brief 处理支付通知
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Shop::notify, "/notify", Options, Post, "common::M2mFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询商城商品数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询商城商品列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个商城商品信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 商品ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 更新商城商品信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 处理商城订单的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void order(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 处理支付通知的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void notify(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /**
   * @brief 检查支付状态
   * @param id 订单ID
   */
  void check_payment(const std::string &id) const;
};
} // namespace api