#include "upload.h"
#include "logger.h"
#include "utils/crypto.h"
#include "utils/string.h"

#include <drogon/MultiPart.h>
#include <drogon/drogon.h>

#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/s3/model/CompleteMultipartUploadRequest.h>
#include <aws/s3/model/CreateMultipartUploadRequest.h>
#include <aws/s3/model/UploadPartRequest.h>

namespace common {
std::shared_ptr<Aws::S3::S3Client> Upload::mS3client = nullptr;
std::mutex Upload::mContextMutex;
std::unordered_map<std::string, Upload::UploadContext> Upload::mContexts;

namespace {
// 创建一个辅助函数来发送标准JSON响应
inline void
sendJsonResponse(const std::function<void(const HttpResponsePtr &)> &callback,
                 const std::string &message, int status,
                 Json::Value data = Json::nullValue) {
  Json::Value ret;
  ret["data"] = data;
  ret["message"] = message;
  ret["status"] = status;
  callback(HttpResponse::newHttpJsonResponse(ret));
}

// 创建一个辅助函数来处理AWS错误
inline void
handleAwsError(const std::string &operation,
               const Aws::Client::AWSError<Aws::S3::S3Errors> &error,
               const std::function<void(const HttpResponsePtr &)> &callback) {
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, true,
                                    "{}失败: {} - {}:{}", operation,
                                    error.GetMessage(), __FILE__, __LINE__);

  sendJsonResponse(callback, operation + "失败: " + error.GetMessage(), 0);
}
} // namespace

std::shared_ptr<Aws::S3::S3Client> Upload::getS3Client() {
  static std::once_flag initFlag;
  std::call_once(initFlag, [] {
    Aws::Client::ClientConfiguration config;
    config.endpointOverride =
        app().getCustomConfig()["storage"]["s3"]["url"].asString();
    config.region =
        app().getCustomConfig()["storage"]["s3"]["region"].asString();

    Aws::Auth::AWSCredentials credentials(
        app().getCustomConfig()["storage"]["s3"]["id"].asString(),
        app().getCustomConfig()["storage"]["s3"]["key"].asString());

    mS3client = std::make_shared<Aws::S3::S3Client>(
        credentials, config,
        Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, false);
  });
  return mS3client;
}

bool Upload::validateJsonParams(
    const std::shared_ptr<Json::Value> &json,
    std::function<void(const HttpResponsePtr &)> &callback) {
  if (!json) {
    sendJsonResponse(callback, "参数错误", 0);
    return false;
  }
  return true;
}

void Upload::preUpload(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!validateJsonParams(json, callback)) {
    return;
  }

  // 解码文件内容
  std::string fileContent =
      utils::Crypto::base64Decode(json->get("content", "").asString());

  // 生成文件名和路径
  std::string filename = utils::String::toLower(
      utils::String::trim(json->get("filename", "").asString()));
  std::string filetype = utils::String::toLower(
      utils::String::trim(json->get("filetype", "").asString()));
  std::string timestamp = std::to_string(std::time(nullptr));
  std::string key = fmt::format("{}/{}/{}", filetype, timestamp, filename);

  // 计算文件内容的SHA256哈希值
  std::string contentSha256 = utils::Crypto::sha256(fileContent);
  std::string file_id = contentSha256;

  // 创建多部分上传
  auto request = Aws::S3::Model::CreateMultipartUploadRequest();
  request.SetBucket(
      app().getCustomConfig()["storage"]["s3"]["bucket"].asString());
  request.SetKey(file_id);
  request.SetContentType(filetype);

  auto createUploadOutcome = getS3Client()->CreateMultipartUpload(request);
  if (!createUploadOutcome.IsSuccess()) {
    return handleAwsError("创建多部分上传", createUploadOutcome.GetError(),
                          callback);
  }

  // 保存上传上下文
  UploadContext context;
  context.uploadId = createUploadOutcome.GetResult().GetUploadId();
  context.filename = filename;
  context.filetype = filetype;
  context.bucket =
      app().getCustomConfig()["storage"]["s3"]["bucket"].asString();
  context.key = file_id;

  {
    std::lock_guard<std::mutex> lock(mContextMutex);
    mContexts[file_id] = std::move(context);
  }

  Json::Value data;
  data["upload_id"] = file_id;
  sendJsonResponse(callback, "success", 1, data);
}

