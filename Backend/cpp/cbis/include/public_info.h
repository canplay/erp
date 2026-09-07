#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 公共接口相关类
 */
class PublicInfo : public HttpController<PublicInfo> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询公共信息数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含公共信息数量的JSON对象
   */
  METHOD_ADD(PublicInfo::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询公共信息列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含公共信息列表的JSON对象
   */
  METHOD_ADD(PublicInfo::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询单个公共信息详情
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 信息ID（路径参数）
   * @return 返回包含单个公共信息详情的JSON对象
   */
  METHOD_ADD(PublicInfo::info, "/info/{id}", Options, Get, "common::JwtFilter");

  /**
   * @brief 更新公共信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(PublicInfo::update, "/update", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询公共信息数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询公共信息列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个公共信息详情的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 信息ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 更新公共信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api