#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 区域管理相关接口类
 */
class Zone : public HttpController<Zone> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询区域数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含区域数量的JSON对象
   */
  METHOD_ADD(Zone::count, "/count", Options, Post, "common::LogFilter");

  /**
   * @brief 查询区域列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含区域列表的JSON对象
   */
  METHOD_ADD(Zone::list, "/list", Options, Post, "common::LogFilter");

  /**
   * @brief 查询单个区域信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含单个区域信息的JSON对象
   */
  METHOD_ADD(Zone::info, "/info", Options, Post, "common::LogFilter");

  /**
   * @brief 创建区域
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Zone::create, "/create", Options, Post, "common::LogtoFilter");

  /**
   * @brief 更新或删除区域信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Zone::update, "/update", Options, Put, Delete,
             "common::LogtoFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询区域数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询区域列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个区域信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 创建区域的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void create(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新或删除区域信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /**
   * @brief 插入区域数据到数据库
   * @param json 包含区域信息的JSON对象
   * @return 返回插入结果的JSON对象
   */
  Task<Json::Value> insert(const Json::Value json) const;
};
} // namespace api