#include "include/utility.h"
#include "include/awsLog.h"
#include "include/node_blf.h"
#include "include/openbsd.h"
#include "include/uuid.h"
#include "pch.h"

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <aws/core/Aws.h>

#include <unicode/putil.h>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>
#include <unicode/udata.h>
#include <unicode/utypes.h>

#include <openssl/bio.h>
#include <openssl/core.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/param_build.h>
#include <openssl/sha.h>

#include <drogon/WebSocketClient.h>

#define VERSION "2025.03.01"

namespace common {
struct EVP_PKEY_Deleter {
  void operator()(EVP_PKEY *p) { EVP_PKEY_free(p); }
};
using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;

constexpr char kHexLower[] = "0123456789abcdef";
constexpr char kHexUpper[] = "0123456789ABCDEF";

int utility::random(int min, int max) {
  std::random_device seed;
  std::ranlux48 engine(seed());
  std::uniform_int_distribution<> distrib(min, max);
  return distrib(engine);
};

std::string utility::toLower(const std::string &data) {
  std::string s = data;
  transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

std::string utility::toUpper(const std::string &data) {
  std::string s = data;
  transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

std::vector<std::string> utility::split(const std::string &data,
                                        char delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = data.find(delimiter);
  while (end != std::string::npos) {
    tokens.push_back(data.substr(start, end - start));
    start = end + 1;
    end = data.find(delimiter, start);
  }
  tokens.push_back(data.substr(start));
  return tokens;
}

std::vector<std::string> utility::split(const std::string &data,
                                        const std::string &delimiter,
                                        bool ignore_space = true) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = data.find(delimiter);
  while (end != std::string::npos) {
    if (start != end && ignore_space)
      tokens.push_back(data.substr(start, end - start));
    start = end + delimiter.length();
    end = data.find(delimiter, start);
  }
  tokens.push_back(data.substr(start));
  return tokens;
}

std::string utility::utf8ToGBK(const std::string &data) {
  int32_t len = static_cast<int32_t>(data.size() * 4);
  std::shared_ptr<char> buf(new char[len], [](char *p) {
    delete[] p;
    p = nullptr;
  });
  UErrorCode error = U_ZERO_ERROR;
  ucnv_convert("gbk", "utf8", buf.get(), len, data.c_str(),
               static_cast<int>(data.size()), &error);
  std::string result(buf.get());
  return result;
}

std::string utility::gbkToUTF8(const std::string &data) {
  int32_t len = static_cast<int32_t>(data.size() * 4);
  std::shared_ptr<char> buf(new char[len], [](char *p) {
    delete[] p;
    p = nullptr;
  });
  UErrorCode error = U_ZERO_ERROR;
  ucnv_convert("utf8", "gbk", buf.get(), len, data.c_str(),
               static_cast<int>(data.size()), &error);
  std::string result(buf.get());
  return result;
}

unsigned char ToHex(unsigned char x) { return x > 9 ? x + 55 : x + 48; }

unsigned char FromHex(unsigned char x) {
  unsigned char y;
  if (x >= 'A' && x <= 'Z')
    y = x - 'A' + 10;
  else if (x >= 'a' && x <= 'z')
    y = x - 'a' + 10;
  else
    y = x - '0';
  return y;
}

std::string utility::urlEncode(const std::string &data) {
  std::string strTemp = "";
  size_t len = data.length();
  for (size_t i = 0; i < len; i++) {
    if (isalnum((unsigned char)data[i]) || (data[i] == '-') ||
        (data[i] == '_') || (data[i] == '.') || (data[i] == '~'))
      strTemp += data[i];
    else if (data[i] == ' ')
      strTemp += "+";
    else {
      strTemp += '%';
      strTemp += ToHex((unsigned char)data[i] >> 4);
      strTemp += ToHex((unsigned char)data[i] % 16);
    }
  }
  return strTemp;
}

std::string utility::urlDecode(const std::string &data) {
  std::string strTemp = "";
  size_t len = data.length();
  for (size_t i = 0; i < len; i++) {
    if (data[i] == '+')
      strTemp += ' ';
    else if (data[i] == '%') {
      unsigned char high = FromHex((unsigned char)data[++i]);
      unsigned char low = FromHex((unsigned char)data[++i]);
      strTemp += high * 16 + low;
    } else
      strTemp += data[i];
  }
  return strTemp;
}

std::string utility::jsonToString(const Json::Value &json) {
  Json::Value def = []() {
    Json::Value def;
    Json::StreamWriterBuilder::setDefaults(&def);
    def["emitUTF8"] = true;
    return def;
  }();

  std::ostringstream stream;
  Json::StreamWriterBuilder stream_builder;
  stream_builder.settings_ = def;
  std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
  writer->write(json, &stream);
  return stream.str();
}

std::string utility::uuid() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  return uuids::to_string(gen());
}

std::string utility::uuidSimple() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  auto str = uuids::to_string(gen());
  std::string::size_type pos = 0;
  while ((pos = str.find("-")) != std::string::npos) {
    str.replace(pos, 1, "");
  }
  return str;
}

