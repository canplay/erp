#pragma once

#include <fmt/format.h>
#include <json/value.h>
#include <jwt-cpp/jwt.h>

#include <map>
#include <string>

namespace common::utils {
/**
 * @brief JWT工具类
 */
class Jwt {
public:
  /**
   * @brief 生成 JWT（JSON Web Token）。
   * @param issuer 签发者。
   * @param audience 接收者。
   * @param extension 是否启用扩展。
   * @param claims 自定义声明。
   * @return 返回生成的 JWT 字符串。
   */
  [[nodiscard]] static std::string
  generate(const std::string &issuer, const std::string &audience,
           const bool extension = false,
           const std::map<std::string, jwt::traits::kazuho_picojson::value_type>
               &claims = {});

  /**
   * @brief 验证 JWT 的有效性。
   * @param jwt 解码后的 JWT。
   * @param issuer 签发者。
   * @param audience 接收者。
   * @return 如果验证成功返回 true，否则返回 false。
   */
  [[nodiscard]] static bool verify(const jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt,
                     const std::string &issuer, const std::string &audience);

  /**
   * @brief 将 JWK 转换为 PEM 格式。
   * @param jwk JWK 对象。
   * @return 返回转换后的 PEM 字符串。
   */
  [[nodiscard]] static std::string toPem(const Json::Value &jwk);

  /**
   * @brief 从PEM文件读取内容
   * @param path PEM文件路径
   * @return 返回PEM文件内容
   */
  [[nodiscard]] static std::string fromPem(const std::string &path);
};

} // namespace common::utils