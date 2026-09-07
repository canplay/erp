#include "utils/jwt.h"
#include "logger.h"
#include <chrono>
#include <drogon/drogon.h>
#include <fstream>
#include <json/json.h>
#include <jwt-cpp/jwt.h>
#include <memory>
#include <openssl/core_names.h>


namespace common::utils {

std::string Jwt::generate(
    const std::string &issuer, const std::string &audience,
    const bool extension,
    const std::map<std::string, jwt::traits::kazuho_picojson::value_type>
        &claims) {
  try {
    // 创建JWT token
    auto token =
        jwt::create().set_issuer(issuer).set_audience(audience).set_issued_at(
            std::chrono::system_clock::now());

    // 如果启用扩展，则设置较长有效期
    if (extension) {
      auto now = std::chrono::system_clock::now();
      auto expireTime = now + std::chrono::hours(24 * 30); // 30天
      token.set_expires_at(expireTime);
    }

    // 添加声明
    for (const auto &[key, value] : claims) {
      token.set_payload_claim(key, jwt::claim(value));
    }

    // 获取配置中的密钥
    auto &config = drogon::app().getCustomConfig();
    if (!config.isMember("auth") || !config["auth"].isMember("jwt") ||
        !config["auth"]["jwt"].isMember("key")) {
      Logger::getInstance().log(Logger::LOGLEVEL::error, true,
                                "JWT配置缺失: auth.jwt.key - {}:{}", __FILE__,
                                __LINE__);
      throw std::runtime_error("JWT配置缺失: auth.jwt.key");
    }

    std::string key = config["auth"]["jwt"]["key"].asString();
    if (key.empty()) {
      Logger::getInstance().log(Logger::LOGLEVEL::error, true,
                                "JWT密钥为空 - {}:{}", __FILE__, __LINE__);
      throw std::runtime_error("JWT密钥为空");
    }

    // 设置签名算法和密钥并生成token
    std::string jwt_token = token.sign(jwt::algorithm::hs256{key});
    return jwt_token;
  } catch (const std::exception &e) {
    Logger::getInstance().log(Logger::LOGLEVEL::error, true,
                              "JWT生成失败: {} - {}:{}", e.what(), __FILE__,
                              __LINE__);
    throw;
  }
}

bool Jwt::verify(const jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt,
                 const std::string &issuer, const std::string &audience) {
  try {
    // 获取配置中的密钥
    auto &config = drogon::app().getCustomConfig();
    if (!config.isMember("auth") || !config["auth"].isMember("jwt") ||
        !config["auth"]["jwt"].isMember("key")) {
      Logger::getInstance().log(Logger::LOGLEVEL::error, true,
                                "JWT配置缺失: auth.jwt.key - {}:{}", __FILE__,
                                __LINE__);
      throw std::runtime_error("JWT配置缺失: auth.jwt.key");
    }

    std::string key = config["auth"]["jwt"]["key"].asString();
    if (key.empty()) {
      Logger::getInstance().log(Logger::LOGLEVEL::error, true,
                                "JWT密钥为空 - {}:{}", __FILE__, __LINE__);
      throw std::runtime_error("JWT密钥为空");
    }

    // 验证JWT token
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{key})
                        .with_issuer(issuer)
                        .with_audience(audience);

    verifier.verify(jwt);

    return true;
  } catch (const jwt::error::signature_verification_exception &e) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "JWT签名验证失败: {} - {}:{}", e.what(), __FILE__,
                              __LINE__);
    return false;
  } catch (const jwt::error::token_verification_exception &e) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "JWT已过期或无效: {} - {}:{}", e.what(), __FILE__,
                              __LINE__);
    return false;
  } catch (const std::exception &e) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "JWT验证失败: {} - {}:{}", e.what(), __FILE__,
                              __LINE__);
    return false;
  }
}

std::string Jwt::toPem(const Json::Value &jwk) {
  const auto decode_b64url = [](const std::string &s) {
    return jwt::base::decode<jwt::alphabet::base64url>(s);
  };

  const auto get_ossl_error = []() {
    char buf[256];
    ERR_error_string_n(
        ERR_get_error_all(nullptr, nullptr, nullptr, nullptr, nullptr), buf,
        sizeof(buf));
    return std::string(buf);
  };

  const std::string x = decode_b64url(jwk["x"].asString());
  const std::string y = decode_b64url(jwk["y"].asString());

  using EVP_PKEY_CTX_ptr =
      std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr),
                       EVP_PKEY_CTX_free);
  if (!ctx) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "创建EVP上下文失败: {} - {}:{}", get_ossl_error(),
                              __FILE__, __LINE__);
    return "";
  }

  std::vector<unsigned char> pubkey;
  pubkey.reserve(1 + 2 * 48);
  pubkey.push_back(0x04);
  pubkey.insert(pubkey.end(), x.begin(), x.end());
  pubkey.insert(pubkey.end(), y.begin(), y.end());

  OSSL_PARAM params[] = {
      OSSL_PARAM_utf8_string(
          OSSL_PKEY_PARAM_GROUP_NAME,
          const_cast<char *>(static_cast<const char *>("secp384r1")), 0),
      OSSL_PARAM_octet_string(OSSL_PKEY_PARAM_PUB_KEY, pubkey.data(),
                              pubkey.size()),
      OSSL_PARAM_utf8_string(
          OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
          const_cast<char *>(static_cast<const char *>("uncompressed")), 0),
      OSSL_PARAM_END};

  if (EVP_PKEY_fromdata_init(ctx.get()) <= 0) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "初始化EVP密钥失败: {} - {}:{}", get_ossl_error(),
                              __FILE__, __LINE__);
    return "";
  }

  EVP_PKEY *pkey = nullptr;
  if (EVP_PKEY_fromdata(ctx.get(), &pkey, EVP_PKEY_PUBLIC_KEY, params) <= 0) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "创建EVP密钥失败: {} - {}:{}", get_ossl_error(),
                              __FILE__, __LINE__);
    return "";
  }

  std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pkey_ptr(pkey,
                                                               EVP_PKEY_free);

  auto bio =
      std::unique_ptr<BIO, decltype(&BIO_free)>(BIO_new(BIO_s_mem()), BIO_free);
  if (!bio) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "创建BIO失败: {} - {}:{}", get_ossl_error(),
                              __FILE__, __LINE__);
    return "";
  }

  if (!PEM_write_bio_PUBKEY(bio.get(), pkey)) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "写入PEM失败: {} - {}:{}", get_ossl_error(),
                              __FILE__, __LINE__);
    return "";
  }

  char *data = nullptr;
  const long len = BIO_get_mem_data(bio.get(), &data);
  if (len <= 0) {
    Logger::getInstance().log(Logger::LOGLEVEL::info, false,
                              "获取PEM数据失败: {} - {}:{}", get_ossl_error(),
                              __FILE__, __LINE__);
    return "";
  }

  return std::string(data, len);
}

std::string Jwt::fromPem(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    Logger::getInstance().log(Logger::LOGLEVEL::error, true,
                              "无法打开PEM文件: {} - {}:{}", path, __FILE__,
                              __LINE__);
    return "";
  }

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  return content;
}

} // namespace common::utils