std::string utility::sha256(const std::string &data) {
  unsigned char digest[SHA256_DIGEST_LENGTH];

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
  EVP_DigestUpdate(ctx, data.c_str(), data.size());
  EVP_DigestFinal_ex(ctx, digest, nullptr);
  EVP_MD_CTX_free(ctx);

  std::stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
  }
  return ss.str();
}

std::string utility::md5(const std::string &data) {
  unsigned char digest[MD5_DIGEST_LENGTH];

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
  EVP_DigestUpdate(ctx, data.c_str(), data.size());
  EVP_DigestFinal_ex(ctx, digest, nullptr);
  EVP_MD_CTX_free(ctx);

  std::stringstream ss;
  for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
  }
  return ss.str();
}

std::string utility::base64Encode(const std::string &data) {
  EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
  EVP_EncodeInit(ctx);

  int outlen = 0;
  std::vector<unsigned char> encoded(data.size() * 4 / 3 + 4);

  EVP_EncodeUpdate(ctx, encoded.data(), &outlen,
                   reinterpret_cast<const unsigned char *>(data.c_str()),
                   static_cast<int>(data.size()));
  EVP_EncodeFinal(ctx, encoded.data() + outlen, &outlen);

  encoded.resize(outlen);
  EVP_ENCODE_CTX_free(ctx);

  return std::string(reinterpret_cast<char *>(encoded.data()), encoded.size());
}

std::string utility::base64Decode(const std::string &data) {
  EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
  EVP_DecodeInit(ctx);

  int outlen = 0;
  std::vector<unsigned char> decoded(data.size());

  EVP_DecodeUpdate(ctx, decoded.data(), &outlen,
                   reinterpret_cast<const unsigned char *>(data.c_str()),
                   static_cast<int>(data.size()));
  int final_len = EVP_DecodeFinal(ctx, decoded.data() + outlen, &outlen);

  decoded.resize(outlen + final_len);
  EVP_ENCODE_CTX_free(ctx);

  size_t pad = 0;
  for (size_t i = decoded.size() - 1; i >= 0; --i) {
    if (decoded[i] == '=')
      pad++;
    else
      break;
  }
  decoded.resize(decoded.size() - pad);

  return std::string(reinterpret_cast<char *>(decoded.data()), decoded.size());
}

std::string utility::bcryptGenerate(const std::string &data,
                                    unsigned int rounds) {
  char salt[_SALT_LEN];

  unsigned char seed[17]{};
  arc4random_buf(seed, 16);

  bcrypt_gensalt('b', rounds, seed, salt);

  std::string hash(61, '\0');
  node_bcrypt(data.c_str(), data.size(), salt, &hash[0]);
  hash.resize(60);
  return hash;
}

