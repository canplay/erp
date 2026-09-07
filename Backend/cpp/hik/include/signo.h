#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 美美停车
///
class Signo : public HttpController<Signo> {
public:
  METHOD_LIST_BEGIN

  ///
  /// @brief 开闸
  /// @return [Json::Value] json对象
  /// @par 请求示例
  /// @code
  /// post("/api/signo/open",
  /// {
  ///   "place": "示例花园地下停车场",
  ///   "name": "入口"
  /// })
  /// @endcode
  /// @par 返回示例
  /// @code
  /// {
  ///   "message": "success",
  ///   "status": 1
  /// }
  /// @endcode
  ///
  METHOD_ADD(Signo::open, "/open", Options, Post, "common::JwtFilter");
  METHOD_LIST_END

public:
  Signo();

  void open(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  struct SignoTag {
    std::string phone;
    std::string key;
  };
  std::map<std::string, SignoTag> tags;
};
} // namespace api
