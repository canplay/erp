#pragma once

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
  void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
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
  void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
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
  void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                FilterChainCallback &&fccb) override;

private:
  /**
   * @brief 验证签名
   * @param params 请求参数
   * @param token 令牌
   * @return 签名有效返回true，否则返回false
   */
  static bool isSigned(const Json::Value &params, const std::string &token);
};

/**
 * @class OidcFilter
 * @brief OIDC
 * 类型过滤器类，作为OIDC客户端，用于验证来自第三方提供商的JWT令牌并处理认证授权逻辑。
 */
class OidcFilter : public drogon::HttpFilter<OidcFilter> {
public:
  /**
   * @brief 执行过滤操作，验证请求中的OIDC令牌并检查权限。
   * @param req HTTP 请求对象指针。
   * @param fcb 过滤回调函数，用于返回错误响应。
   * @param fccb 过滤链回调函数，用于继续执行后续过滤器。
   */
  void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                FilterChainCallback &&fccb) override;

private:
  /**
   * @brief 检查用户角色是否具有访问指定路径的权限。
   * @param userRoles 用户角色列表。
   * @param path 请求路径。
   * @return 具有权限返回true，否则返回false。
   */
  static bool hasRole(const Json::Value &userRoles, const std::string &path);

  /**
   * @brief 检查用户权限范围是否包含访问指定路径所需的权限。
   * @param userScopes 用户权限范围列表。
   * @param path 请求路径。
   * @return 具有权限返回true，否则返回false。
   */
  static bool hasPermission(const Json::Value &userScopes,
                            const std::string &path);
};

} // namespace common