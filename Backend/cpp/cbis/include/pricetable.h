#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 市场价格公示相关接口类
 */
class PriceTable : public HttpController<PriceTable> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询市场价格数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含市场价格数量的JSON对象
   */
  METHOD_ADD(PriceTable::count, "/count", Options, Post, "common::LogFilter");

  /**
   * @brief 查询市场价格列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含市场价格列表的JSON对象
   */
  METHOD_ADD(PriceTable::list, "/list", Options, Post, "common::LogFilter");

  /**
   * @brief 查询单个市场价格信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 市场价格ID（路径参数）
   * @return 返回包含单个市场价格信息的JSON对象
   */
  METHOD_ADD(PriceTable::info, "/info/{id}", Options, Get, "common::LogFilter");

  /**
   * @brief 更新市场价格信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(PriceTable::update, "/update", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询市场价格数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询市场价格列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个市场价格信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 市场价格ID（路径参数）
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /**
   * @brief 更新市场价格信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api