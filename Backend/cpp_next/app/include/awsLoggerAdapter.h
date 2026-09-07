#pragma once

#include "logger.h"
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/LogSystemInterface.h>

namespace common {
/**
 * @class AwsLoggerAdapter
 * @brief AWS SDK 日志系统的适配器类
 *
 * 该类实现了 AWS SDK 的 LogSystemInterface 接口，将 AWS
 * 的日志重定向到系统统一的日志处理模块
 */
class AwsLoggerAdapter : public Aws::Utils::Logging::LogSystemInterface {
public:
  /**
   * @brief 构造函数
   * @param level 日志级别，默认为 trace
   */
  explicit AwsLoggerAdapter(
      common::Logger::LOGLEVEL level = common::Logger::LOGLEVEL::trace);

  /**
   * @brief 获取当前日志级别
   * @return AWS 日志级别
   */
  Aws::Utils::Logging::LogLevel GetLogLevel() const override;

  /**
   * @brief 记录格式化日志
   * @param logLevel 日志级别
   * @param tag 日志标签
   * @param formatStr 格式化字符串
   * @param ... 可变参数
   */
  void Log(Aws::Utils::Logging::LogLevel logLevel, const char *tag,
           const char *formatStr, ...) override;

  /**
   * @brief 记录格式化日志（va_list版本）
   * @param logLevel 日志级别
   * @param tag 日志标签
   * @param formatStr 格式化字符串
   * @param args 可变参数列表
   */
  void vaLog(Aws::Utils::Logging::LogLevel logLevel, const char *tag,
             const char *formatStr, va_list args) override;

  /**
   * @brief 记录流式日志
   * @param logLevel 日志级别
   * @param tag 日志标签
   * @param message 日志消息
   */
  void LogStream(Aws::Utils::Logging::LogLevel logLevel, const char *tag,
                 const Aws::OStringStream &messageStream) override;

  /**
   * @brief 刷新日志缓冲区（未实现）
   */
  void Flush() override;

private:
  common::Logger::LOGLEVEL m_level;

  /**
   * @brief 将AWS日志级别转换为自定义日志级别
   * @param level AWS日志级别
   * @return 自定义日志级别
   */
  [[nodiscard]] common::Logger::LOGLEVEL
  ConvertToLoggerLevel(Aws::Utils::Logging::LogLevel level) const noexcept {
    switch (level) {
    case Aws::Utils::Logging::LogLevel::Off:
      return common::Logger::LOGLEVEL::off;
    case Aws::Utils::Logging::LogLevel::Fatal:
      return common::Logger::LOGLEVEL::fatal;
    case Aws::Utils::Logging::LogLevel::Error:
      return common::Logger::LOGLEVEL::error;
    case Aws::Utils::Logging::LogLevel::Warn:
      return common::Logger::LOGLEVEL::warn;
    case Aws::Utils::Logging::LogLevel::Info:
      return common::Logger::LOGLEVEL::info;
    case Aws::Utils::Logging::LogLevel::Debug:
      return common::Logger::LOGLEVEL::debug;
    default:
      return common::Logger::LOGLEVEL::trace;
    }
  }

  /**
   * @brief 将自定义日志级别转换为AWS日志级别
   * @param level 自定义日志级别
   * @return AWS日志级别
   */
  [[nodiscard]] Aws::Utils::Logging::LogLevel
  ConvertToAwsLevel(common::Logger::LOGLEVEL level) const noexcept {
    switch (level) {
    case common::Logger::LOGLEVEL::off:
      return Aws::Utils::Logging::LogLevel::Off;
    case common::Logger::LOGLEVEL::fatal:
      return Aws::Utils::Logging::LogLevel::Fatal;
    case common::Logger::LOGLEVEL::error:
      return Aws::Utils::Logging::LogLevel::Error;
    case common::Logger::LOGLEVEL::warn:
      return Aws::Utils::Logging::LogLevel::Warn;
    case common::Logger::LOGLEVEL::info:
      return Aws::Utils::Logging::LogLevel::Info;
    case common::Logger::LOGLEVEL::debug:
      return Aws::Utils::Logging::LogLevel::Debug;
    default:
      return Aws::Utils::Logging::LogLevel::Trace;
    }
  }
};

/**
 * @brief 构造函数
 * @param level 日志级别，默认为 trace
 */
inline AwsLoggerAdapter::AwsLoggerAdapter(common::Logger::LOGLEVEL level)
    : m_level(level) {}

/**
 * @brief 获取当前日志级别
 * @return AWS 日志级别
 */
inline Aws::Utils::Logging::LogLevel AwsLoggerAdapter::GetLogLevel() const {
  return ConvertToAwsLevel(m_level);
}

} // namespace common