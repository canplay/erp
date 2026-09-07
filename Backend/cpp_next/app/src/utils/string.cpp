#include "utils/string.h"

#include <algorithm>
#include <numeric>
#include <json/writer.h>
#include <unicode/ucnv.h>
#include <unicode/utypes.h>

namespace common::utils {
std::string String::toLower(const std::string &data) {
  std::string result = data;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

std::string String::toUpper(const std::string &data) {
  std::string result = data;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

std::vector<std::string> String::split(const std::string &data,
                                       char delimiter) {
  std::vector<std::string> tokens;
  if (data.empty()) {
    tokens.emplace_back("");
    return tokens;
  }
  
  size_t start = 0;
  size_t end = data.find(delimiter);

  while (end != std::string::npos) {
    tokens.emplace_back(data.substr(start, end - start));
    start = end + 1;
    end = data.find(delimiter, start);
  }

  tokens.emplace_back(data.substr(start));
  return tokens;
}

std::vector<std::string> String::split(const std::string &data,
                                       const std::string &delimiter,
                                       bool ignore_space) {
  std::vector<std::string> tokens;
  if (data.empty()) {
    tokens.emplace_back("");
    return tokens;
  }
  
  if (delimiter.empty()) {
    tokens.emplace_back(data);
    return tokens;
  }
  
  size_t start = 0;
  size_t end = data.find(delimiter);

  while (end != std::string::npos) {
    std::string token = data.substr(start, end - start);
    if (!ignore_space || !token.empty()) {
      tokens.emplace_back(std::move(token));
    }
    start = end + delimiter.length();
    end = data.find(delimiter, start);
  }

  std::string token = data.substr(start);
  if (!ignore_space || !token.empty()) {
    tokens.emplace_back(std::move(token));
  }
  
  return tokens;
}

std::string String::utf8ToGbk(const std::string &data) {
  if (data.empty()) {
    return data;
  }

  UErrorCode status = U_ZERO_ERROR;
  auto converter = std::unique_ptr<UConverter, decltype(&ucnv_close)>(
      ucnv_open("GBK", &status), &ucnv_close);
  
  if (U_FAILURE(status)) {
    return data;
  }

  // 计算所需缓冲区大小
  int32_t target_len = ucnv_fromAlgorithmic(converter.get(), UConverterType::UCNV_UTF8,
                                           nullptr, 0, data.c_str(), 
                                           static_cast<int32_t>(data.length()), &status);
  
  if (status != U_BUFFER_OVERFLOW_ERROR) {
    return data;
  }
  
  status = U_ZERO_ERROR;
  std::string result(target_len, '\0');
  ucnv_fromAlgorithmic(converter.get(), UConverterType::UCNV_UTF8,
                      &result[0], target_len, data.c_str(),
                      static_cast<int32_t>(data.length()), &status);
  
  if (U_FAILURE(status)) {
    return data;
  }
  
  return result;
}

std::string String::gbkToUtf8(const std::string &data) {
  if (data.empty()) {
    return data;
  }

  UErrorCode status = U_ZERO_ERROR;
  auto converter = std::unique_ptr<UConverter, decltype(&ucnv_close)>(
      ucnv_open("UTF-8", &status), &ucnv_close);
  
  if (U_FAILURE(status)) {
    return data;
  }

  // 计算所需缓冲区大小
  int32_t target_len = ucnv_toAlgorithmic(UConverterType::UCNV_UTF8, converter.get(),
                                         nullptr, 0, data.c_str(), 
                                         static_cast<int32_t>(data.length()), &status);
  
  if (status != U_BUFFER_OVERFLOW_ERROR) {
    return data;
  }
  
  status = U_ZERO_ERROR;
  std::string result(target_len, '\0');
  ucnv_toAlgorithmic(UConverterType::UCNV_UTF8, converter.get(),
                     &result[0], target_len, data.c_str(),
                     static_cast<int32_t>(data.length()), &status);
  
  if (U_FAILURE(status)) {
    return data;
  }
  
  return result;
}

std::string String::trim(const std::string &data) {
  if (data.empty()) {
    return data;
  }
  
  // 查找第一个非空白字符
  size_t start = data.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos) {
    return ""; // 全是空白字符
  }
  
  // 查找最后一个非空白字符
  size_t end = data.find_last_not_of(" \t\n\r\f\v");
  
  // 返回修剪后的子字符串
  return data.substr(start, end - start + 1);
}

std::string String::trimLeft(const std::string &data) {
  if (data.empty()) {
    return data;
  }
  
  size_t start = data.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos) {
    return "";
  }
  
  return data.substr(start);
}

std::string String::trimRight(const std::string &data) {
  if (data.empty()) {
    return data;
  }
  
  size_t end = data.find_last_not_of(" \t\n\r\f\v");
  if (end == std::string::npos) {
    return "";
  }
  
  return data.substr(0, end + 1);
}

bool String::startsWith(const std::string &data, const std::string &prefix) {
  if (prefix.empty()) {
    return true;
  }
  
  if (data.length() < prefix.length()) {
    return false;
  }
  
  return data.compare(0, prefix.length(), prefix) == 0;
}

bool String::endsWith(const std::string &data, const std::string &suffix) {
  if (suffix.empty()) {
    return true;
  }
  
  if (data.length() < suffix.length()) {
    return false;
  }
  
  return data.compare(data.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string String::replace(const std::string &data, 
                           const std::string &from, 
                           const std::string &to) {
  if (from.empty() || data.empty()) {
    return data;
  }
  
  std::string result = data;
  size_t pos = 0;
  
  while ((pos = result.find(from, pos)) != std::string::npos) {
    result.replace(pos, from.length(), to);
    pos += to.length();
  }
  
  return result;
}

std::string String::join(const std::vector<std::string> &tokens, 
                        const std::string &delimiter) {
  if (tokens.empty()) {
    return "";
  }
  
  if (tokens.size() == 1) {
    return tokens[0];
  }
  
  std::string result;
  result.reserve(tokens.size() * delimiter.length() + 
                 std::accumulate(tokens.begin(), tokens.end(), size_t{0},
                                [](size_t sum, const std::string& s) { return sum + s.length(); }));
  
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (i > 0) {
      result += delimiter;
    }
    result += tokens[i];
  }
  
  return result;
}

} // namespace common::utils