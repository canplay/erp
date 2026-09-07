#ifndef ARC4RANDOM_H_INCLUDED
#define ARC4RANDOM_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <random>


namespace common {

namespace detail {
// 线程局部随机数生成器
inline std::mt19937 &get_random_generator() {
  thread_local static std::mt19937 gen(std::random_device{}());
  return gen;
}
} // namespace detail

/**
 * @brief 使用随机字节填充缓冲区。
 *
 * 该函数使用现代C++的随机数生成器生成高质量的随机字节，
 * 并将这些字节存储到指定的缓冲区中。
 * 每个字节的取值范围为 0 到 255。
 *
 * @param buf 指向要填充的缓冲区的指针。必须非空，并且至少分配了 `nbytes`
 * 字节的空间。
 * @param nbytes 要生成并存储的随机字节数。
 */
inline void arc4random_buf(void *buf, size_t nbytes) {
  auto &gen = detail::get_random_generator();
  std::uniform_int_distribution<int> dis(0, 255);

  for (size_t n = 0; n < nbytes; ++n) {
    static_cast<std::uint8_t *>(buf)[n] = static_cast<std::uint8_t>(dis(gen));
  }
}

/**
 * @brief 生成一个随机数。
 *
 * 该函数使用现代C++的随机数生成器生成一个32位随机数。
 *
 * @return 生成的32位随机数。
 */
inline std::uint32_t arc4random(void) {
  auto &gen = detail::get_random_generator();
  std::uniform_int_distribution<std::uint32_t> dis;
  return dis(gen);
}

/**
 * @brief 生成指定范围内的随机数。
 *
 * 该函数使用现代C++的随机数生成器生成指定范围内的随机数。
 *
 * @param upper_bound 随机数的上界（不包含）。
 * @return 生成的随机数，范围在 [0, upper_bound)。
 */
inline std::uint32_t arc4random_uniform(std::uint32_t upper_bound) {
  if (upper_bound <= 1)
    return 0;

  auto &gen = detail::get_random_generator();
  std::uniform_int_distribution<std::uint32_t> dis(0, upper_bound - 1);
  return dis(gen);
}

/**
 * @brief 初始化随机数生成器。
 *
 * 该函数使用当前时间作为种子来初始化随机数生成器。
 * 在现代实现中，此函数可以为空，因为我们在首次使用时已经初始化。
 * 保留此函数以保持API兼容性。
 */
inline void arc4random_init(void) {
  // 现代实现中不需要显式初始化
  // 线程局部存储会在首次访问时自动初始化
}

} // namespace common

#endif // ARC4RANDOM_H_INCLUDED