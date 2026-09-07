#ifndef _NODE_BLF_H_
#define _NODE_BLF_H_

#include <cstdint>
#include <sys/types.h>


/* Solaris compatibility */
#ifdef __sun
#define u_int8_t std::uint8_t
#define u_int16_t std::uint16_t
#define u_int32_t std::uint32_t
#define u_int64_t std::uint64_t
#endif

#ifdef _WIN32
#define u_int8_t std::uint8_t
#define u_int16_t std::uint16_t
#define u_int32_t std::uint32_t
#define u_int64_t std::uint64_t
#endif

/* Windows ssize_t compatibility */
#if defined(_WIN32) || defined(_WIN64)
#if defined(_WIN64)
typedef __int64 LONG_PTR;
#else
typedef long LONG_PTR;
#endif
typedef LONG_PTR SSIZE_T;
typedef SSIZE_T ssize_t;
#endif

/* z/OS compatibility */
#ifdef __MVS__
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
typedef unsigned long long u_int64_t;
#endif

#define BCRYPT_VERSION '2'
#define BCRYPT_MAXSALT 16   /* Precomputation is just so nice */
#define BCRYPT_BLOCKS 6     /* Ciphertext blocks */
#define BCRYPT_MINROUNDS 16 /* we have log2(rounds) in salt */

/* Schneier specifies a maximum key length of 56 bytes.
 * This ensures that every key bit affects every cipher
 * bit.  However, the subkeys can hold up to 72 bytes.
 * Warning: For normal blowfish encryption only 56 bytes
 * of the key affect all cipherbits.
 */

#define BLF_N 16                          /* Number of Subkeys */
#define BLF_MAXKEYLEN ((BLF_N - 2) * 4)   /* 448 bits */
#define BLF_MAXUTILIZED ((BLF_N + 2) * 4) /* 576 bits */

#define _PASSWORD_LEN 128 /* max length, not counting NUL */
#define _SALT_LEN 32      /* max length */

namespace common { /* Blowfish context */

/**
 * @struct BlowfishContext
 * @brief Blowfish 加密上下文结构体。
 * 包含 S 盒和子密钥数组，用于存储 Blowfish 算法的状态。
 */
typedef struct BlowfishContext {
  u_int32_t S[4][256];    ///< S 盒，用于非线性变换。
  u_int32_t P[BLF_N + 2]; ///< 子密钥数组，用于加密和解密。
} blf_ctx;

/**
 * @brief Blowfish 加密函数。
 * 使用给定的 Blowfish 上下文对两个 32 位整数进行加密。
 * @param ctx Blowfish 上下文指针。
 * @param xl 输入的左半部分 32 位整数。
 * @param xr 输入的右半部分 32 位整数。
 */
void Blowfish_encipher(blf_ctx *, u_int32_t *, u_int32_t *);

/**
 * @brief Blowfish 解密函数。
 * 使用给定的 Blowfish 上下文对两个 32 位整数进行解密。
 * @param ctx Blowfish 上下文指针。
 * @param xl 输入的左半部分 32 位整数。
 * @param xr 输入的右半部分 32 位整数。
 */
void Blowfish_decipher(blf_ctx *, u_int32_t *, u_int32_t *);

/**
 * @brief 初始化 Blowfish 状态。
 * 初始化 Blowfish 上下文的 S 盒和子密钥数组为默认值。
 * @param ctx Blowfish 上下文指针。
 */
void Blowfish_initstate(blf_ctx *);

/**
 * @brief 扩展 Blowfish 状态（初始密钥扩展）。
 * 使用初始密钥扩展 Blowfish 上下文的状态。
 * @param ctx Blowfish 上下文指针。
 * @param key 密钥数据指针。
 * @param keylen 密钥长度（以字节为单位）。
 */
void Blowfish_expand0state(blf_ctx *, const u_int8_t *, u_int16_t);

/**
 * @brief 扩展 Blowfish 状态（完整密钥扩展）。
 * 使用盐值和密钥扩展 Blowfish 上下文的状态。
 * @param ctx Blowfish 上下文指针。
 * @param key 密钥数据指针。
 * @param keylen 密钥长度（以字节为单位）。
 * @param salt 盐值数据指针。
 * @param saltlen 盐值长度（以字节为单位）。
 */
void Blowfish_expandstate(blf_ctx *, const u_int8_t *, u_int16_t,
                          const u_int8_t *, u_int16_t);

/* Standard Blowfish */

void blf_key(blf_ctx *, const u_int8_t *, u_int16_t);
void blf_enc(blf_ctx *, u_int32_t *, u_int16_t);
void blf_dec(blf_ctx *, u_int32_t *, u_int16_t);

void blf_ecb_encrypt(blf_ctx *, u_int8_t *, u_int32_t);
void blf_ecb_decrypt(blf_ctx *, u_int8_t *, u_int32_t);

void blf_cbc_encrypt(blf_ctx *, u_int8_t *, u_int8_t *, u_int32_t);
void blf_cbc_decrypt(blf_ctx *, u_int8_t *, u_int8_t *, u_int32_t);

/**
 * @brief 将字节流转换为 32 位整数。
 * 从字节流中提取一个 32 位整数，并更新偏移量。
 * @param in 输入的字节流指针。
 * @param len 字节流长度。
 * @param off 当前偏移量指针。
 * @return 转换后的 32 位整数。
 */
u_int32_t Blowfish_stream2word(const u_int8_t *, u_int16_t, u_int16_t *);

/* bcrypt functions*/

/**
 * @brief 生成 bcrypt 盐值。
 * 根据指定的参数生成 bcrypt 盐值字符串。
 * @param prefix 盐值前缀字符。
 * @param count 迭代次数的指数部分。
 * @param salt 盐值数据指针。
 * @param output 输出的盐值字符串。
 */
void bcrypt_gensalt(char, u_int8_t, u_int8_t *, char *);

/**
 * @brief bcrypt 加密函数。
 * 使用 bcrypt 算法对输入的密钥进行加密。
 * @param key 输入的密钥指针。
 * @param key_len 密钥长度。
 * @param salt 盐值字符串。
 * @param encrypted 输出的加密字符串。
 */
void node_bcrypt(const char *, size_t key_len, const char *, char *);

/**
 * @brief 编码盐值。
 * 将盐值编码为标准格式的字符串。
 * @param output 输出的编码盐值字符串。
 * @param csalt 盐值数据指针。
 * @param prefix 盐值前缀字符。
 * @param clen 盐值长度。
 * @param logr 迭代次数的指数部分。
 */
void encode_salt(char *, u_int8_t *, char, u_int16_t, u_int8_t);

/**
 * @brief 获取 bcrypt 迭代次数。
 * 从 bcrypt 盐值字符串中提取迭代次数的指数部分。
 * @param setting 盐值字符串。
 * @return 迭代次数的指数部分。
 */
u_int32_t bcrypt_get_rounds(const char *);

} // namespace common

#endif