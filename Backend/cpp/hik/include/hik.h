#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
///
/// @brief 海康智慧停车
///
class Hik : public HttpController<Hik> {
public:
  METHOD_LIST_BEGIN

  ///
  /// @brief 按命令调用海康智慧停车接口
  /// @return [Json::Value] json对象
  /// @note 参数信息详见海康《海康智慧停车API》
  /// @par 请求示例
  /// @code
  /// post("/api/hik/exec",
  /// {
  ///   "method": "driver",
  ///   "phone": 13300000000,
  ///   "driverId": "123abc",
  ///   "plateNo": "云H123ab",
  ///   "plateColor": "白",
  ///   "pageNo": "1",
  ///   "pageSize": "20",
  ///   "requestType": "",
  ///   "uniqueId": "",
  ///   "parkCode": "",
  ///   "isUseCoupon": "",
  ///   "appealType": "",
  ///   "appealRemark": "",
  ///   "appealInTime": "",
  ///   "appealOutTime": "",
  ///   "appealSource": "",
  ///   "arrearsIds": "",
  ///   "parkName": "",
  /// })
  /// @endcode
  /// @par 返回示例
  /// @codeg
  /// {
  ///   "data": {
  ///		"id": "123",
  ///	  },
  ///   "status": 1
  /// }
  /// @endcode
  ///
  METHOD_ADD(Hik::exec, "/exec", Options, Post, "common::LogFilter");

  METHOD_ADD(Hik::coupon, "/coupon/{method}/{type}", Options, Post,
             "common::KeyFilter");
  METHOD_LIST_END

public:
  void exec(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;
  void coupon(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback,
              const std::string &method, const std::string &type) const;

private:
  Task<std::string> getToken() const;
};
} // namespace api
