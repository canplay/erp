#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 统计报表相关接口类
 */
class Statistics : public HttpController<Statistics> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询正式统计的数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含正式统计数量的JSON对象
   */
  METHOD_ADD(Statistics::formal_count, "/formal/count", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询正式统计的详细信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含正式统计详细信息的JSON对象
   */
  METHOD_ADD(Statistics::formal_info, "/formal/info", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询正式统计的汇总信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含正式统计汇总信息的JSON对象
   */
  METHOD_ADD(Statistics::formal_total, "/formal/total", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询支付信息统计的数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含支付信息统计数量的JSON对象
   */
  METHOD_ADD(Statistics::payment_info_count, "/payment_info/count", Options,
             Post, "common::JwtFilter");

  /**
   * @brief 查询支付信息统计的详细信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含支付信息统计详细信息的JSON对象
   */
  METHOD_ADD(Statistics::payment_info_info, "/payment_info/info", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询支付信息统计的汇总信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含支付信息统计汇总信息的JSON对象
   */
  METHOD_ADD(Statistics::payment_info_total, "/payment_info/total", Options,
             Post, "common::JwtFilter");

  /**
   * @brief 查询网页支付统计的数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含网页支付统计数量的JSON对象
   */
  METHOD_ADD(Statistics::payment_web_count, "/payment_web/count", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询网页支付统计的详细信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含网页支付统计详细信息的JSON对象
   */
  METHOD_ADD(Statistics::payment_web_info, "/payment_web/info", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询网页支付统计的汇总信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含网页支付统计汇总信息的JSON对象
   */
  METHOD_ADD(Statistics::payment_web_total, "/payment_web/total", Options, Post,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询正式统计数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  formal_count(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询正式统计详细信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  formal_info(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询正式统计汇总信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  formal_total(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询支付信息统计数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void payment_info_count(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询支付信息统计详细信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void payment_info_info(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询支付信息统计汇总信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void payment_info_total(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询网页支付统计数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void payment_web_count(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询网页支付统计详细信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void payment_web_info(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询网页支付统计汇总信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void payment_web_total(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;
};
} // namespace api