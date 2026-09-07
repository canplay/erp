#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 用户权限管理相关接口类
 */
class Permission : public HttpController<Permission> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询用户权限数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含用户权限数量的JSON对象
   */
  METHOD_ADD(Permission::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询单个用户权限信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 权限ID（路径参数）
   * @return 返回包含单个用户权限信息的JSON对象
   */
  METHOD_ADD(Permission::info, "/info/{id}", Options, Get, "common::JwtFilter");

  /**
   * @brief 查询用户权限列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含用户权限列表的JSON对象
   */
  METHOD_ADD(Permission::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 更新用户权限信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Permission::update, "/update", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询用户权限数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个用户权限信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 权限ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 查询用户权限列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新用户权限信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api