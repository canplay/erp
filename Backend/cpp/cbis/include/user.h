#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 用户管理
///
class User : public HttpController<User> {
public:
  METHOD_LIST_BEGIN
  /// @brief 获取用户数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::count, "/count", Options, Post, "common::LogFilter");

  /// @brief 用户注册
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::signup, "/signup", Options, Post, "common::LogFilter");

  /// @brief 用户登录
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::signin, "/signin", Options, Post, "common::LogFilter");

  /// @brief 微信登录
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::wechat, "/wechat", Options, Post, "common::LogFilter");

  /// @brief 用户登出
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param id 用户ID
  METHOD_ADD(User::signout, "/signout/{id}", Options, Get, "common::JwtFilter");

  /// @brief 获取用户信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param id 用户ID
  METHOD_ADD(User::info, "/info/{id}", Options, Get, "common::JwtFilter");

  /// @brief 获取公共用户信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::public_info, "/public/info", Options, Post,
             "common::LogFilter");

  /// @brief 获取用户列表
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::list, "/list", Options, Post, "common::JwtFilter");

  /// @brief 修改用户密码
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::password, "/password", Options, Post, "common::JwtFilter");

  /// @brief 更新用户信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::update, "/update", Options, Post, "common::JwtFilter");

  /// @brief 添加用户附加信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::additive, "/additive", Options, Post, "common::JwtFilter");

  /// @brief 获取用户权限
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  METHOD_ADD(User::permission, "/permission", Options, Post,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  /// @brief 获取用户数量
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void count(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 用户注册
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void signup(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 微信登录
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void wechat(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 用户登录
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void signin(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 用户登出
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param id 用户ID
  void signout(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback,
               const std::string &id) const;

  /// @brief 获取用户信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  /// @param id 用户ID
  void info(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &id) const;

  /// @brief 获取公共用户信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  public_info(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取用户列表
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 修改用户密码
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void password(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 更新用户信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void update(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 添加用户附加信息
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void additive(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取用户权限
  /// @param req HTTP请求指针
  /// @param callback 回调函数，用于返回HTTP响应
  void
  permission(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;

  /// @brief 获取用户详细信息
  /// @param id 用户ID
  /// @return 返回包含用户信息的Json::Value异步任务
  static Task<Json::Value> get_user(const std::string &id);
};
} // namespace api