bool utility::bcryptValidate(const std::string &data, const std::string &hash) {
  std::string got(61, '\0');
  node_bcrypt(data.c_str(), data.size(), hash.c_str(), &got[0]);
  got.resize(60);
  return hash == got;
}

void utility::initLogger(const std::string &dir, const std::string &name) {
  if (!std::filesystem::exists(dir))
    std::filesystem::create_directory(dir);

  spdlog::init_thread_pool(8192, 1);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);
  auto out_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      fmt::format("{}/log.log", dir), 1048576 * 10, 5);
  out_sink->set_level(spdlog::level::debug);
  file_sink->set_level(spdlog::level::debug);
  std::vector<spdlog::sink_ptr> sinks{out_sink, file_sink};
  auto logger = std::make_shared<spdlog::async_logger>(
      "", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);
  spdlog::set_default_logger(logger);
  spdlog::set_error_handler([](const std::string &msg) {
    spdlog::error("{} - {}:{}", msg, __FILE__, __LINE__);
  });

  static Aws::SDKOptions awsOptions;
  static std::shared_ptr<AwsLoggerAdapter> awsLogger;

  awsOptions.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
  awsOptions.loggingOptions.logger_create_fn = [] {
    awsLogger = std::make_shared<AwsLoggerAdapter>(utility::LOGLEVEL::trace);
    return awsLogger;
  };

  Aws::InitAPI(awsOptions);

  sentry_options_t *options = sentry_options_new();
  sentry_options_set_dsn(
      options,
      "https://***@example.com/1");
  sentry_options_set_database_path(options, ".sentry-native");
  sentry_options_set_release(options,
                             fmt::format("{}@{}", name, VERSION).c_str());
#ifdef _DEBUG
  sentry_options_set_debug(options, 1);
#endif
  sentry_options_add_attachment(options,
                                fmt::format("{}/log.log", dir).c_str());
  for (const auto &entry : std::filesystem::recursive_directory_iterator(dir)) {
    if (entry.is_regular_file()) {
      sentry_options_add_attachment(options, entry.path().string().c_str());
    }
  }
  sentry_init(options);

  std::set_terminate([]() {
    sentry_value_t event = sentry_value_new_event();
    sentry_value_t exc = sentry_value_new_exception("Fatal", "set_terminate");
    sentry_value_set_stacktrace(exc, NULL, 0);
    event = sentry_value_new_message_event(SENTRY_LEVEL_FATAL, NULL,
                                           "set_terminate");
    sentry_event_add_exception(event, exc);
    sentry_capture_event(event);
    std::abort();
  });

#ifdef _MSC_VER
  set_unexpected([]() {
    sentry_value_t event = sentry_value_new_event();
    sentry_value_t exc = sentry_value_new_exception("Fatal", "set_unexpected");
    sentry_value_set_stacktrace(exc, NULL, 0);
    event = sentry_value_new_message_event(SENTRY_LEVEL_FATAL, NULL,
                                           "set_unexpected");
    sentry_event_add_exception(event, exc);
    sentry_capture_event(event);
    std::abort();
  });
#else
  std::set_unexpected([]() {
    sentry_value_t event = sentry_value_new_event();
    sentry_value_t exc = sentry_value_new_exception("Fatal", "set_unexpected");
    sentry_value_set_stacktrace(exc, NULL, 0);
    event = sentry_value_new_message_event(SENTRY_LEVEL_FATAL, NULL,
                                           "set_unexpected");
    sentry_event_add_exception(event, exc);
    sentry_capture_event(event);
    std::abort();
  });
#endif
}

void utility::closeLogger() {
  Aws::ShutdownAPI(Aws::SDKOptions());
  spdlog::shutdown();
  sentry_close();
}