void Upload::init(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback) {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!validateJsonParams(json, callback)) {
    return;
  }

  std::string filename = (*json)["filename"].asString();
  std::string filetype = (*json)["filetype"].asString();
  int totalSize = (*json)["total_size"].asInt();

  if (filename.empty()) {
    return sendJsonResponse(callback, "文件名不能为空", 0);
  }

  if (totalSize <= 0) {
    return sendJsonResponse(callback, "文件大小必须大于0", 0);
  }

  // 生成文件名
  std::string key = filename;
  std::string extension;
  size_t dotPos = filename.find_last_of('.');
  if (dotPos != std::string::npos) {
    extension = filename.substr(dotPos + 1);
  }

  std::string contentType = filetype;
  if (contentType.empty()) {
    contentType = "application/octet-stream";
  }

  std::string bucket =
      app().getCustomConfig()["storage"]["s3"]["bucket"].asString();

  // 创建多部分上传
  try {
    if (totalSize <= 0) {
      throw std::runtime_error("文件大小不能为0");
    }

    Aws::S3::Model::CreateMultipartUploadRequest request;
    request.SetBucket(bucket);
    if (!extension.empty()) {
      request.SetKey(key + "." + extension);
    } else {
      request.SetKey(key);
    }
    request.SetContentType(contentType);

    auto outcome = getS3Client()->CreateMultipartUpload(request);
    if (!outcome.IsSuccess()) {
      return handleAwsError("创建分块上传", outcome.GetError(), callback);
    }

    const std::string uploadId = outcome.GetResult().GetUploadId();

    // 保存上传上下文
    {
      std::lock_guard<std::mutex> lock(mContextMutex);
      mContexts.try_emplace(
          uploadId,
          UploadContext{uploadId, filename, filetype, bucket,
                        key + (extension.empty() ? "" : "." + extension),
                        std::unordered_map<int, std::string>()});
    }

    sendJsonResponse(callback, "success", 1, uploadId);
  } catch (const std::exception &e) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, true,
                                      "初始化上传失败: {} - {}:{}", e.what(),
                                      __FILE__, __LINE__);

    sendJsonResponse(callback, e.what(), 0);
  }
}

void Upload::upload(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) {
  MultiPartParser parser;
  if (parser.parse(req) != 0) {
    return sendJsonResponse(callback, "请求解析失败", 0);
  }

  // 获取参数
  std::string uploadId = parser.getParameter<std::string>("uploadId");
  int partNumber = parser.getParameter<int>("partNumber");
  std::string contentMd5 = parser.getParameter<std::string>("contentMd5");

  if (uploadId.empty()) {
    return sendJsonResponse(callback, "缺少上传ID", 0);
  }

  if (partNumber <= 0) {
    return sendJsonResponse(callback, "分块编号必须大于0", 0);
  }

  if (contentMd5.empty()) {
    return sendJsonResponse(callback, "缺少MD5校验值", 0);
  }

  // 获取上传上下文
  UploadContext context;
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    auto it = Upload::mContexts.find(uploadId);
    if (it == Upload::mContexts.end()) {
      return sendJsonResponse(callback, "无效的上传ID", 0);
    }
    context = it->second;
  }

  // 获取上传文件
  auto files = parser.getFiles();
  if (files.empty()) {
    return sendJsonResponse(callback, "没有找到上传文件", 0);
  }

  auto file = files[0];
  auto fileData = file.fileContent();

  // 验证MD5
  Aws::Utils::ByteBuffer md5Bytes =
      Aws::Utils::HashingUtils::CalculateMD5(fileData.data());
  std::string serverMd5 = Aws::Utils::HashingUtils::HexEncode(md5Bytes);

  if (serverMd5 != contentMd5) {
    return sendJsonResponse(callback, "MD5校验失败", 0);
  }

  // 上传分块
  auto request = Aws::S3::Model::UploadPartRequest();
  request.SetBucket(context.bucket);
  request.SetKey(context.key);
  request.SetPartNumber(partNumber);
  request.SetUploadId(uploadId);
  request.SetContentMD5(contentMd5);
  request.SetContentLength(fileData.length());

  auto stream = std::make_shared<std::stringstream>(fileData.data());
  request.SetBody(stream);

  auto outcome = getS3Client()->UploadPart(request);
  if (!outcome.IsSuccess()) {
    return handleAwsError("上传分块", outcome.GetError(), callback);
  }

  // 保存ETag
  std::string eTag = outcome.GetResult().GetETag();
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    Upload::mContexts[uploadId].etags[partNumber] = eTag;
  }

  Json::Value data;
  data["etag"] = eTag;
  data["part_number"] = partNumber;
  sendJsonResponse(callback, "success", 1, data);
}

