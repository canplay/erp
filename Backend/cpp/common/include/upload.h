#pragma once

#include <aws/core/Aws.h>
#include <aws/core/config/defaults/ClientConfigurationDefaults.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CompletedPart.h>

#include <drogon/HttpController.h>

using namespace drogon;

namespace common {

/**
 * @class Upload
 * @brief 文件上传管理类。
 *
 * 该类继承自 Drogon 框架的 HttpController，用于处理文件上传相关的 HTTP 请求。
 * 提供了初始化上传、分块上传和完成上传的功能接口。
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
  METHOD_LIST_END

public:
  /**
   * @brief 初始化上传任务。
   *
   * @param req HTTP 请求对象指针。
   * @param callback 回调函数，用于返回 HTTP 响应。
   */
  void init(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 处理分块上传请求。
   *
   * @param req HTTP 请求对象指针。
   * @param callback 回调函数，用于返回 HTTP 响应。
   */
  void upload(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback) const;

  /**
   * @brief 完成上传任务。
   *
   * @param req HTTP 请求对象指针。
   * @param callback 回调函数，用于返回 HTTP 响应。
   */
  void complete(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const;

private:
  /**
   * @brief 获取 S3 客户端实例。
   *
   * @return 返回一个共享指针，指向 Aws::S3::S3Client 实例。
   */
  static std::shared_ptr<Aws::S3::S3Client> getS3Client();

private:
  /**
   * @struct UploadContext
   * @brief 上传上下文结构体。
   *
   * 用于存储文件上传过程中的相关信息，包括上传 ID、分块信息等。
   */
  struct UploadContext {
    std::string uploadId;     ///< 上传任务的唯一标识符。
    std::string bucket;       ///< S3 存储桶名称。
    std::string objectKey;    ///< S3 对象键（文件路径）。
    size_t totalSize = 0;     ///< 文件总大小（字节）。
    size_t chunkSize = 0;     ///< 分块大小（字节）。
    size_t uploadedBytes = 0; ///< 已上传的字节数。
    std::vector<Aws::S3::Model::CompletedPart> parts; ///< 已完成的分块列表。
    std::mutex mutex; ///< 互斥锁，用于线程安全。

    /**
     * @brief 构造函数，初始化上传上下文。
     *
     * @param id 上传任务的唯一标识符。
     * @param b S3 存储桶名称。
     * @param k S3 对象键（文件路径）。
     * @param total 文件总大小（字节）。
     * @param chunk 分块大小（字节）。
     */
    UploadContext(std::string id, std::string b, std::string k, size_t total,
                  size_t chunk)
        : uploadId(std::move(id)), bucket(std::move(b)),
          objectKey(std::move(k)), totalSize(total), chunkSize(chunk) {}

    /**
     * @brief 移动构造函数。
     *
     * @param other 另一个 UploadContext 对象。
     */
    UploadContext(UploadContext &&other) noexcept
        : uploadId(std::move(other.uploadId)), bucket(std::move(other.bucket)),
          objectKey(std::move(other.objectKey)), totalSize(other.totalSize),
          chunkSize(other.chunkSize), uploadedBytes(other.uploadedBytes),
          parts(std::move(other.parts)) {}

    /**
     * @brief 删除拷贝构造函数。
     */
    UploadContext(const UploadContext &) = delete;

    /**
     * @brief 删除赋值操作符。
     */
    UploadContext &operator=(const UploadContext &) = delete;

    /**
     * @brief 判断当前分块是否为最后一块。
     *
     * @param currentChunkSize 当前分块的大小。
     * @return 如果是最后一块，返回 true；否则返回 false。
     */
    bool isLastPart(size_t currentChunkSize) const {
      return (uploadedBytes + currentChunkSize) >= totalSize ||
             currentChunkSize < chunkSize;
    }
  };

  static std::shared_ptr<Aws::S3::S3Client> mS3client; ///< S3 客户端实例。
  static std::mutex mContextMutex; ///< 互斥锁，用于保护上传上下文。
  static std::unordered_map<std::string, UploadContext>
      mContexts; ///< 上传上下文映射表。
};
} // namespace common