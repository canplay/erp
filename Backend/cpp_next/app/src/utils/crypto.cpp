#include "utils/crypto.h"
#include "logger.h"
#include "utils/node_blf.h"
#include "utils/openbsd.h"

#include <gmssl/sm2.h>
#include <gmssl/sm3.h>
#include <gmssl/sm4.h>

#include <cstring>
#include <openssl/md5.h>
#include <openssl/sha.h>

namespace common::utils {
// 添加bcrypt_hashpass函数实现
int bcrypt_hashpass(const char *key, const char *salt, char *encrypted,
                    size_t encryptedlen) {
  if (encryptedlen < 128) {
    return -1;
  }
  common::node_bcrypt(key, strlen(key), salt, encrypted);
  return 0;
}

// 添加bcrypt_checkpass函数实现
int bcrypt_checkpass(const char *key, const char *encrypted, char *buffer,
                     size_t bufferSize) {
  char hash[128];
  if (bcrypt_hashpass(key, encrypted, hash, sizeof(hash)) != 0) {
    return -1;
  }

  if (strncmp(hash, encrypted, sizeof(hash)) != 0) {
    return -1;
  }

  return 0;
}

// 添加带rounds参数的bcrypt_hashpass函数实现
int bcrypt_hashpass(const char *key, const char *salt, size_t salt_len,
                    unsigned int rounds, char *encrypted, size_t encryptedlen) {
  if (encryptedlen < 128) {
    return -1;
  }

  // 生成盐值字符串
  char salt_str[32];
  common::bcrypt_gensalt(
      'b', static_cast<std::uint8_t>(rounds),
      reinterpret_cast<std::uint8_t *>(const_cast<char *>(salt)), salt_str);

  // 生成哈希
  common::node_bcrypt(key, strlen(key), salt_str, encrypted);
  return 0;
}

std::string Crypto::sha256(const std::string &data) {
  auto ctx = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>(
      EVP_MD_CTX_new(), &EVP_MD_CTX_free);

  if (!ctx) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "无法创建SHA256上下文");
    return "";
  }

  if (EVP_DigestInit_ex(ctx.get(), EVP_sha256(), nullptr) != 1) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "SHA256初始化失败");
    return "";
  }

  if (EVP_DigestUpdate(ctx.get(), data.c_str(), data.size()) != 1) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "SHA256更新失败");
    return "";
  }

  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len;

  if (EVP_DigestFinal_ex(ctx.get(), hash, &hash_len) != 1) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "SHA256最终化失败");
    return "";
  }

  return hexEncode(hash, hash_len);
}

std::string Crypto::sha256FromFile(const std::string &filename) {
  // 打开文件
  FILE *file = fopen(filename.c_str(), "rb");
  if (!file) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "无法打开文件: {}", filename);
    return "";
  }

  // 创建SHA256上下文
  auto ctx = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>(
      EVP_MD_CTX_new(), &EVP_MD_CTX_free);

  if (!ctx) {
    fclose(file);
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "无法创建SHA256上下文");
    return "";
  }

  // 初始化SHA256上下文
  if (EVP_DigestInit_ex(ctx.get(), EVP_sha256(), nullptr) != 1) {
    fclose(file);
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "SHA256初始化失败");
    return "";
  }

  // 读取文件并更新哈希值
  constexpr size_t BUFFER_SIZE = 8192;
  auto buffer = std::make_unique<unsigned char[]>(BUFFER_SIZE);
  size_t bytes_read;

  while ((bytes_read = fread(buffer.get(), 1, BUFFER_SIZE, file)) > 0) {
    if (EVP_DigestUpdate(ctx.get(), buffer.get(), bytes_read) != 1) {
      fclose(file);
      common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                        "SHA256更新失败");
      return "";
    }
  }

  // 检查是否有读取错误
  if (ferror(file)) {
    fclose(file);
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "读取文件时发生错误: {}", filename);
    return "";
  }

  // 关闭文件
  fclose(file);

  // 完成哈希计算
  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len;

  if (EVP_DigestFinal_ex(ctx.get(), hash, &hash_len) != 1) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "SHA256最终化失败");
    return "";
  }

  return hexEncode(hash, hash_len);
}

