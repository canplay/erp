#pragma once

#include <cstdint>
#include <memory>
#include <openssl/evp.h>
#include <string>

namespace common::utils {
/**
 * @brief 加密和编码工具类
 */
class Crypto {
public:
  /**
   * @brief 使用 SHA-256 算法对字符串进行加密。
   * @param data 原始字符串。
   * @return 返回加密后的字符串。
   */
  static std::string sha256(const std::string &data);

  /**
   * @brief 使用 SHA-256 算法对文件内容进行哈希计算
   * @param filename 文件路径
   * @return 返回文件内容的SHA256哈希值（十六进制字符串）
   */
  static std::string sha256FromFile(const std::string &filename);

  /**
   * @brief 使用 MD5 算法对字符串进行加密。
   * @param data 原始字符串。
   * @return 返回加密后的字符串。
   */
  static std::string md5(const std::string &data);

  /**
   * @brief 对字符串进行 Base64 编码。
   * @param data 原始字符串。
   * @return 返回编码后的字符串。
   */
  static std::string base64Encode(const std::string &data);

  /**
   * @brief 对 Base64 编码的字符串进行解码。
   * @param data 编码后的字符串。
   * @return 返回解码后的原始字符串。
   */
  static std::string base64Decode(const std::string &data);

  /**
   * @brief 使用 BCrypt 算法对字符串进行加密。
   * @param data 原始字符串。
   * @param rounds 加密轮次，默认为 10。
   * @return 返回加密后的字符串。
   */
  static std::string bcryptGenerate(const std::string &data,
                                    unsigned int rounds = 10);

  /**
   * @brief 验证 BCrypt 加密字符串。
   * @param data 原始字符串。
   * @param hash 加密后的字符串。
   * @return 如果验证成功返回 true，否则返回 false。
   */
  static bool bcryptValidate(const std::string &data, const std::string &hash);

  /**
   * @brief 将字节值转换为小写十六进制字符。
   * @param value 字节值。
   * @return 返回对应的小写十六进制字符。
   */
  static char toHexLower(std::uint8_t value);

  /**
   * @brief 将字节值转换为大写十六进制字符。
   * @param value 字节值。
   * @return 返回对应的大写十六进制字符。
   */
  static char toHexUpper(std::uint8_t value);

  /**
   * @brief 将数据编码为十六进制字符串。
   * @param data 数据指针。
   * @param len 数据长度。
   * @param uppercase 是否使用大写，默认为 false。
   * @return 返回编码后的十六进制字符串。
   */
  static std::string hexEncode(const std::uint8_t *data, size_t len,
                               bool uppercase = false);

private:
  struct EVP_PKEY_Deleter {
    void operator()(EVP_PKEY *p) { EVP_PKEY_free(p); }
  };
  using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;

  static constexpr char kHexLower[] = "0123456789abcdef";
  static constexpr char kHexUpper[] = "0123456789ABCDEF";
};

/**
 * @brief SM3 哈希算法工具类
 */
class SM3 {
public:
  /**
   * @brief 对数据进行 SM3 哈希。
   * @param data 数据指针。
   * @param len 数据长度。
   * @param digest 输出的哈希值缓冲区（至少32字节）。
   */
  static void hash(const unsigned char *data, size_t len,
                   unsigned char *digest);

  /**
   * @brief 对字符串进行 SM3 哈希。
   * @param str 输入字符串。
   * @return 返回哈希后的十六进制字符串。
   */
  static std::string hash(const std::string &str);
};

/**
 * @brief SM4 加密算法工具类
 */
class SM4 {

public:
  static constexpr size_t BLOCK_SIZE = 16;
  static constexpr size_t KEY_SIZE = 16;

  /**
   * @brief 加密数据。
   * @param plaintext 明文数据指针。
   * @param len 明文数据长度。
   * @param key 密钥指针（至少16字节）。
   * @param ciphertext 输出的密文缓冲区。
   */
  static void encrypt(const unsigned char *plaintext, size_t len,
                      const unsigned char *key, unsigned char *ciphertext);

  /**
   * @brief 解密数据。
   * @param ciphertext 密文数据指针。
   * @param len 密文数据长度（必须是16的倍数）。
   * @param key 密钥指针（至少16字节）。
   * @param plaintext 输出的明文缓冲区。
   */
  static void decrypt(const unsigned char *ciphertext, size_t len,
                      const unsigned char *key, unsigned char *plaintext);

  /**
   * @brief 加密字符串。
   * @param plaintext 明文字符串。
   * @param key 密钥字符串（至少16字节）。
   * @return 返回加密后的十六进制字符串。
   */
  static std::string encrypt(const std::string &plaintext,
                             const std::string &key);

  /**
   * @brief 解密十六进制字符串。
   * @param ciphertext 密文十六进制字符串。
   * @param key 密钥字符串（至少16字节）。
   * @return 返回解密后的明文字符串。
   */
  static std::string decrypt(const std::string &ciphertext,
                             const std::string &key);
};

/**
 * @brief SM2 公钥算法工具类
 */
class SM2 {
public:
  /**
   * @brief 生成 SM2 密钥对。
   * @param private_key 输出的私钥缓冲区（32字节）。
   * @param public_key 输出的公钥缓冲区（65字节）。
   */
  static void generateKeyPair(unsigned char *private_key,
                              unsigned char *public_key);

  /**
   * @brief 对数据进行 SM2 签名。
   * @param data 数据指针。
   * @param len 数据长度。
   * @param private_key 私钥指针（32字节）。
   * @param signature 输出的签名缓冲区（64字节）。
   */
  static void sign(const unsigned char *data, size_t len,
                   const unsigned char *private_key, unsigned char *signature);

  /**
   * @brief 验证 SM2 签名。
   * @param data 数据指针。
   * @param len 数据长度。
   * @param public_key 公钥指针（65字节）。
   * @param signature 签名指针（64字节）。
   * @return 验证成功返回 true，否则返回 false。
   */
  static bool verify(const unsigned char *data, size_t len,
                     const unsigned char *public_key,
                     const unsigned char *signature);

  /**
   * @brief 使用 SM2 公钥加密数据。
   * @param plaintext 明文数据指针。
   * @param len 明文数据长度。
   * @param public_key 公钥指针（65字节）。
   * @param ciphertext 输出的密文缓冲区。
   * @return 返回密文长度。
   */
  static size_t encrypt(const unsigned char *plaintext, size_t len,
                        const unsigned char *public_key,
                        unsigned char *ciphertext);

  /**
   * @brief 使用 SM2 私钥解密数据。
   * @param ciphertext 密文数据指针。
   * @param len 密文数据长度。
   * @param private_key 私钥指针（32字节）。
   * @param plaintext 输出的明文缓冲区。
   * @return 返回明文长度。
   */
  static size_t decrypt(const unsigned char *ciphertext, size_t len,
                        const unsigned char *private_key,
                        unsigned char *plaintext);
};
} // namespace common::utils