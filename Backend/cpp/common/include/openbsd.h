#ifndef ARC4RANDOM_H_INCLUDED
#define ARC4RANDOM_H_INCLUDED

#include <cstddef>
#include <cstdlib> /* srand, rand */
#include <cstring>
#include <ctime>

namespace common {

/**
 * @brief 使用随机字节填充缓冲区。
 *
 * 该函数通过调用 `rand()` 函数生成随机字节，并将这些字节存储到指定的缓冲区中。
 * 每个字节的取值范围为 0 到 255。
 *
 * @param buf 指向要填充的缓冲区的指针。必须非空，并且至少分配了 `nbytes`
 * 字节的空间。
 * @param nbytes 要生成并存储的随机字节数。
 */
inline void arc4random_buf(void *buf, size_t nbytes) {
  for (size_t n = 0; n < nbytes; ++n)
    ((char *)(buf))[n] = rand() % 256;
}

/**
 * @brief 初始化随机数生成器。
 *
 * 该函数使用当前时间作为种子来初始化随机数生成器。
 * 在调用 `arc4random_buf`
 * 之前，应先调用此函数以确保每次运行时生成不同的随机序列。
 */
inline void arc4random_init(void) { srand((unsigned int)time(NULL)); }

} // namespace common

#endif // ARC4RANDOM_H_INCLUDED