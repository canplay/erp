#pragma once

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CompletedPart.h>

#include <drogon/HttpController.h>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>


using namespace drogon;

namespace common {

/**
 * @class Upload
 * @brief 文件上传管理类。
 *
 * 该类继承自 Drogon 框架的 HttpController，用于处理文件上传相关的 HTTP 请求。
 * 提供了预上传、分块上传、上传中状态和上传完成等完整的上传流程接口。
 */
class Upload : public HttpController<Upload> {
public:
  METHOD_LIST_BEGIN
  /**
   * @brief 注册初始化上传接口。
   *
   * 路径: /init
   * 方法: POST
   * 过滤器: common::LogFilter
   */
  METHOD_ADD(Upload::init, "/init", Options, Post, "common::LogFilter");

  /**
   * @brief 注册分块上传接口。
   *
   * 路径: /upload
   * 方法: POST
   * 过滤器: common::LogFilter
   */
  METHOD_ADD(Upload::upload, "/upload", Options, Post, "common::LogFilter");

  /**
   * @brief 注册完成上传接口。
   *
   * 路径: /complete
   * 方法: POST
   * 过滤器: common::LogFilter
   */
  METHOD_ADD(Upload::complete, "/complete", Options, Post, "common::LogFilter");
  
  /**
   * @brief 注册预上传接口。
   *
   * 路径: /preupload
   * 方法: POST
   * 过滤器: common::LogFilter
   */
  METHOD_ADD(Upload::preUpload, "/preupload", Options, Post, "common::LogFilter");
  
  /**
   * @brief 注册上传中接口。
   *
   * 路径: /uploading
   * 方法: POST
   * 过滤器: common::LogFilter
   */
  METHOD_ADD(Upload::uploading, "/uploading", Options, Post, "common::LogFilter");
  
  /**
   * @brief 注册上传完成接口。
   *
   * 路径: /uploaded
   * 方法: POST
   * 过滤器: common::LogFilter
   */
  METHOD_ADD(Upload::uploaded, "/uploaded", Options, Post, "common::LogFilter");

  METHOD_LIST_END

public:
  /**
   * @brief 处理初始化上传请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void init(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);

  /**
   * @brief 处理分块上传请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void upload(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);

  /**
   * @brief 处理完成上传请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void complete(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
                
  /**
   * @brief 处理预上传请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void preUpload(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
                 
  /**
   * @brief 处理上传中请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void uploading(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
                 
  /**
   * @brief 处理上传完成请求
   *
   * @param req HTTP 请求对象指针
   * @param callback 回调函数，用于返回 HTTP 响应
   */
  void uploaded(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);

private:
  /// 上传上下文结构体
  struct UploadContext {
    std::string uploadId;
    std::string filename;
    std::string filetype;
    std::string bucket;
    std::string key;
    std::unordered_map<int, std::string> etags;
  };

  /// S3客户端实例
  static std::shared_ptr<Aws::S3::S3Client> mS3client;
  
  /// 上传上下文映射表
  static std::unordered_map<std::string, UploadContext> mContexts;
  
  /// 上传上下文互斥锁
  static std::mutex mContextMutex;

  /**
   * @brief 获取S3客户端实例
   *
   * @return S3客户端共享指针
   */
  static std::shared_ptr<Aws::S3::S3Client> getS3Client();

  /**
   * @brief 验证JSON参数
   *
   * @param json JSON对象指针
   * @param callback 回调函数
   * @return 参数有效返回true，否则返回false
   */
  static bool validateJsonParams(const std::shared_ptr<Json::Value> &json,
                                std::function<void(const HttpResponsePtr &)> &callback);
};

} // namespace common