std::string Crypto::md5(const std::string &data) {
  auto ctx = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>(
      EVP_MD_CTX_new(), &EVP_MD_CTX_free);

  if (!ctx) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "无法创建MD5上下文");
    return "";
  }

  if (EVP_DigestInit_ex(ctx.get(), EVP_md5(), nullptr) != 1) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "MD5初始化失败");
    return "";
  }

  if (EVP_DigestUpdate(ctx.get(), data.c_str(), data.size()) != 1) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "MD5更新失败");
    return "";
  }

  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len;

  if (EVP_DigestFinal_ex(ctx.get(), hash, &hash_len) != 1) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "MD5最终化失败");
    return "";
  }

  return hexEncode(hash, hash_len);
}

std::string Crypto::base64Encode(const std::string &data) {
  if (data.empty()) {
    return "";
  }

  // 计算所需缓冲区大小
  size_t encoded_len = 4 * ((data.size() + 2) / 3);
  auto buffer = std::make_unique<char[]>(encoded_len + 1);

  // 使用OpenSSL进行Base64编码
  auto ctx = std::unique_ptr<EVP_ENCODE_CTX, decltype(&EVP_ENCODE_CTX_free)>(
      EVP_ENCODE_CTX_new(), &EVP_ENCODE_CTX_free);

  if (!ctx) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "无法创建Base64编码上下文");
    return "";
  }

  int encoded_len_out = 0;
  int encoded_len_chunk = 0;

  EVP_EncodeInit(ctx.get());
  EVP_EncodeUpdate(ctx.get(), reinterpret_cast<unsigned char *>(buffer.get()),
                   &encoded_len_chunk,
                   reinterpret_cast<const unsigned char *>(data.c_str()),
                   static_cast<int>(data.size()));

  encoded_len_out += encoded_len_chunk;

  EVP_EncodeFinal(ctx.get(),
                  reinterpret_cast<unsigned char *>(buffer.get()) +
                      encoded_len_out,
                  &encoded_len_chunk);

  encoded_len_out += encoded_len_chunk;

  buffer[encoded_len_out] = '\0';

  return std::string(buffer.get());
}

std::string Crypto::base64Decode(const std::string &data) {
  if (data.empty()) {
    return "";
  }

  // 计算所需缓冲区大小
  size_t decoded_len = 3 * ((data.size() + 3) / 4);
  auto buffer = std::make_unique<unsigned char[]>(decoded_len);

  // 使用OpenSSL进行Base64解码
  auto ctx = std::unique_ptr<EVP_ENCODE_CTX, decltype(&EVP_ENCODE_CTX_free)>(
      EVP_ENCODE_CTX_new(), &EVP_ENCODE_CTX_free);

  if (!ctx) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "无法创建Base64解码上下文");
    return "";
  }

  int decoded_len_out = 0;
  int decoded_len_chunk = 0;

  EVP_DecodeInit(ctx.get());
  int ret =
      EVP_DecodeUpdate(ctx.get(), buffer.get(), &decoded_len_chunk,
                       reinterpret_cast<const unsigned char *>(data.c_str()),
                       static_cast<int>(data.size()));

  if (ret < 0) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "Base64解码失败");
    return "";
  }

  decoded_len_out += decoded_len_chunk;

  ret = EVP_DecodeFinal(ctx.get(), buffer.get() + decoded_len_out,
                        &decoded_len_chunk);

  if (ret < 0) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "Base64解码最终化失败");
    return "";
  }

  decoded_len_out += decoded_len_chunk;

  return std::string(reinterpret_cast<char *>(buffer.get()), decoded_len_out);
}

std::string Crypto::bcryptGenerate(const std::string &data,
                                   unsigned int rounds) {
  if (rounds < 4 || rounds > 31) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "BCrypt轮次必须在4到31之间");
    return "";
  }

  char salt[16];
  char encrypted[128];

  // 初始化随机数生成器
  arc4random_init();

  // 生成盐值
  for (size_t i = 0; i < sizeof(salt); ++i) {
    salt[i] = static_cast<char>(arc4random() & 0xFF);
  }

  // 生成BCrypt哈希
  if (bcrypt_hashpass(data.c_str(), salt, sizeof(salt), rounds, encrypted,
                      sizeof(encrypted)) != 0) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "BCrypt哈希生成失败");
    return "";
  }

  return std::string(encrypted);
}

bool Crypto::bcryptValidate(const std::string &data, const std::string &hash) {
  char buffer[128];

  // 验证BCrypt哈希
  if (bcrypt_checkpass(data.c_str(), hash.c_str(), buffer, sizeof(buffer)) !=
      0) {
    return false;
  }

  return true;
}

char Crypto::toHexLower(std::uint8_t value) { return kHexLower[value & 0xF]; }

