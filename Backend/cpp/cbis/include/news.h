#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 新闻相关接口类
 */
class News : public HttpController<News> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询最近5条新闻
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含最近5条新闻的JSON对象
   * @par 请求示例
   * @code
   * get("/api/news/latest")
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
  METHOD_ADD(News::latest, "/latest", Options, Post, "common::LogFilter");

  /**
   * @brief 查询新闻数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含新闻数量的JSON对象
   * @par 请求示例
   * @code
   * get("/api/news/count")
   * @endcode
   * @par 返回示例
   * @code
   * {
   *   "data": 100,
   *   "status": 1
   * }
   * @endcode
   */
  METHOD_ADD(News::count, "/count", Options, Post, "common::LogFilter");

  /**
   * @brief 查询多条新闻列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含新闻列表的JSON对象
   * @par 请求示例
   * @code
   * post("/api/news/list",
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
  METHOD_ADD(News::list, "/list", Options, Post, "common::LogFilter");

  /**
   * @brief 查询单条新闻信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含单条新闻信息的JSON对象
   * @par 请求示例
   * @code
   * post("/api/news/info",
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
  METHOD_ADD(News::info, "/info", Options, Post, "common::LogFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询最近5条新闻的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @bug 运行速度不稳定
   */
  void latest(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询新闻数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询多条新闻列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单条新闻信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api