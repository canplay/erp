#pragma once

#include <string>
#include <cstdint>

namespace common::utils {
class Time {
public:
  /**
   * @brief 获取当前时间的格式化字符串。
   * @return 返回格式化的时间字符串 "YYYY-MM-DD HH:MM:SS"。
   */
  [[nodiscard]] static std::string now();
  
  /**
   * @brief 获取当前时间的ISO8601格式字符串。
   * @return 返回ISO8601格式的时间字符串 "YYYY-MM-DDTHH:MM:SSZ"。
   */
  [[nodiscard]] static std::string iso8601();
  
  /**
   * @brief 获取当前时间的毫秒时间戳字符串。
   * @return 返回毫秒时间戳字符串。
   */
  [[nodiscard]] static std::string timestamp();
  
  /**
   * @brief 获取当前时间的Unix时间戳。
   * @return 返回Unix时间戳（秒）。
   */
  [[nodiscard]] static std::uint64_t unix_timestamp();
};
} // namespace common::utils