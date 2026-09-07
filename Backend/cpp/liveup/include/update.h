#pragma once

#include <fmt/format.h>
#include <json/json.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>

///
/// @brief 自动更新
///
class __declspec(dllexport) Update {
public:
  bool update();

private:
  int versionCompare(std::string v1, std::string v2);

  bool updateBak();

  bool execCopy();
};
