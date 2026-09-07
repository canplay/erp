#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 售房订单相关接口类
 */
class SellHouse : public HttpController<SellHouse> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询售房订单数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含售房订单数量的JSON对象
   */
  METHOD_ADD(SellHouse::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询售房订单列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含售房订单列表的JSON对象
   */
  METHOD_ADD(SellHouse::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询单个售房订单信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 订单ID（路径参数）
   * @return 返回包含单个售房订单信息的JSON对象
   */
  METHOD_ADD(SellHouse::info, "/info/{id}", Options, Get, "common::JwtFilter");

  /**
   * @brief 查询指定房源价格
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param zone 区域（路径参数）
   * @param building 楼栋（路径参数）
   * @param room 房间号（路径参数）
   * @return 返回包含房源价格信息的JSON对象
   */
  METHOD_ADD(SellHouse::price, "/public/price/{zone}/{building}/{room}",
             Options, Get, "common::LogFilter");

  /**
   * @brief 查询公开售房订单列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含公开售房订单列表的JSON对象
   */
  METHOD_ADD(SellHouse::public_list, "/public/list", Options, Post,
             "common::LogFilter");

  /**
   * @brief 提交售房订单
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(SellHouse::sell, "/public/sell", Options, Post,
             "common::LogFilter");

  /**
   * @brief 更新售房订单信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(SellHouse::update, "/update", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询售房订单数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询售房订单列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个售房订单信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 订单ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 查询指定房源价格的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param zone 区域（路径参数）
   * @param building 楼栋（路径参数）
   * @param room 房间号（路径参数）
   */
  void price(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback,
             const std::string &zone, const std::string &building,
             const std::string &room) const;

  /**
   * @brief 查询公开售房订单列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  public_list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 提交售房订单的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void sell(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新售房订单信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api