char Crypto::toHexUpper(std::uint8_t value) { return kHexUpper[value & 0xF]; }

std::string Crypto::hexEncode(const std::uint8_t *data, size_t len,
                              bool uppercase) {
  if (!data || len == 0) {
    return "";
  }

  std::string result;
  result.reserve(len * 2);

  const char *hex_chars = uppercase ? kHexUpper : kHexLower;

  for (size_t i = 0; i < len; ++i) {
    result.push_back(hex_chars[(data[i] >> 4) & 0xF]);
    result.push_back(hex_chars[data[i] & 0xF]);
  }

  return result;
}

void SM3::hash(const unsigned char *data, size_t len, unsigned char *digest) {
  if (!data || !digest) {
    return;
  }

  SM3_CTX ctx;
  sm3_init(&ctx);
  sm3_update(&ctx, data, len);
  sm3_finish(&ctx, digest);
}

std::string SM3::hash(const std::string &str) {
  unsigned char digest[SM3_DIGEST_SIZE];
  hash(reinterpret_cast<const unsigned char *>(str.c_str()), str.length(),
       digest);
  return Crypto::hexEncode(digest, SM3_DIGEST_SIZE, false);
}

void SM4::encrypt(const unsigned char *plaintext, size_t len,
                  const unsigned char *key, unsigned char *ciphertext) {
  if (!plaintext || !key || !ciphertext) {
    return;
  }

  SM4_KEY sm4_key;
  sm4_set_encrypt_key(&sm4_key, key);
  sm4_encrypt(&sm4_key, plaintext, ciphertext);
}

void SM4::decrypt(const unsigned char *ciphertext, size_t len,
                  const unsigned char *key, unsigned char *plaintext) {
  if (!ciphertext || !key || !plaintext) {
    return;
  }

  SM4_KEY sm4_key;
  sm4_set_decrypt_key(&sm4_key, key);
  sm4_encrypt(&sm4_key, ciphertext,
              plaintext); // 注意：GmSSL中解密也使用sm4_encrypt函数
}

