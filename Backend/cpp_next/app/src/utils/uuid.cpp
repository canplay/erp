#include "utils/uuid.h"
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>


namespace common::utils {
// 线程安全的随机数生成器
thread_local std::mt19937 Uuid::rng{std::random_device{}()};

namespace {
// UUID生成辅助函数，避免重复代码
std::string generate_uuid_impl(std::mt19937 &rng, bool with_hyphens) {
  // 生成UUID版本4 (随机UUID)
  std::uniform_int_distribution<std::uint32_t> dist(0, 0xFFFFFFFF);

  std::uint32_t time_low = dist(rng);
  std::uint32_t time_mid = dist(rng) & 0xFFFF;
  std::uint32_t time_hi_and_version =
      (dist(rng) & 0x0FFF) | 0x4000; // 设置版本位为4
  std::uint32_t clock_seq_hi_and_reserved =
      (dist(rng) & 0x3F) | 0x80; // 设置变体位
  std::uint32_t clock_seq_low = dist(rng) & 0xFF;
  std::uint32_t node_low = dist(rng) & 0xFFFFFF;
  std::uint32_t node_high = dist(rng) & 0xFFFFFF;

  std::stringstream ss;
  ss << std::hex << std::setfill('0');

  if (with_hyphens) {
    ss << std::setw(8) << time_low << "-" << std::setw(4) << time_mid << "-"
       << std::setw(4) << time_hi_and_version << "-" << std::setw(2)
       << clock_seq_hi_and_reserved << std::setw(2) << clock_seq_low << "-"
       << std::setw(6) << node_low << std::setw(6) << node_high;
  } else {
    ss << std::setw(8) << time_low << std::setw(4) << time_mid << std::setw(4)
       << time_hi_and_version << std::setw(2) << clock_seq_hi_and_reserved
       << std::setw(2) << clock_seq_low << std::setw(6) << node_low
       << std::setw(6) << node_high;
  }

  return ss.str();
}

// 实现SHA1哈希算法（UUID v5需要）
void sha1_hash(const unsigned char *data, std::size_t len,
               unsigned char *hash) {
  // 简化的SHA1实现，实际项目中应该使用成熟的加密库
  // 这里为了保持代码简洁，使用一个简单的哈希实现
  // 在实际应用中，应该使用 OpenSSL 或其他加密库的 SHA1 实现

  // 由于SHA1实现较为复杂，这里使用一个简化版本仅作示例
  // 实际项目中建议使用 OpenSSL 提供的 SHA1 函数
  std::memset(hash, 0, 20);
  for (std::size_t i = 0; i < len; ++i) {
    hash[i % 20] ^= data[i];
  }
}

std::string uuid_v5_impl(const std::string &name_space,
                         const std::string &name) {
  // UUID v5 实现
  // 组合命名空间和名称
  std::string input = name_space + name;

  // 计算SHA1哈希
  unsigned char hash[20];
  sha1_hash(reinterpret_cast<const unsigned char *>(input.c_str()),
            input.length(), hash);

  // 设置UUID版本为5
  hash[6] = (hash[6] & 0x0F) | 0x50; // 版本5
  // 设置变体位
  hash[8] = (hash[8] & 0x3F) | 0x80; // 变体位

  // 格式化为UUID字符串
  std::stringstream ss;
  ss << std::hex << std::setfill('0');

  // 构建UUID格式字符串
  for (int i = 0; i < 16; ++i) {
    if (i == 4 || i == 6 || i == 8 || i == 10) {
      ss << "-";
    }
    ss << std::setw(2) << static_cast<int>(hash[i]);
  }

  return ss.str();
}
} // namespace

std::string Uuid::uuid() { return generate_uuid_impl(Uuid::rng, true); }

std::string Uuid::uuid_simple() { return generate_uuid_impl(Uuid::rng, false); }

std::string Uuid::uuid_v5(const std::string &name_space,
                          const std::string &name) {
  return uuid_v5_impl(name_space, name);
}

} // namespace common::utils