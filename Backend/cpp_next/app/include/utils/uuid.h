#pragma once

#include <random>
#include <string>

namespace common::utils {
/**
 * @brief UUID工具类
 */
class Uuid {
public:
  /**
   * @brief 生成 UUID。
   * @return 返回生成的 UUID 字符串。
   */
  [[nodiscard]] static std::string uuid();

  /**
   * @brief 生成不含 [-] 符号的 UUID。
   * @return 返回生成的简化 UUID 字符串。
   */
  [[nodiscard]] static std::string uuid_simple();

  /**
   * @brief 生成UUID版本5（基于名称的UUID）
   * @param name_space 命名空间UUID
   * @param name 名称
   * @return 返回生成的UUID字符串
   */
  [[nodiscard]] static std::string uuid_v5(const std::string &name_space,
                                           const std::string &name);

private:
  static thread_local std::mt19937 rng;
};
} // namespace common::utils