void utility::wsNotify(const std::string &topic, const std::string &subscriber,
                       const std::string &type, const std::string &msg) {
  try {
    auto ws =
        drogon::WebSocketClient::newWebSocketClient("ws://127.0.0.1:51560");
    auto wsreq = drogon::HttpRequest::newHttpRequest();

    auto url = fmt::format(
        "/api/ws/notify?auth=key&token={}&topic={}",
        drogon::app().getCustomConfig()["auth"]["m2m"]["key"].asString(),
        topic);
    wsreq->setPath(url);

    ws->setMessageHandler([](const std::string &message,
                             const drogon::WebSocketClientPtr &,
                             const drogon::WebSocketMessageType &type) {
      std::string messageType = "Unknown";
      if (type == drogon::WebSocketMessageType::Text)
        messageType = "text";
      else if (type == drogon::WebSocketMessageType::Pong)
        messageType = "pong";
      else if (type == drogon::WebSocketMessageType::Ping)
        messageType = "ping";
      else if (type == drogon::WebSocketMessageType::Binary)
        messageType = "binary";
      else if (type == drogon::WebSocketMessageType::Close)
        messageType = "Close";
    });

    ws->setConnectionClosedHandler(
        [=](const drogon::WebSocketClientPtr &conn) {});

    ws->connectToServer(wsreq, [=](drogon::ReqResult r,
                                   const drogon::HttpResponsePtr &resp,
                                   const drogon::WebSocketClientPtr &conn) {
      if (r != drogon::ReqResult::Ok) {
        log(LOGLEVEL::warn, true, "Failed to establish WebSocket connection!");
        conn->stop();
        return;
      }

      Json::Value info;
      info["topic"] = topic;
      info["subscriber"] = subscriber;
      info["type"] = type;
      info["msg"] = msg;
      conn->getConnection()->send(info.toStyledString());
    });

    ws->getLoop()->runAfter(15, [=]() { ws->stop(); });
  } catch (const std::exception &e) {
    log(LOGLEVEL::error, true, "{} - {}:{}", e.what(), __FILE__, __LINE__);
  }
}

std::string utility::timeNow() {
  auto now = std::chrono::system_clock::now();
  time_t time = std::chrono::system_clock::to_time_t(now);
  auto tm = *std::localtime(&time);
  return fmt::format("{:%Y-%m-%d %H:%M:%S}", tm);
}

std::string utility::jwtGenerate(
    const std::string &issuer, const std::string &audience,
    const bool extension,
    const std::map<std::string, jwt::traits::kazuho_picojson::value_type>
        &claims) {
  const auto now = std::chrono::system_clock::now();
  const auto exp = now + std::chrono::hours{extension ? 30 * 24 : 24};

  auto token = jwt::create()
                   .set_issuer(issuer)
                   .set_audience(audience)
                   .set_issued_at(now)
                   .set_expires_at(exp);

  for (const auto &[key, value] : claims) {
    token.set_payload_claim(key, value);
  }

  return {token.sign(jwt::algorithm::hs256(
      drogon::app().getCustomConfig()["auth"]["jwt"]["key"].asString()))};
}

bool utility::jwtVerify(
    const jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt,
    const std::string &issuer, const std::string &audience) {
  try {
    jwt::verify()
        .with_type("JWT")
        .with_issuer(issuer)
        .with_audience(audience)
        .allow_algorithm(jwt::algorithm::hs256(
            drogon::app().getCustomConfig()["auth"]["jwt"]["key"].asString()))
        .verify(jwt);
    return true;
  } catch (const std::exception &e) {
    utility::log(utility::LOGLEVEL::info, false, "{} - {}:{}", e.what(),
                 __FILE__, __LINE__);
    return false;
  }
}

