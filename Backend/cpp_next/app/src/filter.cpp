#include "filter.h"
#include "logger.h"
#include "utils/jwt.h"
#include "utils/string.h"

#include <cpr/cpr.h>
#include <drogon/drogon.h>
#include <jwt-cpp/jwt.h>
#include <openssl/hmac.h>

namespace common {

namespace {
// 辅助函数，用于记录请求日志，消除重复代码
void logRequest(const HttpRequestPtr &req, const std::string &params = "") {
  std::string path_fix = "";

  for (const auto &[key, value] : req->getParameters()) {
    if (path_fix.empty()) {
      path_fix += fmt::format("?{}={}", key, value);
    } else {
      path_fix += fmt::format("&{}={}", key, value);
    }
  }

  common::Logger::getInstance().log(
      common::Logger::LOGLEVEL::info, false, "{} {} [{}] {}{} {}",
      req->getHeader("X-Forwarded-For"), req->getHeader("User-Agent"),
      req->getMethodString(), utils::String::utf8ToGbk(req->getPath()),
      utils::String::utf8ToGbk(path_fix), utils::String::utf8ToGbk(params));
}

// 辅助函数，用于处理OPTIONS请求
bool handleOptionsRequest(const HttpRequestPtr &req,
                          FilterChainCallback &&fccb) {
  if (req->getMethod() == HttpMethod::Options) {
    fccb();
    return true;
  }
  return false;
}

// 辅助函数，用于返回错误响应
void sendErrorResponse(FilterCallback &&fcb, const std::string &message) {
  Json::Value ret;
  ret["data"] = Json::nullValue;
  ret["message"] = message;
  ret["status"] = 0;
  fcb(HttpResponse::newHttpJsonResponse(ret));
}
} // namespace

void LogFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                         FilterChainCallback &&fccb) {
  std::string params = "";
  std::shared_ptr<Json::Value> json = req->getJsonObject();

  if (json) {
    params = (*json).toStyledString();
  }

  logRequest(req, params);

  fccb();
}

void JwtFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                         FilterChainCallback &&fccb) {
  std::string params = "";
  std::shared_ptr<Json::Value> json = req->getJsonObject();

  if (json) {
    params = (*json).toStyledString();
  }

  logRequest(req, params);

  if (handleOptionsRequest(req, std::move(fccb))) {
    return;
  }

  auto token = utils::String::split(req->getHeader("Authorization"), ' ');

  if (token.size() < 2 || token[0] != "Bearer" || token[1].empty()) {
    return sendErrorResponse(std::move(fcb), "Jwt令牌不正确");
  }

  try {
    auto jwt_decode = jwt::decode(token[1]);

    auto verifier =
        jwt::verify()
            .with_issuer(
                app().getCustomConfig()["auth"]["jwt"]["issuer"].asString())
            .with_audience(
                app().getCustomConfig()["auth"]["jwt"]["audience"].asString())
            .expires_at_leeway(
                std::chrono::duration_cast<std::chrono::seconds>(
                    jwt_decode.get_expires_at().time_since_epoch())
                    .count());

    if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() == "hs384") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::hs384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "hs512") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::hs512(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "rs256") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::rs256(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "rs384") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::rs384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "rs512") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::rs512(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es256") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es256(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es384") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es512") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es512(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es256k") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es256k(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ed25519") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ed25519(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ed448") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ed448(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ps256") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ps256(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ps384") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ps384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ps512") {
      const std::string &pemPublicKey = utils::Jwt::fromPem(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ps512(pemPublicKey));
    } else {
      verifier.allow_algorithm(jwt::algorithm::hs256(
          app().getCustomConfig()["auth"]["jwt"]["key"].asString()));
    }

    verifier.verify(jwt_decode);

    for (const auto &[key, value] : jwt_decode.get_payload_json()) {
      req->getAttributes()->insert("jwt_" + key, value);
    }

    fccb();
  } catch (const std::exception &e) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                      "{} - {}:{}", e.what(), __FILE__,
                                      __LINE__);
    sendErrorResponse(std::move(fcb), "Jwt令牌不正确");
  }
}

void KeyFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                         FilterChainCallback &&fccb) {
  auto token = utils::String::split(req->getHeader("Authorization"), ' ');

  try {
    if (token.size() < 2 || token[0] != "Bearer" || token[1].empty()) {
      return sendErrorResponse(std::move(fcb), "Key令牌不正确");
    }

    std::shared_ptr<Json::Value> json = req->getJsonObject();

    auto requestTimePoint = std::chrono::time_point<std::chrono::system_clock,
                                                    std::chrono::milliseconds>{
        std::chrono::milliseconds{(*json)["timestamp"].asInt64()}};
    auto now = std::chrono::system_clock::now();
    auto diff =
        std::chrono::duration_cast<std::chrono::seconds>(now - requestTimePoint)
            .count();
    if (std::abs(diff) > 300) {
      return sendErrorResponse(std::move(fcb), "Key令牌超时");
    }

    logRequest(req, token[1]);

    if (handleOptionsRequest(req, std::move(fccb))) {
      return;
    }

    if (!isSigned((*json), token[1])) {
      return sendErrorResponse(std::move(fcb), "Key令牌不正确");
    }

    fccb();
  } catch (const std::exception &e) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                      "{} - {}:{}", e.what(), __FILE__,
                                      __LINE__);
    sendErrorResponse(std::move(fcb), e.what());
  }
}

