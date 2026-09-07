#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 操作日志相关接口类
 */
class Log : public HttpController<Log> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询日志数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含日志数量的JSON对象
   * @par 请求示例
   * @code
   * get("/api/log/count")
   * @endcode
   * @par 返回示例
   * @code
   * {
   *   "data": 100,
   *   "status": 1
   * }
   * @endcode
   */
  METHOD_ADD(Log::count, "/count", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询多条日志列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含日志列表的JSON对象
   * @par 请求示例
   * @code
   * post("/api/log/list",
   * {
   *   "curPage": "1",
   *   "maxPage": "20"
   * })
   * @endcode
   * @par 返回示例
   * @code
   * {
   *   "data": [{
   *		"id": "123abc",
   *     "title": "新闻一"
   *	  },
   *   {
   *		"id": "345cde",
   *     "title": "新闻二"
   *	  }],
   *   "status": 1
   * }
   * @endcode
   */
  METHOD_ADD(Log::list, "/list", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询单条日志信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含单条日志信息的JSON对象
   * @par 请求示例
   * @code
   * post("/api/log/info",
   * {
   *   "id": "123abc",
   * })
   * @endcode
   * @par 返回示例
   * @code
   * {
   *   "data": {
   *		"id": "123abc",
   *     "title": "新闻一"
   *	  },
   *   "status": 1
   * }
   * @endcode
   */
  METHOD_ADD(Log::info, "/info", Options, Post, "common::JwtFilter");

  /**
   * @brief 修改单条日志信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回修改后的日志信息的JSON对象
   * @par 请求示例
   * @code
   * post("/api/log/update",
   * {
   *   "id": "123abc",
   * })
   * @endcode
   * @par 返回示例
   * @code
   * {
   *   "data": {
   *		"id": "123abc",
   *     "title": "新闻一"
   *	  },
   *   "status": 1
   * }
   * @endcode
   */
  METHOD_ADD(Log::update, "/update", Options, Put, Post, "common::LogFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询日志数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询多条日志列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单条日志信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 修改单条日志信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api