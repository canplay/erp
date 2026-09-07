#pragma once

#include <fmt/format.h>
#include <json/value.h>
#include <jwt-cpp/jwt.h>
#include <sentry.h>
#include <spdlog/spdlog.h>

namespace common {

/**
 * @class utility
 * @brief 工具类，提供一系列静态方法用于处理字符串、加密、日志、时间等通用功能。
 */
class utility {
public:
  /**
   * @enum LOGLEVEL
   * @brief 日志级别枚举。
   */
  enum LOGLEVEL { info = 0, warn, error, fatal, debug, trace };

public:
  /**
   * @brief 生成指定范围内的随机整数。
   * @param min 最小值（包含）。
   * @param max 最大值（包含）。
   * @return 返回生成的随机整数。
   */
  static int random(int min, int max);

  /**
   * @brief 将字符串转换为小写。
   * @param data 输入字符串。
   * @return 返回转换后的小写字符串。
   */
  static std::string toLower(const std::string &data);

  /**
   * @brief 将字符串转换为大写。
   * @param data 输入字符串。
   * @return 返回转换后的大写字符串。
   */
  static std::string toUpper(const std::string &data);

  /**
   * @brief 按单字符分隔符分割字符串。
   * @param data 输入字符串。
   * @param delimiter 分隔符。
   * @return 返回分割后的字符串列表。
   */
  static std::vector<std::string> split(const std::string &data,
                                        char delimiter);

  /**
   * @brief 按字符串分隔符分割字符串。
   * @param data 输入字符串。
   * @param delimiter 分隔符。
   * @param ignore_space 是否忽略空格。
   * @return 返回分割后的字符串列表。
   */
  static std::vector<std::string> split(const std::string &data,
                                        const std::string &delimiter,
                                        bool ignore_space);

  /**
   * @brief 将 UTF-8 字符串转换为 GBK 字符串。
   * @param data UTF-8 字符串。
   * @return 返回转换后的 GBK 字符串。
   */
  static std::string utf8ToGBK(const std::string &data);

  /**
   * @brief 将 GBK 字符串转换为 UTF-8 字符串。
   * @param data GBK 字符串。
   * @return 返回转换后的 UTF-8 字符串。
   */
  static std::string gbkToUTF8(const std::string &data);

  /**
   * @brief 对 URL 进行编码（类似 encodeURIComponent）。
   * @param data 原始 URL。
   * @return 返回编码后的 URL。
   */
  static std::string urlEncode(const std::string &data);

  /**
   * @brief 对 URL 进行解码（类似 decodeURIComponent）。
   * @param data 编码后的 URL。
   * @return 返回解码后的原始 URL。
   */
  static std::string urlDecode(const std::string &data);

  /**
   * @brief 将 JSON 对象转换为字符串。
   * @param json JSON 对象。
   * @return 返回转换后的字符串。
   */
  static std::string jsonToString(const Json::Value &json);

  /**
   * @brief 生成 UUID。
   * @return 返回生成的 UUID 字符串。
   */
  static std::string uuid();

  /**
   * @brief 生成不含 [-] 符号的 UUID。
   * @return 返回生成的简化 UUID 字符串。
   */
  static std::string uuidSimple();

  /**
   * @brief 使用 SHA-256 算法对字符串进行加密。
   * @param data 原始字符串。
   * @return 返回加密后的字符串。
   */
  static std::string sha256(const std::string &data);

  /**
   * @brief 使用 MD5 算法对字符串进行加密。
   * @param data 原始字符串。
   * @return 返回加密后的字符串。
   */
  static std::string md5(const std::string &data);

  /**
   * @brief 对字符串进行 Base64 编码。
   * @param data 原始字符串。
   * @return 返回编码后的字符串。
   */
  static std::string base64Encode(const std::string &data);

  /**
   * @brief 对 Base64 编码的字符串进行解码。
   * @param data 编码后的字符串。
   * @return 返回解码后的原始字符串。
   */
  static std::string base64Decode(const std::string &data);

  /**
   * @brief 使用 BCrypt 算法对字符串进行加密。
   * @param data 原始字符串。
   * @param rounds 加密轮次，默认为 10。
   * @return 返回加密后的字符串。
   */
  static std::string bcryptGenerate(const std::string &data,
                                    unsigned int rounds = 10);

  /**
   * @brief 验证 BCrypt 加密字符串。
   * @param data 原始字符串。
   * @param hash 加密后的字符串。
   * @return 如果验证成功返回 true，否则返回 false。
   */
  static bool bcryptValidate(const std::string &data, const std::string &hash);

