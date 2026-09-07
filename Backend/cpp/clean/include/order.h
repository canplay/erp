#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 订单管理
///
class Order : public HttpController<Order> {
public:
  METHOD_LIST_BEGIN
  /// @brief 查询订单数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Order::count, "/count", Options, Post, "common::LogFilter");

  /// @brief 获取订单详情
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Order::info, "/info", Options, Post, "common::LogFilter");

  /// @brief 查询遗留订单数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Order::legacy_count, "/legacy/count", Options, Post,
             "common::LogFilter");

  /// @brief 获取遗留订单详情
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Order::legacy_info, "/legacy/info", Options, Post,
             "common::LogFilter");

  /// @brief 处理遗留订单支付
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Order::legacy_payment, "/legacy/payment", Options, Post,
             "common::M2mFilter");

  /// @brief 遗留订单支付（Web端）
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Order::legacy_pay, "/legacy/pay", Options, Post,
             "common::LogFilter");

  /// @brief 遗留订单支付（PDA端）
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Order::legacy_pay_pda, "/legacy/paypda", Options, Post,
             "common::LogFilter");
  METHOD_LIST_END

public:
  /// @brief 查询订单数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取订单详情
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 查询遗留订单数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  legacy_count(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取遗留订单详情
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  legacy_info(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 遗留订单支付
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  legacy_pay(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 遗留订单支付（PDA端）
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  legacy_pay_pda(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 处理遗留订单支付
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  legacy_payment(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /// @brief 更新正式账单信息
  /// @param id 账单ID
  /// @param callback 回调函数，用于返回操作结果
  void update_formal_bill(const std::string &id,
                          std::function<void(const bool)> &&callback) const;

  /// @brief 插入支付明细信息
  /// @param id 支付ID
  /// @param payment_info 支付信息
  /// @param formal_bill 正式账单信息
  void insert_payment_detail(const std::string &id,
                             const std::string &payment_info,
                             const std::string &formal_bill) const;

  /// @brief 插入Web端支付信息
  /// @param formal_bill 正式账单信息
  /// @param payment_info 支付信息
  /// @param amount 支付金额
  /// @param ums_order 银联商务订单号
  /// @param callback 回调函数，用于返回操作结果
  void insert_payment_web(const std::string &formal_bill,
                          const std::string &payment_info, int amount,
                          const std::string &ums_order,
                          std::function<void(const bool)> &&callback) const;
};
} // namespace api