#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 系统设置相关接口类
 */
class Option : public HttpController<Option> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询系统设置项数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含系统设置项数量的JSON对象
   */
  METHOD_ADD(Option::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询单个系统设置项信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 设置项ID（路径参数）
   * @return 返回包含单个系统设置项信息的JSON对象
   */
  METHOD_ADD(Option::info, "/info/{id}", Options, Get, "common::JwtFilter");

  /**
   * @brief 查询系统设置项列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含系统设置项列表的JSON对象
   */
  METHOD_ADD(Option::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 更新系统设置项信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Option::update, "/update", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询系统设置项数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个系统设置项信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 设置项ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 查询系统设置项列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新系统设置项信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api