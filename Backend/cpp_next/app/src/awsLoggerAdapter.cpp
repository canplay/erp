#include "awsLoggerAdapter.h"
#include <cstdarg>

namespace common {

/**
 * @brief 记录格式化日志
 * @param logLevel 日志级别
 * @param tag 日志标签
 * @param formatStr 格式化字符串
 * @param ... 可变参数
 */
void AwsLoggerAdapter::Log(Aws::Utils::Logging::LogLevel logLevel,
                           const char *tag, const char *formatStr, ...) {
  va_list args;
  va_start(args, formatStr);
  vaLog(logLevel, tag, formatStr, args);
  va_end(args);
}

/**
 * @brief 记录格式化日志（va_list版本）
 * @param logLevel 日志级别
 * @param tag 日志标签
 * @param formatStr 格式化字符串
 * @param args 可变参数列表
 */
void AwsLoggerAdapter::vaLog(Aws::Utils::Logging::LogLevel logLevel,
                             const char *tag, const char *formatStr,
                             va_list args) {
  // 使用动态缓冲区避免固定大小限制
  constexpr size_t kInitialBufferSize = 2048;
  std::vector<char> buffer(kInitialBufferSize);

  // 创建args的副本，因为vsprintf可能消耗va_list
  va_list args_copy;
  va_copy(args_copy, args);

  // 尝试在初始缓冲区大小内格式化
  int needed = vsnprintf(buffer.data(), buffer.size(), formatStr, args_copy);
  va_end(args_copy);

  // 如果初始缓冲区不够大，重新分配并再次尝试
  if (needed < 0) {
    // 格式化错误
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, true,
                                      "[AWS] 日志格式化错误 - 标签: {} - {}:{}",
                                      tag ? tag : "Unknown", __FILE__,
                                      __LINE__);
    return;
  }

  if (static_cast<size_t>(needed) >= buffer.size()) {
    // 扩大缓冲区并重试
    buffer.resize(needed + 1);
    va_list args_copy2;
    va_copy(args_copy2, args);
    needed = vsnprintf(buffer.data(), buffer.size(), formatStr, args_copy2);
    va_end(args_copy2);

    if (needed < 0) {
      common::Logger::getInstance().log(
          common::Logger::LOGLEVEL::error, true,
          "[AWS] 日志格式化错误 - 标签: {} - {}:{}", tag ? tag : "Unknown",
          __FILE__, __LINE__);
      return;
    }
  }

  // 转换日志级别并记录
  common::Logger::LOGLEVEL level = ConvertToLoggerLevel(logLevel);
  common::Logger::getInstance().log(level, true, "[AWS-{}] {}",
                                    tag ? tag : "Unknown",
                                    std::string(buffer.data(), needed));
}

/**
 * @brief 记录流式日志
 * @param logLevel 日志级别
 * @param tag 日志标签
 * @param message 日志消息
 */
void AwsLoggerAdapter::LogStream(Aws::Utils::Logging::LogLevel logLevel,
                                 const char *tag,
                                 const Aws::OStringStream &message) {
  common::Logger::LOGLEVEL level = ConvertToLoggerLevel(logLevel);
  common::Logger::getInstance().log(level, true, "[AWS-{}] {}",
                                    tag ? tag : "Unknown", message.str());
}

/**
 * @brief 刷新日志缓冲区（未实现）
 */
void AwsLoggerAdapter::Flush() {
  // 由于我们直接将日志输出到统一日志系统，这里不需要特殊处理
  // 但为了接口完整性，我们调用底层日志系统的flush方法
  // 注意：spdlog默认会在适当的时候自动刷新，这里显式调用以确保关键日志被记录
}

} // namespace common