  /**
   * @brief 初始化 CrashPad 日志系统。
   * @param name 日志名称。
   * @param files 日志文件列表。
   */
  static void initLogger(const std::string &dir, const std::string &name);

  /**
   * @brief 记录日志信息。
   * @param msg 日志消息。
   * @param level 日志级别，默认为 info。
   * @param sentry 是否发送到 Sentry，默认为 false。
   */
  template <typename... Args>
  static void log(LOGLEVEL level, bool sentry, const char *fmt,
                  Args &&...args) {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&now_time_t);

    char time_str[24];
    std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::string level_str;
    switch (level) {
    case warn:
      level_str = "warn";
    case error:
      level_str = "error";
    case fatal:
      level_str = "fatal";
    case debug:
      level_str = "debug";
    default:
      level_str = "info";
    }

    std::string log_prefix =
        fmt::format("[{}.{:03d}] [{}]", time_str, ms.count(), level_str);
    std::string formatted_msg =
        fmt::vformat(fmt, fmt::make_format_args(args...));
    formatted_msg = log_prefix + " " + formatted_msg;

    switch (level) {
    case warn:
      spdlog::warn(formatted_msg);
      break;
    case error:
      spdlog::error(formatted_msg);
      break;
    case fatal:
      spdlog::critical(formatted_msg);
      break;
    case debug:
      spdlog::debug(formatted_msg);
      break;
    default:
      spdlog::info(formatted_msg);
      break;
    }

    if (sentry) {
      sentry_value_t event = sentry_value_new_event();
      switch (level) {
      case warn:
        event = sentry_value_new_message_event(SENTRY_LEVEL_WARNING, NULL,
                                               formatted_msg.c_str());
        break;
      case error:
        event = sentry_value_new_message_event(SENTRY_LEVEL_ERROR, NULL,
                                               formatted_msg.c_str());
        break;
      case fatal:
        event = sentry_value_new_message_event(SENTRY_LEVEL_FATAL, NULL,
                                               formatted_msg.c_str());
        break;
      default:
        event = sentry_value_new_message_event(SENTRY_LEVEL_INFO, NULL,
                                               formatted_msg.c_str());
        break;
      }
      sentry_capture_event(event);
    }
  }

  /**
   * @brief 关闭日志系统。
   */
  static void closeLogger();

  /**
   * @brief 发送 WebSocket 通知。
   * @param topic 主题。
   * @param subscriber 订阅者。
   * @param type 消息类型。
   * @param msg 消息内容。
   */
  static void wsNotify(const std::string &topic, const std::string &subscriber,
                       const std::string &type, const std::string &msg);

  /**
   * @brief 获取当前时间字符串。
   * @return 返回当前时间的字符串表示。
   */
  static std::string timeNow();

  /**
   * @brief 生成 JWT（JSON Web Token）。
   * @param issuer 签发者。
   * @param audience 接收者。
   * @param extension 是否启用扩展。
   * @param claims 自定义声明。
   * @return 返回生成的 JWT 字符串。
   */
  static std::string jwtGenerate(
      const std::string &issuer, const std::string &audience,
      const bool extension,
      const std::map<std::string, jwt::traits::kazuho_picojson::value_type>
          &claims);

  /**
   * @brief 验证 JWT 的有效性。
   * @param jwt 解码后的 JWT。
   * @param issuer 签发者。
   * @param audience 接收者。
   * @return 如果验证成功返回 true，否则返回 false。
   */
  static bool
  jwtVerify(const jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt,
            const std::string &issuer, const std::string &audience);

  /**
   * @brief 将 JWK 转换为 PEM 格式。
   * @param jwk JWK 对象。
   * @return 返回转换后的 PEM 字符串。
   */
  static std::string jwkToPem(const Json::Value &jwk);

  static std::string readPemFile(const std::string &path);

  /**
   * @brief 将字节值转换为小写十六进制字符。
   * @param value 字节值。
   * @return 返回对应的小写十六进制字符。
   */
  static char toHexLower(uint8_t value);

  /**
   * @brief 将字节值转换为大写十六进制字符。
   * @param value 字节值。
   * @return 返回对应的大写十六进制字符。
   */
  static char toHexUpper(uint8_t value);

  /**
   * @brief 将字节数组编码为十六进制字符串。
   * @param data 字节数组指针。
   * @param len 字节数组长度。
   * @param uppercase 是否使用大写，默认为 false。
   * @return 返回编码后的十六进制字符串。
   */
  static std::string hexEncode(const uint8_t *data, size_t len,
                               bool uppercase = false);
};
} // namespace common