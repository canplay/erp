#pragma once

#include "json/value.h"
#include <drogon/HttpFilter.h>

using namespace drogon;

namespace common {

/**
 * @class LogFilter
 * @brief 日志类型过滤器类，用于处理日志相关的过滤逻辑。
 */
class LogFilter : public drogon::HttpFilter<LogFilter> {
public:
  /**
   * @brief 执行过滤操作。
   * @param req HTTP 请求对象指针。
   * @param fcb 过滤回调函数，用于返回过滤结果。
   * @param fccb 过滤链回调函数，用于继续执行后续过滤器。
   */
  virtual void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                        FilterChainCallback &&fccb) override;
};

/**
 * @class JwtFilter
 * @brief JWT 类型过滤器类，用于处理基于 JWT 的认证过滤逻辑。
 */
class JwtFilter : public drogon::HttpFilter<JwtFilter> {
public:
  /**
   * @brief 执行过滤操作。
   * @param req HTTP 请求对象指针。
   * @param fcb 过滤回调函数，用于返回过滤结果。
   * @param fccb 过滤链回调函数，用于继续执行后续过滤器。
   */
  virtual void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                        FilterChainCallback &&fccb) override;
};

/**
 * @class KeyFilter
 * @brief Key 类型过滤器类，用于处理基于 Key 的认证过滤逻辑。
 */
class KeyFilter : public drogon::HttpFilter<KeyFilter> {
public:
  /**
   * @brief 执行过滤操作。
   * @param req HTTP 请求对象指针。
   * @param fcb 过滤回调函数，用于返回过滤结果。
   * @param fccb 过滤链回调函数，用于继续执行后续过滤器。
   */
  virtual void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                        FilterChainCallback &&fccb) override;

private:
  bool isSigned(const Json::Value &params, const std::string &token);
};

/**
 * @class M2mFilter
 * @brief m2m 类型过滤器类，用于处理基于 Machine-to-machine 的认证过滤逻辑。
 */
class M2mFilter : public drogon::HttpFilter<M2mFilter> {
public:
  /**
   * @brief 执行过滤操作。
   * @param req HTTP 请求对象指针。
   * @param fcb 过滤回调函数，用于返回过滤结果。
   * @param fccb 过滤链回调函数，用于继续执行后续过滤器。
   */
  virtual void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                        FilterChainCallback &&fccb) override;
};

/**
 * @class LogtoFilter
 * @brief Logto 类型过滤器类，用于处理基于 Logto 的认证过滤逻辑。
 */
class LogtoFilter : public drogon::HttpFilter<LogtoFilter> {
public:
  /**
   * @brief 执行过滤操作。
   * @param req HTTP 请求对象指针。
   * @param fcb 过滤回调函数，用于返回过滤结果。
   * @param fccb 过滤链回调函数，用于继续执行后续过滤器。
   */
  virtual void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                        FilterChainCallback &&fccb) override;

private:
  /**
   * @brief 检查用户角色是否匹配。
   * @param userRoles 用户角色列表。
   * @param path 请求路径。
   * @return 如果匹配返回 true，否则返回 false。
   */
  bool hasRole(const Json::Value &userRoles, const std::string &path);

  /**
   * @brief 检查用户权限是否匹配。
   * @param userScopes 用户权限列表。
   * @param path 请求路径。
   * @return 如果匹配返回 true，否则返回 false。
   */
  bool hasPermission(const Json::Value &userScopes, const std::string &path);
};
} // namespace common