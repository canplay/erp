#include "include/filter.h"
#include "include/utility.h"
#include "pch.h"

#include <jwt-cpp/jwt.h>
#include <openssl/hmac.h>

namespace common {
void LogFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                         FilterChainCallback &&fccb) {
  std::string params = "";
  std::shared_ptr<Json::Value> json = req->getJsonObject();

  if (json) {
    params = (*json).toStyledString();
  }

  std::string path_fix = "";

  for (auto i : req->getParameters()) {
    if (i == *req->getParameters().begin()) {
      path_fix += fmt::format("?{}={}", i.first, i.second);
    } else {
      path_fix += fmt::format("&{}={}", i.first, i.second);
    }
  }

  common::utility::log(
      common::utility::LOGLEVEL::info, false, "{} {} [{}] {}{} {}",
      req->getHeader("X-Forwarded-For"), req->getHeader("User-Agent"),
      req->getMethodString(), common::utility::utf8ToGBK(req->getPath()),
      common::utility::utf8ToGBK(path_fix), common::utility::utf8ToGBK(params));

  fccb();
}

void JwtFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                         FilterChainCallback &&fccb) {
  std::string params = "";
  std::shared_ptr<Json::Value> json = req->getJsonObject();

  if (json) {
    params = (*json).toStyledString();
  }

  std::string path_fix = "";

  for (auto i : req->getParameters()) {
    if (i == *req->getParameters().begin()) {
      path_fix += fmt::format("?{}={}", i.first, i.second);
    } else {
      path_fix += fmt::format("&{}={}", i.first, i.second);
    }
  }

  common::utility::log(
      common::utility::LOGLEVEL::info, false, "{} {} [{}] {}{} {}",
      req->getHeader("X-Forwarded-For"), req->getHeader("User-Agent"),
      req->getMethodString(), common::utility::utf8ToGBK(req->getPath()),
      common::utility::utf8ToGBK(path_fix), common::utility::utf8ToGBK(params));

  if (req->getMethod() == HttpMethod::Options)
    return fccb();

  auto token = utility::split(req->getHeader("Authorization"), ' ');

  if (token.size() < 2 || token[0] != "Bearer" || token[1].empty()) {
    Json::Value ret;
    ret["message"] = "Jwt令牌不正确";
    ret["status"] = 0;

    return fcb(HttpResponse::newHttpJsonResponse(ret));
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
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::hs384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "hs512") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::hs512(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "rs256") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::rs256(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "rs384") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::rs384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "rs512") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::rs512(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es256") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es256(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es384") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es512") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es512(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "es256k") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::es256k(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ed25519") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ed25519(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ed448") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ed448(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ps256") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ps256(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ps384") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ps384(pemPublicKey));
    } else if (app().getCustomConfig()["auth"]["jwt"]["alg"].asString() ==
               "ps512") {
      const std::string &pemPublicKey = utility::readPemFile(
          app().getCustomConfig()["auth"]["jwt"]["public"].asString());
      verifier.allow_algorithm(jwt::algorithm::ps512(pemPublicKey));
    } else {
      verifier.allow_algorithm(jwt::algorithm::hs256(
          app().getCustomConfig()["auth"]["jwt"]["key"].asString()));
    }

    verifier.verify(jwt_decode);

    for (const auto &it : jwt_decode.get_payload_json()) {
      req->getAttributes()->insert("jwt_" + it.first, it.second);
    }

    fccb();
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);
    Json::Value ret;
    ret["message"] = "Jwt令牌不正确";
    ret["status"] = 0;
    fcb(HttpResponse::newHttpJsonResponse(ret));
  }
}

void KeyFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                         FilterChainCallback &&fccb) {
  auto token = utility::split(req->getHeader("Authorization"), ' ');

  try {
    if (token.size() < 2 || token[0] != "Bearer" || token[1].empty()) {
      Json::Value ret;
      ret["message"] = "Key令牌不正确";
      ret["status"] = 0;

      return fcb(HttpResponse::newHttpJsonResponse(ret));
    }

    std::shared_ptr<Json::Value> json = req->getJsonObject();

    auto requestTime =
        std::chrono::system_clock::from_time_t((*json)["timestamp"].asInt64());
    auto now = std::chrono::system_clock::now();
    auto diff =
        std::chrono::duration_cast<std::chrono::seconds>(now - requestTime)
            .count();
    if (std::abs(diff) > 300) {
      Json::Value ret;
      ret["message"] = "Key令牌超时";
      ret["status"] = 0;

      return fcb(HttpResponse::newHttpJsonResponse(ret));
    }

    std::string path_fix = "";

    for (auto i : req->getParameters()) {
      if (i == *req->getParameters().begin()) {
        path_fix += fmt::format("?{}={}", i.first, i.second);
      } else {
        path_fix += fmt::format("&{}={}", i.first, i.second);
      }
    }

    common::utility::log(
        common::utility::LOGLEVEL::info, false, "{} {} [{}] {}{} {}",
        req->getHeader("X-Forwarded-For"), req->getHeader("User-Agent"),
        req->getMethodString(), common::utility::utf8ToGBK(req->getPath()),
        common::utility::utf8ToGBK(path_fix),
        common::utility::utf8ToGBK(token[1]));

    if (req->getMethod() == HttpMethod::Options)
      return fccb();

    if (!isSigned((*json), token[1])) {
      Json::Value ret;
      ret["message"] = "Key令牌不正确";
      ret["status"] = 0;

      return fcb(HttpResponse::newHttpJsonResponse(ret));
    }

    fccb();
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);
    Json::Value ret;
    ret["message"] = e.what();
    ret["status"] = 0;
    fcb(HttpResponse::newHttpJsonResponse(ret));
  }
}