std::string utility::jwkToPem(const Json::Value &jwk) {
  const auto decode_b64url = [](const std::string &s) {
    return jwt::base::decode<jwt::alphabet::base64url>(s);
  };

  const auto get_ossl_error = []() {
    char buf[256];
    ERR_error_string_n(
        ERR_get_error_all(nullptr, nullptr, nullptr, nullptr, nullptr), buf,
        sizeof(buf));
    return std::string(buf);
  };

  const std::string x = decode_b64url(jwk["x"].asString());
  const std::string y = decode_b64url(jwk["y"].asString());

  using EVP_PKEY_CTX_ptr =
      std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr),
                       EVP_PKEY_CTX_free);
  if (!ctx) {
    utility::log(utility::LOGLEVEL::info, false,
                 "创建EVP上下文失败: {} - {}:{}", get_ossl_error(), __FILE__,
                 __LINE__);
  }

  std::vector<uint8_t> pubkey;
  pubkey.reserve(1 + 2 * 48);
  pubkey.push_back(0x04);
  pubkey.insert(pubkey.end(), x.begin(), x.end());
  pubkey.insert(pubkey.end(), y.begin(), y.end());

  OSSL_PARAM params[] = {
      OSSL_PARAM_utf8_string(
          OSSL_PKEY_PARAM_GROUP_NAME,
          const_cast<char *>(static_cast<const char *>("secp384r1")), 0),
      OSSL_PARAM_octet_string(OSSL_PKEY_PARAM_PUB_KEY, pubkey.data(),
                              pubkey.size()),
      OSSL_PARAM_utf8_string(
          OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
          const_cast<char *>(static_cast<const char *>("uncompressed")), 0),
      OSSL_PARAM_END};

  EVP_PKEY *raw_pkey = nullptr;
  if (EVP_PKEY_fromdata_init(ctx.get()) <= 0 ||
      EVP_PKEY_fromdata(ctx.get(), &raw_pkey, EVP_PKEY_PUBLIC_KEY, params) <=
          0) {
    utility::log(utility::LOGLEVEL::info, false, "创建EVP_PKEY失败: {} - {}:{}",
                 get_ossl_error(), __FILE__, __LINE__);
  }
  EVP_PKEY_ptr pkey(raw_pkey);

  using BIO_ptr = std::unique_ptr<BIO, decltype(&BIO_free)>;
  BIO_ptr bio(BIO_new(BIO_s_mem()), BIO_free);
  if (!bio) {
    utility::log(utility::LOGLEVEL::info, false, "创建BIO失败: {} - {}:{}",
                 get_ossl_error(), __FILE__, __LINE__);
  }

  if (PEM_write_bio_PUBKEY(bio.get(), pkey.get()) != 1) {
    utility::log(utility::LOGLEVEL::info, false, "生成PEM失败: {} - {}:{}",
                 get_ossl_error(), __FILE__, __LINE__);
  }

  char *pem_data;
  const long pem_len = BIO_get_mem_data(bio.get(), &pem_data);
  return std::string(pem_data, pem_len);
}

std::string utility::readPemFile(const std::string &path) {
  std::ifstream keyFile(path);
  if (!keyFile.is_open()) {
    utility::log(utility::LOGLEVEL::info, false, "读取pem文件失败 - {}:{}",
                 __FILE__, __LINE__);
    return "";
  }
  std::stringstream buffer;
  buffer << keyFile.rdbuf();
  return buffer.str();
}

char utility::toHexLower(uint8_t value) { return kHexLower[value]; }

char utility::toHexUpper(uint8_t value) { return kHexUpper[value]; }

std::string utility::hexEncode(const uint8_t *data, size_t len,
                               bool uppercase) {
  std::string hex;
  hex.resize(len * 2);
  for (size_t i = 0; i < len; ++i) {
    hex[i * 2] =
        uppercase ? toHexUpper(data[i] >> 4) : toHexLower(data[i] >> 4);
    hex[i * 2 + 1] =
        uppercase ? toHexUpper(data[i] & 0x0F) : toHexLower(data[i] & 0x0F);
  }
  return hex;
  return hex;
}
} // namespace common