std::string SM4::encrypt(const std::string &plaintext, const std::string &key) {
  if (plaintext.empty() || key.length() < KEY_SIZE) {
    return "";
  }

  // 计算填充后的长度（SM4要求是16字节的倍数）
  size_t padded_len =
      ((plaintext.length() + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
  auto input = std::make_unique<unsigned char[]>(padded_len);
  auto output = std::make_unique<unsigned char[]>(padded_len);

  // 复制数据并进行PKCS7填充
  memcpy(input.get(), plaintext.c_str(), plaintext.length());
  unsigned char padding_value =
      static_cast<unsigned char>(padded_len - plaintext.length());
  for (size_t i = plaintext.length(); i < padded_len; i++) {
    input[i] = padding_value;
  }

  // 执行加密
  SM4_KEY sm4_key;
  sm4_set_encrypt_key(&sm4_key,
                      reinterpret_cast<const unsigned char *>(key.c_str()));
  for (size_t i = 0; i < padded_len; i += BLOCK_SIZE) {
    sm4_encrypt(&sm4_key, input.get() + i, output.get() + i);
  }

  // 返回十六进制编码的结果
  return Crypto::hexEncode(output.get(), padded_len, false);
}

std::string SM4::decrypt(const std::string &ciphertext,
                         const std::string &key) {
  if (ciphertext.empty() || key.length() < KEY_SIZE) {
    return "";
  }

  // 解码十六进制密文
  // 这里需要实现十六进制解码，暂时假设输入已经是正确的十六进制格式且长度是BLOCK_SIZE的倍数
  size_t len = ciphertext.length() / 2;
  if (len % BLOCK_SIZE != 0) {
    return "";
  }

  auto input = std::make_unique<unsigned char[]>(len);
  auto output = std::make_unique<unsigned char[]>(len);

  // 简化的十六进制解码（实际应该更严格）
  for (size_t i = 0; i < len; i++) {
    char ch1 = ciphertext[2 * i];
    char ch2 = ciphertext[2 * i + 1];
    unsigned char val1 = (ch1 >= '0' && ch1 <= '9')   ? ch1 - '0'
                         : (ch1 >= 'a' && ch1 <= 'f') ? ch1 - 'a' + 10
                         : (ch1 >= 'A' && ch1 <= 'F') ? ch1 - 'A' + 10
                                                      : 0;
    unsigned char val2 = (ch2 >= '0' && ch2 <= '9')   ? ch2 - '0'
                         : (ch2 >= 'a' && ch2 <= 'f') ? ch2 - 'a' + 10
                         : (ch2 >= 'A' && ch2 <= 'F') ? ch2 - 'A' + 10
                                                      : 0;
    input[i] = (val1 << 4) | val2;
  }

  // 执行解密
  SM4_KEY sm4_key;
  sm4_set_decrypt_key(&sm4_key,
                      reinterpret_cast<const unsigned char *>(key.c_str()));
  for (size_t i = 0; i < len; i += BLOCK_SIZE) {
    sm4_encrypt(&sm4_key, input.get() + i,
                output.get() + i); // 注意：GmSSL中解密也使用sm4_encrypt函数
  }

  // 移除PKCS7填充
  unsigned char padding_value = output[len - 1];
  if (padding_value > 0 && padding_value <= BLOCK_SIZE) {
    // 检查填充是否正确
    bool valid_padding = true;
    for (size_t i = len - padding_value; i < len; i++) {
      if (output[i] != padding_value) {
        valid_padding = false;
        break;
      }
    }

    if (valid_padding) {
      return std::string(reinterpret_cast<char *>(output.get()),
                         len - padding_value);
    }
  }

  // 如果没有有效填充，返回整个解密结果
  return std::string(reinterpret_cast<char *>(output.get()), len);
}

void SM2::generateKeyPair(unsigned char *private_key,
                          unsigned char *public_key) {
  if (!private_key || !public_key) {
    return;
  }

  SM2_KEY sm2_key;
  sm2_key_generate(&sm2_key);
  memcpy(private_key, sm2_key.private_key, 32);
  sm2_z256_point_to_bytes(&sm2_key.public_key, public_key);
}

void SM2::sign(const unsigned char *data, size_t len,
               const unsigned char *private_key, unsigned char *signature) {
  if (!data || !private_key || !signature) {
    return;
  }

  SM2_KEY sm2_key;
  memcpy(sm2_key.private_key, private_key, 32);
  // 注意：这里需要从私钥恢复公钥，实际实现可能更复杂
  // 为简化起见，我们假定公钥已知或可从私钥推导

  unsigned char digest[SM3_DIGEST_SIZE];
  SM3_CTX ctx;
  sm3_init(&ctx);
  sm3_update(&ctx, data, len);
  sm3_finish(&ctx, digest);

  SM2_SIGNATURE sm2_sig;
  sm2_do_sign(&sm2_key, digest, &sm2_sig);
  memcpy(signature, sm2_sig.r, 32);
  memcpy(signature + 32, sm2_sig.s, 32);
}

bool SM2::verify(const unsigned char *data, size_t len,
                 const unsigned char *public_key,
                 const unsigned char *signature) {
  if (!data || !public_key || !signature) {
    return false;
  }

  SM2_KEY sm2_key;
  sm2_z256_point_from_bytes(&sm2_key.public_key, public_key);

  unsigned char digest[SM3_DIGEST_SIZE];
  SM3_CTX ctx;
  sm3_init(&ctx);
  sm3_update(&ctx, data, len);
  sm3_finish(&ctx, digest);

  SM2_SIGNATURE sm2_sig;
  memcpy(sm2_sig.r, signature, 32);
  memcpy(sm2_sig.s, signature + 32, 32);

  return sm2_do_verify(&sm2_key, digest, &sm2_sig) == 1;
}

size_t SM2::encrypt(const unsigned char *plaintext, size_t len,
                    const unsigned char *public_key,
                    unsigned char *ciphertext) {
  if (!plaintext || !public_key || !ciphertext) {
    return 0;
  }

  SM2_KEY sm2_key;
  sm2_z256_point_from_bytes(&sm2_key.public_key, public_key);

  size_t ciphertext_len;
  if (sm2_encrypt(&sm2_key, plaintext, len, ciphertext, &ciphertext_len) != 1) {
    return 0;
  }

  return ciphertext_len;
}

size_t SM2::decrypt(const unsigned char *ciphertext, size_t len,
                    const unsigned char *private_key,
                    unsigned char *plaintext) {
  if (!ciphertext || !private_key || !plaintext) {
    return 0;
  }

  SM2_KEY sm2_key;
  memcpy(sm2_key.private_key, private_key, 32);
  // 同样，这里需要从私钥恢复公钥

  size_t plaintext_len;
  if (sm2_decrypt(&sm2_key, ciphertext, len, plaintext, &plaintext_len) != 1) {
    return 0;
  }

  return plaintext_len;
}

} // namespace common::utils