bool KeyFilter::isSigned(const Json::Value &params, const std::string &token) {
  std::string key = app().getCustomConfig()["auth"]["key"]["key"].asString();

  std::map<std::string, std::string> signParams;

  if (auto json = params) {
    for (const auto &key : json.getMemberNames()) {
      std::string lkey = utility::toLower(key);
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
       static_cast<int>(content.size()), digest, &len);

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

void M2mFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                         FilterChainCallback &&fccb) {
  std::string params = "";
  std::shared_ptr<Json::Value> json = req->getJsonObject();

  if (json) {
    params = (*json).toStyledString();
  }

  std::string path_fix = "";

  for (auto i : req->getParameters()) {
    if (i == *req->getParameters().begin()) {
      path_fix += fmt::format("?{}={}", i.first, i.second);
    } else {
      path_fix += fmt::format("&{}={}", i.first, i.second);
    }
  }

  common::utility::log(
      common::utility::LOGLEVEL::info, false, "{} {} [{}] {}{} {}",
      req->getHeader("X-Forwarded-For"), req->getHeader("User-Agent"),
      req->getMethodString(), common::utility::utf8ToGBK(req->getPath()),
      common::utility::utf8ToGBK(path_fix), common::utility::utf8ToGBK(params));

  if (req->getMethod() == HttpMethod::Options)
    return fccb();

  auto token = utility::split(req->getHeader("Authorization"), ' ');

  if (token.size() < 2 || token[0] != "Bearer" || token[1].empty() ||
      token[1] != app().getCustomConfig()["auth"]["key"]["key"].asString()) {
    Json::Value ret;
    ret["message"] = "M2M令牌不正确";
    ret["status"] = 0;

    return fcb(HttpResponse::newHttpJsonResponse(ret));
  }

  fccb();
}

void LogtoFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                           FilterChainCallback &&fccb) {
  std::string params = "";
  std::shared_ptr<Json::Value> json = req->getJsonObject();

  if (json) {
    params = (*json).toStyledString();
  }

  std::string path_fix = "";

  for (auto i : req->getParameters()) {
    if (i == *req->getParameters().begin()) {
      path_fix += fmt::format("?{}={}", i.first, i.second);
    } else {
      path_fix += fmt::format("&{}={}", i.first, i.second);
    }
  }

  common::utility::log(
      common::utility::LOGLEVEL::info, false, "{} {} [{}] {}{} {}",
      req->getHeader("X-Forwarded-For"), req->getHeader("User-Agent"),
      req->getMethodString(), common::utility::utf8ToGBK(req->getPath()),
      common::utility::utf8ToGBK(path_fix), common::utility::utf8ToGBK(params));

  if (req->getMethod() == HttpMethod::Options)
    return fccb();

  auto token = utility::split(req->getHeader("Authorization"), ' ');

  if (token.size() < 2 || token[0] != "Bearer" || token[1].empty()) {
    Json::Value ret;
    ret["message"] = "Logto令牌不正确";
    ret["status"] = 0;

    return fcb(HttpResponse::newHttpJsonResponse(ret));
  }

  try {
    std::string jwks_url = fmt::format(
        "{}/jwks",
        app().getCustomConfig()["auth"]["logto"]["issuer"].asString());

    auto ar = cpr::GetAsync(cpr::Url{jwks_url},
                            cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                            cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                            cpr::VerifySsl(false));
    auto res = ar.get();

    if (res.status_code == 200) {
      Json::Reader reader;
      Json::Value body;

      if (reader.parse(res.text, body)) {
        const auto &jwt = jwt::decode(token[1]);
        std::string pem = "";

        for (const auto &key : body["keys"]) {
          if (key.isMember("kid") &&
              key["kid"].asString() == jwt.get_key_id() &&
              key.isMember("kty") && key["kty"].asString() == "EC" &&
              key.isMember("crv") && key["crv"].asString() == "P-384") {
            pem = utility::jwkToPem(key);
            break;
          }
        }

        if (common::utility::toLower(jwt.get_type()) == "at+jwt") {
          jwt::verify()
              .with_type(jwt.get_type())
              .with_issuer(
                  app().getCustomConfig()["auth"]["logto"]["issuer"].asString())
              .with_audience(fmt::format(
                  "http://{}", drogon::app().getListeners()[0].toIpPort()))
              .allow_algorithm(jwt::algorithm::es384(pem))
              .expires_at_leeway(
                  std::chrono::duration_cast<std::chrono::seconds>(
                      jwt.get_expires_at().time_since_epoch())
                      .count())
              .verify(jwt);

          for (const auto &it : jwt.get_payload_json()) {
            req->getAttributes()->insert("jwt_" + it.first, it.second);
          }

          Json::Value claims;
          Json::Reader().parse(jwt.get_payload(), claims);
          Json::Value scopes = claims.get("scope", Json::arrayValue);
          if (hasPermission(scopes, req->getPath())) {
            return fccb();
          } else {
            Json::Value ret;
            ret["message"] = "Logto权限不足";
            ret["status"] = 0;
            fcb(HttpResponse::newHttpJsonResponse(ret));
          }
        } else {
          jwt::verify()
              .with_type(jwt.get_type())
              .with_issuer(
                  app().getCustomConfig()["auth"]["logto"]["issuer"].asString())
              .with_audience(
                  app().getCustomConfig()["auth"]["logto"]["id"].asString())
              .allow_algorithm(jwt::algorithm::es384(pem))
              .expires_at_leeway(
                  std::chrono::duration_cast<std::chrono::seconds>(
                      jwt.get_expires_at().time_since_epoch())
                      .count())
              .verify(jwt);

          for (const auto &it : jwt.get_payload_json()) {
            req->getAttributes()->insert("jwt_" + it.first, it.second);
          }

          Json::Value claims;
          Json::Reader().parse(jwt.get_payload(), claims);
          if (hasRole(claims["roles"], req->getPath())) {
            return fccb();
          } else {
            Json::Value ret;
            ret["message"] = "Logto权限不足";
            ret["status"] = 0;
            fcb(HttpResponse::newHttpJsonResponse(ret));
          }
        }
      }
    }

    common::utility::log(common::utility::LOGLEVEL::info, false,
                         "Logto解析失败 - {}:{}", __FILE__, __LINE__);
    Json::Value ret;
    ret["message"] = "Logto解析失败";
    ret["status"] = 0;
    fcb(HttpResponse::newHttpJsonResponse(ret));
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);
    Json::Value ret;
    ret["message"] = e.what();
    ret["status"] = 0;
    fcb(HttpResponse::newHttpJsonResponse(ret));
  }
}

bool LogtoFilter::hasRole(const Json::Value &userRoles,
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

bool LogtoFilter::hasPermission(const Json::Value &userScopes,
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
