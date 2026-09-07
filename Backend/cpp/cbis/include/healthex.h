#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 体检预约相关接口类
 */
class HealthEx : public HttpController<HealthEx> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询体检预约数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含体检预约数量的JSON对象
   */
  METHOD_ADD(HealthEx::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询体检预约列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含体检预约列表的JSON对象
   */
  METHOD_ADD(HealthEx::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询单个体检预约信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 预约ID（路径参数）
   * @return 返回包含单个体检预约信息的JSON对象
   */
  METHOD_ADD(HealthEx::info, "/info/{id}", Options, Get, "common::JwtFilter");

  /**
   * @brief 更新体检预约信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(HealthEx::update, "/update", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询体检预约数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询体检预约列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个体检预约信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 预约ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 更新体检预约信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api