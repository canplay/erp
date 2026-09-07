#include "utils/time.h"

#include <fmt/chrono.h>
#include <chrono>

namespace common::utils {

namespace {
// 获取当前时间点的辅助函数，避免重复代码
inline auto get_current_time_point() {
    return std::chrono::system_clock::now();
}

// 获取time_t格式的时间，避免重复代码
inline std::time_t get_time_t(auto time_point) {
    return std::chrono::system_clock::to_time_t(time_point);
}
} // namespace

std::string Time::now() {
    auto now = get_current_time_point();
    auto time = get_time_t(now);
    return fmt::format("{:%Y-%m-%d %H:%M:%S}", *std::localtime(&time));
}

std::string Time::iso8601() {
    auto now = get_current_time_point();
    return fmt::format("{:%Y-%m-%dT%H:%M:%SZ}", std::chrono::floor<std::chrono::seconds>(now));
}

std::string Time::timestamp() {
    auto now = get_current_time_point();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return std::to_string(ms.count());
}

std::uint64_t Time::unix_timestamp() {
    auto now = get_current_time_point();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

} // namespace common::utils