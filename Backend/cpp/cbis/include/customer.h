#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
/**
 * @brief 客户相关接口类
 */
class Customer : public HttpController<Customer> {
public:
  METHOD_LIST_BEGIN

  /**
   * @brief 查询供应商数量
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含供应商数量的JSON对象
   */
  METHOD_ADD(Customer::supplier_count, "/supplier/count", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询供应商列表
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @return 返回包含供应商列表的JSON对象
   */
  METHOD_ADD(Customer::supplier_list, "/supplier/list", Options, Post,
             "common::JwtFilter");

  /**
   * @brief 查询或更新单个供应商信息
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 供应商ID（路径参数）
   * @return 返回包含供应商信息的JSON对象
   */
  METHOD_ADD(Customer::supplier_info, "/supplier/info/{id}", Options, Get, Post,
             "common::JwtFilter");
  METHOD_LIST_END

public:
  /**
   * @brief 查询供应商数量的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  supplier_count(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询供应商列表的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   */
  void
  supplier_list(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 查询或更新单个供应商信息的实现方法
   * @param req HTTP请求对象指针
   * @param callback 回调函数，用于返回HTTP响应
   * @param id 供应商ID（路径参数）
   */
  void supplier_info(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback,
                     const std::string &id) const;
};
} // namespace api