#pragma once

#include "utility.h"
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/LogSystemInterface.h>
#include <fmt/format.h>

class AwsLoggerAdapter : public Aws::Utils::Logging::LogSystemInterface {
public:
  AwsLoggerAdapter(
      common::utility::LOGLEVEL level = common::utility::LOGLEVEL::trace)
      : m_level(level) {}

  Aws::Utils::Logging::LogLevel GetLogLevel() const override {
    return ConvertToAwsLevel(m_level);
  }

  void Log(Aws::Utils::Logging::LogLevel logLevel, const char *tag,
           const char *formatStr, ...) override {
    va_list args;
    va_start(args, formatStr);
    vaLog(logLevel, tag, formatStr, args);
    va_end(args);
  }

  void vaLog(Aws::Utils::Logging::LogLevel logLevel, const char *tag,
             const char *formatStr, va_list args) override {
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), formatStr, args);

    common::utility::log(ConvertFromAwsLevel(logLevel), false, "[AWS] [{}] {}",
                         tag, buffer);
  }

  void LogStream(Aws::Utils::Logging::LogLevel logLevel, const char *tag,
                 const Aws::OStringStream &messageStream) override {}
  void Flush() override {}

private:
  common::utility::LOGLEVEL m_level;

  static Aws::Utils::Logging::LogLevel
  ConvertToAwsLevel(common::utility::LOGLEVEL level) {
    switch (level) {
    case common::utility::LOGLEVEL::fatal:
      return Aws::Utils::Logging::LogLevel::Fatal;
    case common::utility::LOGLEVEL::error:
      return Aws::Utils::Logging::LogLevel::Error;
    case common::utility::LOGLEVEL::warn:
      return Aws::Utils::Logging::LogLevel::Warn;
    case common::utility::LOGLEVEL::info:
      return Aws::Utils::Logging::LogLevel::Info;
    case common::utility::LOGLEVEL::debug:
      return Aws::Utils::Logging::LogLevel::Debug;
    case common::utility::LOGLEVEL::trace:
      return Aws::Utils::Logging::LogLevel::Trace;

    default:
      return Aws::Utils::Logging::LogLevel::Off;
    }
  }

  static common::utility::LOGLEVEL
  ConvertFromAwsLevel(Aws::Utils::Logging::LogLevel level) {
    switch (level) {
    case Aws::Utils::Logging::LogLevel::Fatal:
      return common::utility::LOGLEVEL::fatal;
    case Aws::Utils::Logging::LogLevel::Error:
      return common::utility::LOGLEVEL::error;
    case Aws::Utils::Logging::LogLevel::Warn:
      return common::utility::LOGLEVEL::warn;
    case Aws::Utils::Logging::LogLevel::Info:
      return common::utility::LOGLEVEL::info;
    case Aws::Utils::Logging::LogLevel::Debug:
      return common::utility::LOGLEVEL::debug;
    case Aws::Utils::Logging::LogLevel::Trace:
      return common::utility::LOGLEVEL::trace;
    default:
      return common::utility::LOGLEVEL::info;
    }
  }
};