bool KeyFilter::isSigned(const Json::Value &params, const std::string &token) {
  std::string key = app().getCustomConfig()["auth"]["key"]["key"].asString();

  std::map<std::string, std::string> signParams;

  if (auto json = params) {
    for (const auto &key : json.getMemberNames()) {
      std::string lkey = utils::String::toLower(key);
      signParams[lkey] = json[key].asString();
    }
  }

  std::string content;
  for (const auto &[k, v] : signParams) {
    content += k + "=" + v + "&";
  }
  if (!content.empty())
    content.pop_back();

  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int len = 0;

  HMAC(EVP_sha256(), key.data(), key.size(),
       reinterpret_cast<const unsigned char *>(content.data()),
       static_cast<int>(static_cast<unsigned int>(content.size())), digest,
       &len);

  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (size_t i = 0; i < len; ++i) {
    ss << std::setw(2) << static_cast<int>(digest[i]);
  }

  auto serverSign = ss.str();

  if (serverSign.size() != token.size())
    return false;
  unsigned char result = 0;
  for (size_t i = 0; i < serverSign.size(); ++i) {
    result |= serverSign[i] ^ token[i];
  }

  return result == 0;
}

void OidcFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                          FilterChainCallback &&fccb) {
  std::string params = "";
  std::shared_ptr<Json::Value> json = req->getJsonObject();

  if (json) {
    params = (*json).toStyledString();
  }

  logRequest(req, params);

  if (handleOptionsRequest(req, std::move(fccb))) {
    return;
  }

  auto token = utils::String::split(req->getHeader("Authorization"), ' ');

  if (token.size() < 2 || token[0] != "Bearer" || token[1].empty()) {
    return sendErrorResponse(std::move(fcb), "OIDC令牌不正确");
  }

  try {
    // Decode the JWT to get the issuer
    const auto &jwt = jwt::decode(token[1]);
    std::string issuer = jwt.get_issuer();

    // Get our own configuration to find matching OIDC provider
    auto &config = app().getCustomConfig();
    Json::Value oidcProviders = config["auth"]["oidc_providers"];

    if (oidcProviders.isNull() || !oidcProviders.isArray()) {
      common::Logger::getInstance().log(
          common::Logger::LOGLEVEL::error, false,
          "Missing or invalid OIDC providers configuration - {}:{}", __FILE__,
          __LINE__);
      return sendErrorResponse(std::move(fcb), "OIDC配置错误");
    }

    Json::Value providerConfig;
    bool providerFound = false;

    // Find the provider configuration that matches the token issuer
    for (const auto &provider : oidcProviders) {
      if (provider.isMember("issuer") &&
          provider["issuer"].asString() == issuer) {
        providerConfig = provider;
        providerFound = true;
        break;
      }
    }

    if (!providerFound) {
      common::Logger::getInstance().log(
          common::Logger::LOGLEVEL::error, false,
          "No matching OIDC provider found for issuer: {} - {}:{}", issuer,
          __FILE__, __LINE__);
      return sendErrorResponse(std::move(fcb), "不支持的OIDC提供商");
    }

    // Get JWKS endpoint from provider configuration
    std::string jwks_uri = providerConfig.get("jwks_uri", "").asString();
    if (jwks_uri.empty() && providerConfig.isMember("issuer")) {
      // Construct standard JWKS URI if not explicitly provided
      std::string issuer_url = providerConfig["issuer"].asString();
      if (issuer_url.back() == '/') {
        jwks_uri = issuer_url + ".well-known/jwks.json";
      } else {
        jwks_uri = issuer_url + "/.well-known/jwks.json";
      }
    }

    if (jwks_uri.empty()) {
      common::Logger::getInstance().log(
          common::Logger::LOGLEVEL::error, false,
          "Missing jwks_uri for OIDC provider - {}:{}", __FILE__, __LINE__);
      return sendErrorResponse(std::move(fcb), "OIDC配置错误");
    }

    // Fetch JWKS
    cpr::Response res = cpr::Get(
        cpr::Url{jwks_uri}, cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
        cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
        cpr::VerifySsl(false));

    if (res.status_code == 200) {
      Json::Reader reader;
      Json::Value body;

      if (reader.parse(res.text, body)) {
        std::string pem = "";

        // Find the key that matches the JWT
        for (const auto &key : body["keys"]) {
          if (key.isMember("kid") &&
              key["kid"].asString() == jwt.get_key_id()) {
            pem = utils::Jwt::toPem(key);
            break;
          }
        }

        if (pem.empty()) {
          common::Logger::getInstance().log(
              common::Logger::LOGLEVEL::error, false,
              "Unable to find matching key in JWKS - {}:{}", __FILE__,
              __LINE__);
          return sendErrorResponse(std::move(fcb), "OIDC密钥错误");
        }

        // Determine audience based on token type
        std::string audience;
        if (utils::String::toLower(jwt.get_type()) == "at+jwt") {
          // Access token - use the service's URL as audience
          audience = fmt::format("http://{}",
                                 drogon::app().getListeners()[0].toIpPort());
        } else {
          // ID token - use client ID as audience
          audience = providerConfig.get("client_id", "").asString();
        }

        // Setup verifier with supported algorithms
        auto verifier =
            jwt::verify()
                .with_type(jwt.get_type())
                .with_issuer(issuer)
                .with_audience(audience)
                .expires_at_leeway(
                    std::chrono::duration_cast<std::chrono::seconds>(
                        jwt.get_expires_at().time_since_epoch())
                        .count());

        // Allow algorithms based on what's configured for this provider
        Json::Value supportedAlgorithms =
            providerConfig.get("supported_algorithms", Json::arrayValue);
        if (supportedAlgorithms.isNull() || supportedAlgorithms.empty()) {
          // Default algorithms if none specified
          verifier.allow_algorithm(jwt::algorithm::rs256(pem))
              .allow_algorithm(jwt::algorithm::rs384(pem))
              .allow_algorithm(jwt::algorithm::rs512(pem))
              .allow_algorithm(jwt::algorithm::es256(pem))
              .allow_algorithm(jwt::algorithm::es384(pem))
              .allow_algorithm(jwt::algorithm::es512(pem));
        } else {
          // Use configured algorithms
          for (const auto &alg : supportedAlgorithms) {
            std::string algorithm = alg.asString();
            if (algorithm == "RS256")
              verifier.allow_algorithm(jwt::algorithm::rs256(pem));
            else if (algorithm == "RS384")
              verifier.allow_algorithm(jwt::algorithm::rs384(pem));
            else if (algorithm == "RS512")
              verifier.allow_algorithm(jwt::algorithm::rs512(pem));
            else if (algorithm == "ES256")
              verifier.allow_algorithm(jwt::algorithm::es256(pem));
            else if (algorithm == "ES384")
              verifier.allow_algorithm(jwt::algorithm::es384(pem));
            else if (algorithm == "ES512")
              verifier.allow_algorithm(jwt::algorithm::es512(pem));
            // Add more algorithms as needed
          }
        }

        // Verify the JWT
        verifier.verify(jwt);

        // Store JWT claims in request attributes
        for (const auto &[key, value] : jwt.get_payload_json()) {
          req->getAttributes()->insert("jwt_" + key, value);
        }

        // Parse claims for role/scope checking
        Json::Value claims;
        Json::Reader().parse(jwt.get_payload(), claims);

        if (utils::String::toLower(jwt.get_type()) == "at+jwt") {
          // For access tokens, check scopes
          Json::Value scopes = claims.get("scope", Json::arrayValue);
          if (hasPermission(scopes, req->getPath())) {
            return fccb();
          } else {
            sendErrorResponse(std::move(fcb), "OIDC权限不足");
            return;
          }
        } else {
          // For ID tokens, check roles
          if (hasRole(claims["roles"], req->getPath())) {
            return fccb();
          } else {
            sendErrorResponse(std::move(fcb), "OIDC权限不足");
            return;
          }
        }
      }
    }

    common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                      "OIDC解析失败 - {}:{}", __FILE__,
                                      __LINE__);
    sendErrorResponse(std::move(fcb), "OIDC解析失败");
  } catch (const std::exception &e) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                      "{} - {}:{}", e.what(), __FILE__,
                                      __LINE__);
    sendErrorResponse(std::move(fcb), e.what());
  }
}

bool OidcFilter::hasRole(const Json::Value &userRoles,
                         const std::string &path) {
  static const std::unordered_map<std::string, std::set<std::string>> roleMap =
      {{"/test1", {"super admin"}}};

  auto it = roleMap.find(path);
  if (it == roleMap.end())
    return true;

  for (const auto &role : userRoles) {
    if (it->second.count(role.asString()))
      return true;
  }

  return false;
}

bool OidcFilter::hasPermission(const Json::Value &userScopes,
                               const std::string &path) {
  static const std::unordered_map<std::string, std::set<std::string>> scopeMap =
      {{"/test2", {"read:all, write:all"}}};

  auto it = scopeMap.find(path);
  if (it == scopeMap.end())
    return true;

  for (const auto &scope : userScopes) {
    if (it->second.count(scope.asString()))
      return true;
  }

  return false;
}

} // namespace common