void Upload::complete(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!validateJsonParams(json, callback)) {
    return;
  }

  std::string uploadId = (*json)["upload_id"].asString();
  Json::Value parts = (*json)["parts"];

  if (uploadId.empty()) {
    return sendJsonResponse(callback, "缺少上传ID", 0);
  }

  // 获取上传上下文
  UploadContext context;
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    auto it = Upload::mContexts.find(uploadId);
    if (it == Upload::mContexts.end()) {
      return sendJsonResponse(callback, "无效的上传ID", 0);
    }
    context = it->second;
  }

  // 构建完成请求
  Aws::S3::Model::CompletedMultipartUpload completedMultipartUpload;

  if (parts.isArray()) {
    for (const auto &part : parts) {
      Aws::S3::Model::CompletedPart completedPart;
      completedPart.SetETag(part["etag"].asString());
      completedPart.SetPartNumber(part["part_number"].asInt());
      completedMultipartUpload.AddParts(completedPart);
    }
  }

  auto request = Aws::S3::Model::CompleteMultipartUploadRequest();
  request.SetBucket(context.bucket);
  request.SetKey(context.key);
  request.SetUploadId(uploadId);
  request.SetMultipartUpload(completedMultipartUpload);

  auto outcome = getS3Client()->CompleteMultipartUpload(request);
  if (!outcome.IsSuccess()) {
    return handleAwsError("完成上传", outcome.GetError(), callback);
  }

  // 清理上传上下文
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    Upload::mContexts.erase(uploadId);
  }

  Json::Value data;
  data["location"] = outcome.GetResult().GetLocation();
  sendJsonResponse(callback, "success", 1, data);
}

void Upload::uploading(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!validateJsonParams(json, callback)) {
    return;
  }

  std::string upload_id = (*json)["upload_id"].asString();
  int part_number = (*json)["part_number"].asInt();
  std::string content = (*json)["content"].asString();

  if (upload_id.empty()) {
    return sendJsonResponse(callback, "缺少上传ID", 0);
  }

  // 检查上传上下文
  UploadContext context;
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    auto it = Upload::mContexts.find(upload_id);
    if (it == Upload::mContexts.end()) {
      return sendJsonResponse(callback, "无效的上传ID", 0);
    }
    context = it->second;
  }

  // 解码Base64内容
  std::string decoded_content;
  try {
    decoded_content = utils::Crypto::base64Decode(content);
  } catch (const std::exception &e) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, true,
                                      "Base64解码失败: {} - {}:{}", e.what(),
                                      __FILE__, __LINE__);

    return sendJsonResponse(callback, "内容解码失败", 0);
  }

  // 上传部分
  auto request = Aws::S3::Model::UploadPartRequest();
  request.SetBucket(context.bucket);
  request.SetKey(context.key);
  request.SetPartNumber(part_number);
  request.SetUploadId(context.uploadId);
  request.SetContentLength(decoded_content.length());

  // 创建流
  auto stream = std::make_shared<std::stringstream>(decoded_content);
  request.SetBody(stream);

  auto uploadPartOutcome = getS3Client()->UploadPart(request);
  if (!uploadPartOutcome.IsSuccess()) {
    return handleAwsError("上传部分", uploadPartOutcome.GetError(), callback);
  }

  // 保存ETag
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    Upload::mContexts[upload_id].etags[part_number] =
        uploadPartOutcome.GetResult().GetETag();
  }

  Json::Value data;
  data["etag"] = uploadPartOutcome.GetResult().GetETag();
  sendJsonResponse(callback, "success", 1, data);
}

void Upload::uploaded(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!validateJsonParams(json, callback)) {
    return;
  }

  std::string upload_id = (*json)["upload_id"].asString();
  Json::Value parts = (*json)["parts"];

  if (upload_id.empty()) {
    return sendJsonResponse(callback, "缺少上传ID", 0);
  }

  // 检查上传上下文
  UploadContext context;
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    auto it = Upload::mContexts.find(upload_id);
    if (it == Upload::mContexts.end()) {
      return sendJsonResponse(callback, "无效的上传ID", 0);
    }
    context = it->second;
  }

  // 构建完成上传请求
  auto request = Aws::S3::Model::CompleteMultipartUploadRequest();
  request.SetBucket(context.bucket);
  request.SetKey(context.key);
  request.SetUploadId(context.uploadId);

  // 添加所有部分
  Aws::S3::Model::CompletedMultipartUpload completedMultipartUpload;
  for (const auto &part : parts) {
    Aws::S3::Model::CompletedPart completedPart;
    completedPart.SetETag(part["etag"].asString());
    completedPart.SetPartNumber(part["part_number"].asInt());
    completedMultipartUpload.AddParts(completedPart);
  }
  request.SetMultipartUpload(completedMultipartUpload);

  auto completeUploadOutcome = getS3Client()->CompleteMultipartUpload(request);
  if (!completeUploadOutcome.IsSuccess()) {
    return handleAwsError("完成上传", completeUploadOutcome.GetError(),
                          callback);
  }

  // 清理上传上下文
  {
    std::lock_guard<std::mutex> lock(Upload::mContextMutex);
    Upload::mContexts.erase(upload_id);
  }

  Json::Value data;
  data["url"] = fmt::format(
      "{}/{}/{}", app().getCustomConfig()["storage"]["s3"]["url"].asString(),
      context.bucket, context.key);
  sendJsonResponse(callback, "success", 1, data);
}
} // namespace common