#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 公共资源资产管理相关接口类
 */
class PublicAssets : public HttpController<PublicAssets> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询公共资源资产数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含公共资源资产数量的JSON对象
   */
  METHOD_ADD(PublicAssets::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 添加公共资源资产
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(PublicAssets::add, "/add", Options, Post, "common::JwtFilter");

  /**
   * @brief 更新公共资源资产信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(PublicAssets::update, "/update", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询公共资源资产信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含公共资源资产信息的JSON对象
   */
  METHOD_ADD(PublicAssets::info, "/info", Options, Post, "common::JwtFilter");

  /**
   * @brief 开启道闸
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(PublicAssets::dz_open, "/dz/open", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 添加公共资源资产的历史记录
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(PublicAssets::history_add, "/history/add", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 更新公共资源资产的历史记录
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(PublicAssets::history_update, "/history/update", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询公共资源资产的历史记录信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含历史记录信息的JSON对象
   */
  METHOD_ADD(PublicAssets::history_info, "/history/info", Options, Post,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询公共资源资产数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 添加公共资源资产的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void add(const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新公共资源资产信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询公共资源资产信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 开启道闸的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void dz_open(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 添加公共资源资产历史记录的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  history_add(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新公共资源资产历史记录的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  history_update(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询公共资源资产历史记录信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  history_info(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api