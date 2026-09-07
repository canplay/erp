#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 项目管理相关接口类
 */
class Project : public HttpController<Project> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询项目数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含项目数量的JSON对象
   */
  METHOD_ADD(Project::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询项目列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含项目列表的JSON对象
   */
  METHOD_ADD(Project::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询公开项目列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含公开项目列表的JSON对象
   */
  METHOD_ADD(Project::public_list, "/public/list", Options, Post,
             "common::LogFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询项目数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询项目列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询公开项目列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  public_list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api