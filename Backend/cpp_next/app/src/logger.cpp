#include "logger.h"
#include "awsLoggerAdapter.h"

#include <aws/core/Aws.h>
#include <filesystem>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/version.h>

#define VERSION "2025.03.01"

namespace common {
void Logger::init(const std::string &dir, const std::string &name) {
  // 确保日志目录存在
  if (!std::filesystem::exists(dir)) {
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if (ec) {
      // 如果创建目录失败，记录错误但继续初始化
      spdlog::error("无法创建日志目录 '{}': {}", dir, ec.message());
    }
  }

#ifdef _WIN32
  // 在Windows上设置控制台编码为UTF-8以正确显示中文
  SetConsoleOutputCP(CP_UTF8);
#endif

  // 初始化spdlog线程池
  spdlog::init_thread_pool(8192, 1);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);

  // 创建控制台和文件输出目标
  auto out_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      fmt::format("{}/log.log", dir), 1048576 * 10, 5);

  out_sink->set_level(spdlog::level::debug);
  file_sink->set_level(spdlog::level::debug);

  // 组合输出目标
  std::vector<spdlog::sink_ptr> sinks{out_sink, file_sink};
  auto logger = std::make_shared<spdlog::async_logger>(
      name, sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::set_default_logger(logger);
  spdlog::set_error_handler([](const std::string &msg) {
    spdlog::error("{} - {}:{}", msg, __FILE__, __LINE__);
  });

  // 初始化AWS日志适配器
  static Aws::SDKOptions awsOptions;
  static std::shared_ptr<AwsLoggerAdapter> awsLogger;

  awsOptions.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
  awsOptions.loggingOptions.logger_create_fn = [] {
    awsLogger = std::make_shared<AwsLoggerAdapter>(LOGLEVEL::trace);
    return awsLogger;
  };

  Aws::InitAPI(awsOptions);

  log(LOGLEVEL::info, false, "日志系统初始化完成, 版本: {}", VERSION);
}

Logger::~Logger() {
  // 确保所有日志都被刷新
  spdlog::shutdown();
}

} // namespace common