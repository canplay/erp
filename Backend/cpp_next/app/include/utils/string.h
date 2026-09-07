#pragma once

#include <json/value.h>
#include <string>
#include <vector>

namespace common::utils {
/**
 * @brief 字符串处理工具类
 */
class String {
public:
  /**
   * @brief 将字符串转换为小写。
   * @param data 输入字符串。
   * @return 返回转换后的小写字符串。
   */
  [[nodiscard]] static std::string toLower(const std::string &data);

  /**
   * @brief 将字符串转换为大写。
   * @param data 输入字符串。
   * @return 返回转换后的大写字符串。
   */
  [[nodiscard]] static std::string toUpper(const std::string &data);

  /**
   * @brief 按单字符分隔符分割字符串。
   * @param data 输入字符串。
   * @param delimiter 分隔符。
   * @return 返回分割后的字符串列表。
   */
  [[nodiscard]] static std::vector<std::string> split(const std::string &data,
                                                      char delimiter);

  /**
   * @brief 按字符串分隔符分割字符串。
   * @param data 输入字符串。
   * @param delimiter 分隔符。
   * @param ignore_space 是否忽略空格。
   * @return 返回分割后的字符串列表。
   */
  [[nodiscard]] static std::vector<std::string>
  split(const std::string &data, const std::string &delimiter,
        bool ignore_space = true);

  /**
   * @brief 将 UTF-8 字符串转换为 GBK 字符串。
   * @param data UTF-8 字符串。
   * @return 返回转换后的 GBK 字符串。
   */
  [[nodiscard]] static std::string utf8ToGbk(const std::string &data);

  /**
   * @brief 将 GBK 字符串转换为 UTF-8 字符串。
   * @param data GBK 字符串。
   * @return 返回转换后的 UTF-8 字符串。
   */
  [[nodiscard]] static std::string gbkToUtf8(const std::string &data);

  /**
   * @brief 去除字符串两端的空白字符。
   * @param data 输入字符串。
   * @return 返回去除两端空白字符后的字符串。
   */
  [[nodiscard]] static std::string trim(const std::string &data);

  /**
   * @brief 去除字符串左端的空白字符。
   * @param data 输入字符串。
   * @return 返回去除左端空白字符后的字符串。
   */
  [[nodiscard]] static std::string trimLeft(const std::string &data);

  /**
   * @brief 去除字符串右端的空白字符。
   * @param data 输入字符串。
   * @return 返回去除右端空白字符后的字符串。
   */
  [[nodiscard]] static std::string trimRight(const std::string &data);

  /**
   * @brief 检查字符串是否以指定前缀开头。
   * @param data 输入字符串。
   * @param prefix 前缀字符串。
   * @return 如果字符串以指定前缀开头返回true，否则返回false。
   */
  [[nodiscard]] static bool startsWith(const std::string &data,
                                       const std::string &prefix);

  /**
   * @brief 检查字符串是否以指定后缀结尾。
   * @param data 输入字符串。
   * @param suffix 后缀字符串。
   * @return 如果字符串以指定后缀结尾返回true，否则返回false。
   */
  [[nodiscard]] static bool endsWith(const std::string &data,
                                     const std::string &suffix);

  /**
   * @brief 替换字符串中的子字符串。
   * @param data 输入字符串。
   * @param from 要被替换的子字符串。
   * @param to 替换后的子字符串。
   * @return 返回替换后的字符串。
   */
  [[nodiscard]] static std::string replace(const std::string &data,
                                           const std::string &from,
                                           const std::string &to);

  /**
   * @brief 使用分隔符连接字符串向量。
   * @param tokens 字符串向量。
   * @param delimiter 分隔符。
   * @return 返回连接后的字符串。
   */
  [[nodiscard]] static std::string join(const std::vector<std::string> &tokens,
                                        const std::string &delimiter);
};
} // namespace common::utils