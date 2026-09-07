#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 票务管理相关接口类
 */
class Ticket : public HttpController<Ticket> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询票务数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含票务数量的JSON对象
   */
  METHOD_ADD(Ticket::count, "/count", Options, Post, "common::LogFilter");

  /**
   * @brief 查询票务列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含票务列表的JSON对象
   */
  METHOD_ADD(Ticket::list, "/list", Options, Post, "common::LogFilter");

  /**
   * @brief 查询票务信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含票务信息的JSON对象
   */
  METHOD_ADD(Ticket::info, "/info", Options, Post, "common::LogFilter");

  /**
   * @brief 更新、创建或删除票务信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Ticket::update, "/update", Options, Put, Post, Delete,
             "common::JwtFilter");

  /**
   * @brief 查询单个票务信息（公开接口）
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   * @return 返回包含单个票务信息的JSON对象
   */
  METHOD_ADD(Ticket::public_info, "/public/info/{id}", Options, Get,
             "common::JwtFilter");

  /**
   * @brief 查询可用票务数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含可用票务数量的JSON对象
   */
  METHOD_ADD(Ticket::available_count, "/available/count", Options, Post,
             "common::LogFilter");

  /**
   * @brief 查询可用票务列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含可用票务列表的JSON对象
   */
  METHOD_ADD(Ticket::available_list, "/available/list", Options, Post,
             "common::LogFilter");

  /**
   * @brief 处理票务订单
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Ticket::order, "/order", Options, Post, "common::JwtFilter");

  /**
   * @brief 处理支付通知
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Ticket::notify, "/notify", Options, Post, "common::M2mFilter");

  /**
   * @brief 接收票务
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Ticket::recive, "/recive", Options, Post, "common::JwtFilter");

  /**
   * @brief 查询已接收的票务信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   * @return 返回包含已接收票务信息的JSON对象
   */
  METHOD_ADD(Ticket::recived, "/recived/{id}", Options, Get,
             "common::JwtFilter");

  /**
   * @brief 查询票务验证历史
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   * @return 返回包含票务验证历史的JSON对象
   */
  METHOD_ADD(Ticket::verify_history, "/history/{id}", Options, Get,
             "common::JwtFilter");

  /**
   * @brief 验证票务
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  METHOD_ADD(Ticket::verify, "/verify", Options, Post, "common::JwtFilter");

  /**
   * @brief 退款票务
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   */
  METHOD_ADD(Ticket::refund, "/refund/{id}", Options, Get, "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询票务数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询票务列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询票务信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 更新、创建或删除票务信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询单个票务信息（公开接口）的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   */
  void public_info(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   const std::string &id) const;

  /**
   * @brief 查询可用票务数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void available_count(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询可用票务列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  available_list(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 处理票务订单的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void order(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 处理支付通知的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void notify(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 接收票务的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void recive(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询已接收的票务信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   */
  void recived(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback,
               const std::string &id) const;

  /**
   * @brief 查询票务验证历史的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   */
  void verify_history(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &id) const;

  /**
   * @brief 验证票务的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void verify(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 退款票务的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 票务ID（路径参数）
   */
  void refund(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback,
              const std::string &id) const;

private:
  /**
   * @brief 检查购票限制
   * @param title 票务标题
   * @param user 用户ID
   * @param callback 回调函数，用于返回检查结果
   */
  void check_limit(const std::string &title, const std::string &user,
                   std::function<void(const bool)> &&callback) const;

  /**
   * @brief 检查票务可用性
   * @param id 票务ID
   * @param callback 回调函数，用于返回可用数量
   */
  void check_available(const std::string &id,
                       std::function<void(const int)> &&callback) const;

  /**
   * @brief 检查支付状态
   * @param id 票务ID
   */
  void check_payment(const std::string &id) const;
};
} // namespace api