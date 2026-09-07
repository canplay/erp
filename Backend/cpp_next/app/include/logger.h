#pragma once

#include "utils/singleton.h"

#include <fmt/format.h>
#include <sentry.h>
#include <spdlog/spdlog.h>
#include <string>

namespace common {
/**
 * @brief 日志工具类
 */
class Logger : public common::Singleton<Logger> {
  friend class common::Singleton<Logger>;

public:
  /**
   * @enum LOGLEVEL
   * @brief 日志级别枚举。
   */
  enum class LOGLEVEL { info = 0, warn, error, fatal, debug, trace, off };

public:
  void init(const std::string &dir, const std::string &name);
  ~Logger();

  /**
   * @brief 记录日志信息。
   * @param msg 日志消息。
   * @param level 日志级别，默认为 info。
   * @param sentry 是否发送到 Sentry，默认为 false。
   */
  template <typename... Args>
  void log(LOGLEVEL level, bool sentry, const char *fmt, Args &&...args) {
    std::string formatted_msg =
        fmt::vformat(fmt, fmt::make_format_args(args...));

    switch (level) {
    case LOGLEVEL::warn:
      spdlog::warn(formatted_msg);
      break;
    case LOGLEVEL::error:
      spdlog::error(formatted_msg);
      break;
    case LOGLEVEL::fatal:
      spdlog::critical(formatted_msg);
      break;
    case LOGLEVEL::debug:
      spdlog::debug(formatted_msg);
      break;
    case LOGLEVEL::trace:
      spdlog::trace(formatted_msg);
      break;
    case LOGLEVEL::info:
    default:
      spdlog::info(formatted_msg);
      break;
    }

    if (sentry) {
      sentry_value_t event = sentry_value_new_message_event(
          static_cast<sentry_level_t>(static_cast<int>(level)), nullptr,
          formatted_msg.c_str());
      sentry_capture_event(event);
    }
  }

private:
  Logger() = default;
};

} // namespace common