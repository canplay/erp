#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 银联商务支付
///
class Ums : public HttpController<Ums> {
public:
  METHOD_LIST_BEGIN
  /// @brief 查询支付订单信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Ums::query, "/query", Options, Post, "common::LogFilter");

  /// @brief 生成支付订单
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Ums::order, "/order", Options, Post, "common::LogFilter");

  /// @brief 支付订单状态通知
  /// @warning 此接口用于银行系统通知, 请勿占用
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Ums::notify, "/notify", Options, Post, "common::M2mFilter");

  /// @brief 关闭支付订单
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Ums::close, "/close", Options, Post, "common::M2mFilter");

  /// @brief 支付订单退款
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Ums::refund, "/refund", Options, Post, "common::M2mFilter");

  /// @brief 查询支付订单信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(Ums::info, "/info", Options, Post, "common::M2mFilter");
  METHOD_LIST_END

public:
  /// @brief 查询支付订单信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void query(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 生成支付订单
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void order(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 支付订单状态通知
  /// @warning 此接口用于银行系统通知, 请勿占用
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void notify(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 关闭支付订单
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void close(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 支付订单退款
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void refund(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 查询支付订单信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /// @brief 获取访问令牌
  /// @return 访问令牌字符串
  std::string get_access_token() const;
};
} // namespace api