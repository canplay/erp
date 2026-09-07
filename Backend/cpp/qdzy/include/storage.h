#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 仓储管理相关接口类
 */
class Storage : public HttpController<Storage> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询仓储数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含仓储数量的JSON对象
   */
  METHOD_ADD(Storage::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询仓储列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含仓储列表的JSON对象
   */
  METHOD_ADD(Storage::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询、更新或创建单个仓储信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 仓储ID（路径参数）
   * @return 返回包含仓储信息的JSON对象
   */
  METHOD_ADD(Storage::info, "/info/{id}", Options, Get, Put, Post,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询仓储数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询仓储列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询、更新或创建单个仓储信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 仓储ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;
};
} // namespace api