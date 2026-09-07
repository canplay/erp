#include "include/upload.h"
#include "include/utility.h"
#include "pch.h"

#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/xml/XmlSerializer.h>
#include <aws/s3/model/CompleteMultipartUploadRequest.h>
#include <aws/s3/model/CreateMultipartUploadRequest.h>
#include <aws/s3/model/UploadPartRequest.h>

#include <drogon/MultiPart.h>

namespace common {
std::shared_ptr<Aws::S3::S3Client> Upload::mS3client = nullptr;
std::mutex Upload::mContextMutex;
std::unordered_map<std::string, Upload::UploadContext> Upload::mContexts;

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

void Upload::init(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  Json::Value ret;
  if (!json) {
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  try {
    const std::string bucket = (*json)["bucket"].asString();
    std::string key = (*json)["key"].asString();
    const std::string extension = (*json)["extension"].asString();
    const std::string contentType = (*json)["contentType"].asString();
    const size_t totalSize = (*json)["totalSize"].asInt();
    const size_t chunkSize = 5 * 1024 * 1024;

    Aws::S3::Model::CreateMultipartUploadRequest request;
    request.SetBucket(bucket);
    request.SetKey(key += fmt::format(".{}", extension));
    request.SetContentType(contentType);
    auto outcome = getS3Client()->CreateMultipartUpload(request);

    if (!outcome.IsSuccess()) {
      throw std::runtime_error(outcome.GetError().GetMessage());
    }

    const std::string uploadId = outcome.GetResult().GetUploadId();
    std::lock_guard<std::mutex> lock(mContextMutex);
    mContexts.try_emplace(uploadId, uploadId, bucket, key, totalSize,
                          chunkSize);

    ret["data"] = uploadId;
    ret["message"] = "success";
    ret["status"] = 1;
  } catch (const std::exception &e) {
    ret["message"] = e.what();
    ret["status"] = 0;
  }

  callback(HttpResponse::newHttpJsonResponse(ret));
}

std::vector<unsigned char> hexToBytes(const std::string &hex) {
  std::vector<unsigned char> bytes;
  if (hex.length() % 2 != 0) {
    throw std::runtime_error("Invalid hex string length");
  }
  for (size_t i = 0; i < hex.length(); i += 2) {
    std::string byteStr = hex.substr(i, 2);
    char *end = nullptr;
    unsigned long byte = std::strtoul(byteStr.c_str(), &end, 16);
    if (end != byteStr.c_str() + byteStr.size() || byte > 0xFF) {
      throw std::runtime_error("Invalid hex character: " + byteStr);
    }
    bytes.push_back(static_cast<unsigned char>(byte));
  }
  return bytes;
}

void Upload::upload(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  Json::Value ret;

  try {
    MultiPartParser mpp;
    mpp.parse(req);
    const std::string uploadId = mpp.getParameter<std::string>("uploadId");
    const int partNumber = mpp.getParameter<int>("partNumber");
    const std::string clientMd5 = mpp.getParameter<std::string>("contentMd5");

    std::unique_lock<std::mutex> lock(mContextMutex);
    auto it = mContexts.find(uploadId);
    if (it == mContexts.end()) {
      throw std::runtime_error("Invalid upload ID");
    }
    UploadContext &context = it->second;
    lock.unlock();

    const auto &files = mpp.getFiles();
    if (files.empty()) {
      throw std::runtime_error("No file part found");
    }
    const auto &file = files[0];
    const size_t chunkSize = file.fileLength();

    {
      std::lock_guard<std::mutex> lock(context.mutex);
      if (partNumber != context.parts.size() + 1) {
        throw std::runtime_error("Invalid part sequence");
      }
    }

    std::string hexMd5 = utility::toLower(file.getMd5());
    std::vector<unsigned char> md5Bytes;
    try {
      md5Bytes = hexToBytes(hexMd5);
    } catch (const std::exception &e) {
      throw std::runtime_error("MD5 conversion failed: " +
                               std::string(e.what()));
    }
    Aws::Utils::ByteBuffer buffer(md5Bytes.data(), md5Bytes.size());
    const Aws::String serverMd5 =
        Aws::Utils::HashingUtils::Base64Encode(buffer);

    if (serverMd5 != clientMd5) {
      throw std::runtime_error("MD5 mismatch");
    }

    {
      std::lock_guard<std::mutex> lock(context.mutex);
      context.uploadedBytes += chunkSize;
    }

    if (!context.isLastPart(chunkSize) && chunkSize != context.chunkSize) {
      throw std::runtime_error("Invalid chunk size");
    }

    Aws::S3::Model::UploadPartRequest request;
    request.SetBucket(context.bucket);
    request.SetKey(context.objectKey);
    request.SetUploadId(uploadId);
    request.SetPartNumber(partNumber);
    request.SetContentMD5(serverMd5);
    request.SetContentLength(chunkSize);

    auto stream = Aws::MakeShared<Aws::StringStream>("UploadPartStream");
    stream->write(file.fileData(), chunkSize);
    request.SetBody(stream);

    auto outcome = getS3Client()->UploadPart(request);
    if (!outcome.IsSuccess()) {
      throw std::runtime_error(outcome.GetError().GetMessage());
    }

    {
      std::lock_guard<std::mutex> lock(context.mutex);
      Aws::S3::Model::CompletedPart part;
      part.SetPartNumber(partNumber);
      part.SetETag(outcome.GetResult().GetETag());
      context.parts.push_back(part);
    }

    Json::Value info;
    std::string etag = outcome.GetResult().GetETag();
    if (!etag.empty() && etag.front() == '"' && etag.back() == '"') {
      etag = etag.substr(1, etag.size() - 2);
    }
    info["etag"] = etag;
    info["isLast"] = context.isLastPart(chunkSize);

    ret["data"] = info;
    ret["message"] = "success";
    ret["status"] = 1;
  } catch (const std::exception &e) {
    ret["message"] = e.what();
    ret["status"] = 0;
  }
  callback(HttpResponse::newHttpJsonResponse(ret));
}

void Upload::complete(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  Json::Value ret;
  if (!json) {
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  try {
    const std::string uploadId = (*json)["uploadId"].asString();

    std::unique_lock<std::mutex> lock(mContextMutex);
    auto it = mContexts.find(uploadId);
    if (it == mContexts.end()) {
      throw std::runtime_error("Invalid upload ID");
    }
    UploadContext context = std::move(it->second);
    mContexts.erase(it);
    lock.unlock();

    std::sort(context.parts.begin(), context.parts.end(),
              [](const auto &a, const auto &b) {
                return a.GetPartNumber() < b.GetPartNumber();
              });

    for (size_t i = 0; i < context.parts.size(); ++i) {
      if (context.parts[i].GetPartNumber() != i + 1) {
        throw std::runtime_error(fmt::format("Missing part number: {}", i + 1));
      }
    }

    Aws::S3::Model::CompleteMultipartUploadRequest request;
    request.SetBucket(context.bucket);
    request.SetKey(context.objectKey);
    request.SetUploadId(uploadId);

    Aws::S3::Model::CompletedMultipartUpload completed;
    for (const auto &part : context.parts) {
      completed.AddParts(part);
    }
    request.WithMultipartUpload(completed);

    auto outcome = getS3Client()->CompleteMultipartUpload(request);
    if (!outcome.IsSuccess()) {
      throw std::runtime_error(outcome.GetError().GetMessage());
    }

    ret["data"] = outcome.GetResult().GetLocation();
    ret["message"] = "success";
    ret["status"] = 1;
  } catch (const std::exception &e) {
    ret["message"] = e.what();
    ret["status"] = 0;
  }
  callback(HttpResponse::newHttpJsonResponse(ret));
}
} // namespace common