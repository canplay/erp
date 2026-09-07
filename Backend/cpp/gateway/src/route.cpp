#include "route.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Route::route(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  const std::string &auth, const std::string &service,
                  const std::string &method, const std::string &path) const {
  async_run([=, this]() -> Task<> {
    if (method == "get") {
      callback(HttpResponse::newHttpJsonResponse(
          co_await route_get(req, auth, service, path)));
    } else if (method == "post") {
      callback(HttpResponse::newHttpJsonResponse(
          co_await route_post(req, auth, service, path)));
    } else {
      Json::Value ret;
      ret["message"] = "方法无效";
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  });
}

void Route::get(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                const std::string &auth, const std::string &service,
                const std::string &path) const {
  async_run([=, this]() -> Task<> {
    try {
      auto service_json = co_await get_service(service);

      if (service_json["status"] == 0) {
        co_return callback(HttpResponse::newHttpJsonResponse(service_json));
      }

      service_json = service_json["data"];

      std::string path_fix = "";
      for (auto i : req->getParameters()) {
        if (i == *req->getParameters().begin()) {
          path_fix += fmt::format("?{}={}", i.first, i.second);
        } else {
          path_fix += fmt::format("&{}={}", i.first, i.second);
        }
      }

      cpr::Header headers;

      if (auth == "key" || auth == "jwt") {
        headers = {
            {"Accept-Encoding", "gzip, deflate, br"},
            {"User-Agent", "CaNplay/1.0"},
            {"Authorization", req->getHeader("Authorization")},
            {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
        };
      } else if (auth == "logto") {
        headers = {
            {"Accept-Encoding", "gzip, deflate, br"},
            {"User-Agent", "CaNplay/1.0"},
            {"Authorization", req->getHeader("Authorization")},
            {"client_id",
             app().getCustomConfig()["auth"]["logto"]["id"].asString()},
            {"client_secret",
             app().getCustomConfig()["auth"]["logto"]["secret"].asString()},
            {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
        };
      } else {
        headers = {
            {"Accept-Encoding", "gzip, deflate, br"},
            {"User-Agent", "CaNplay/1.0"},
            {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
        };
      }

      auto ar = cpr::GetAsync(
          cpr::Url{fmt::format("{}{}{}", service_json["url"].asString(), path,
                               path_fix)},
          headers, cpr::Timeout{30000});
      auto res = ar.get();

      if (res.status_code == 200) {
        Json::Reader reader;
        Json::Value ret;
        reader.parse(res.text, ret);
        co_return callback(HttpResponse::newHttpJsonResponse(ret));
      }

      common::utility::log(common::utility::LOGLEVEL::info, false,
                           "{}{}{}\n=====> {}:{} - {}:{}",
                           service_json["url"].asString(), path, path_fix,
                           res.status_code, res.text, __FILE__, __LINE__);
      Json::Value ret;
      ret["message"] = "GET方法无效";
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    } catch (const std::exception &e) {
      common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                           e.what(), __FILE__, __LINE__);

      Json::Value ret;
      ret["message"] = e.what();
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    };
  });
}

void Route::post(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback,
                 const std::string &auth, const std::string &service,
                 const std::string &path) const {
  async_run([=, this]() -> Task<> {
    try {
      auto service_json = co_await get_service(service);

      if (service_json["status"] == 0) {
        co_return callback(HttpResponse::newHttpJsonResponse(service_json));
      }

      service_json = service_json["data"];

      std::string params = "";
      std::shared_ptr<Json::Value> json = req->getJsonObject();
      if (json) {
        params = (*json).toStyledString();
      }

      cpr::Multipart MultipartItems{};
      MultiPartParser multiPartParser;
      if (multiPartParser.parse(req) != 0) {
        auto it = multiPartParser.getParameters().begin();
        while (it != multiPartParser.getParameters().end()) {
          auto a = it->first;
          std::transform(a.begin(), a.end(), a.begin(), tolower);

          cpr::Part data{"", ""};

          if (a == "name")
            data.name = it->second;
          else if (a == "filename")
            data.value = it->second;
          else if (a == "Content-Type")
            data.content_type = it->second;
          else if (a == "Content")
            data.value = it->second;

          MultipartItems.parts.push_back(data);
        }
      }

      std::string path_fix = "";
      for (auto i : req->getParameters()) {
        if (i == *req->getParameters().begin()) {
          path_fix += fmt::format("?{}={}", i.first, i.second);
        } else {
          path_fix += fmt::format("&{}={}", i.first, i.second);
        }
      }

      cpr::Header headers;

      if (auth == "key" || auth == "jwt") {
        headers = {
            {"Accept-Encoding", "gzip, deflate, br"},
            {"User-Agent", "CaNplay/1.0"},
            {"Content-type", "application/json"},
            {"Authorization", req->getHeader("Authorization")},
            {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
        };
      } else if (auth == "logto") {
        headers = {
            {"Accept-Encoding", "gzip, deflate, br"},
            {"User-Agent", "CaNplay/1.0"},
            {"Content-type", "application/json"},
            {"Authorization", req->getHeader("Authorization")},
            {"client_id",
             app().getCustomConfig()["auth"]["logto"]["id"].asString()},
            {"client_secret",
             app().getCustomConfig()["auth"]["logto"]["secret"].asString()},
            {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
        };
      } else {
        headers = {{"Accept-Encoding", "gzip, deflate, br"},
                   {"User-Agent", "CaNplay/1.0"},
                   {"Content-type", "application/json"}};
      }

      auto ar = cpr::PostAsync(
          cpr::Url{fmt::format("{}{}{}", service_json["url"].asString(), path,
                               path_fix)},
          headers, cpr::Timeout{30000}, cpr::Body{params});
      auto res = ar.get();

      if (res.status_code == 200) {
        Json::Reader reader;
        Json::Value body;
        if (reader.parse(res.text, body)) {
          Json::Value ret;
          ret = body;
          co_return callback(HttpResponse::newHttpJsonResponse(ret));
        }
      }

      Json::Value ret;
      ret["message"] = "POST方法无效";
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    } catch (const std::exception &e) {
      common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                           e.what(), __FILE__, __LINE__);

      Json::Value ret;
      ret["message"] = e.what();
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    };
  });
}

Task<Json::Value> Route::get_service(const std::string &service) const {
  try {
    auto redisClient = app().getRedisClient("redis-slave");
    auto redis_result = co_await redisClient->execCommandCoro("GET service:%s",
                                                              service.c_str());

    if (redis_result.isNil() || redis_result.asString().empty()) {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM service WHERE name = $1",
                                        service);

      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["name"] = row["name"].as<std::string>();
        info["status"] = row["status"].as<std::string>();
        info["create_date"] = row["create_date"].as<std::string>();
        info["update_date"] = row["update_date"].as<std::string>();
        info["url"] = row["url"].as<std::string>();
        info["client_id"] = row["client_id"].as<std::string>();
        info["client_secret"] = row["client_secret"].as<std::string>();
        info["access_token"] = row["access_token"].as<std::string>();

        auto redisClient = app().getRedisClient("redis-master");
        co_await redisClient->execCommandCoro(
            "SET service:%s %s EX 600", row["name"].as<std::string>().c_str(),
            Json::FastWriter().write(info).c_str());
      }

      Json::Value ret;
      ret["data"] = info;
      ret["message"] = "success";
      ret["status"] = 1;
      co_return ret;
    } else {
      Json::Value info;
      Json::Reader reader;
      if (reader.parse(redis_result.asString(), info)) {
        Json::Value ret;
        ret["data"] = info;
        ret["message"] = "success";
        ret["status"] = 1;
        co_return ret;
      } else {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "get service redis error - {}:{}", __FILE__,
                             __LINE__);

        Json::Value ret;
        ret["message"] = "获取节点缓存失败";
        ret["status"] = 0;
        co_return ret;
      }
    }
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);

    Json::Value ret;
    ret["message"] = e.what();
    ret["status"] = 0;
    co_return ret;
  }
}

Task<Json::Value> Route::route_get(const HttpRequestPtr &req,
                                   const std::string &auth,
                                   const std::string &service,
                                   const std::string &path) const {
  try {
    auto service_json = co_await get_service(service);

    if (service_json["status"] == 0) {
      co_return service_json;
    }

    service_json = service_json["data"];

    std::string path_fix = "";
    for (auto i : req->getParameters()) {
      if (i == *req->getParameters().begin()) {
        path_fix += fmt::format("?{}={}", i.first, i.second);
      } else {
        path_fix += fmt::format("&{}={}", i.first, i.second);
      }
    }

    cpr::Header headers;

    if (auth == "key" || auth == "jwt") {
      headers = {
          {"Accept-Encoding", "gzip, deflate, br"},
          {"User-Agent", "CaNplay/1.0"},
          {"Authorization", req->getHeader("Authorization")},
          {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
      };
    } else if (auth == "logto") {
      headers = {
          {"Accept-Encoding", "gzip, deflate, br"},
          {"User-Agent", "CaNplay/1.0"},
          {"Authorization", req->getHeader("Authorization")},
          {"client_id",
           app().getCustomConfig()["auth"]["logto"]["id"].asString()},
          {"client_secret",
           app().getCustomConfig()["auth"]["logto"]["secret"].asString()},
          {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
      };
    } else {
      headers = {
          {"Accept-Encoding", "gzip, deflate, br"},
          {"User-Agent", "CaNplay/1.0"},
          {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
      };
    }

    auto ar = cpr::GetAsync(
        cpr::Url{fmt::format("{}{}{}", service_json["url"].asString(), path,
                             path_fix)},
        headers, cpr::Timeout{30000});
    auto res = ar.get();

    if (res.status_code == 200) {
      Json::Reader reader;
      Json::Value ret;
      reader.parse(res.text, ret);
      co_return ret;
    }

    common::utility::log(common::utility::LOGLEVEL::info, false,
                         "{}{}{}\n=====> {}:{} - {}:{}",
                         service_json["url"].asString(), path, path_fix,
                         res.status_code, res.text, __FILE__, __LINE__);
    Json::Value ret;
    ret["message"] = "GET方法无效";
    ret["status"] = 0;
    co_return ret;
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);

    Json::Value ret;
    ret["message"] = e.what();
    ret["status"] = 0;
    co_return ret;
  };
}

Task<Json::Value> Route::route_post(const HttpRequestPtr &req,
                                    const std::string &auth,
                                    const std::string &service,
                                    const std::string &path) const {
  try {
    auto service_json = co_await get_service(service);

    if (service_json["status"] == 0) {
      co_return service_json;
    }

    service_json = service_json["data"];

    std::string params = "";
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    if (json) {
      params = (*json).toStyledString();
    }

    cpr::Multipart MultipartItems{};
    MultiPartParser multiPartParser;
    if (multiPartParser.parse(req) != 0) {
      auto it = multiPartParser.getParameters().begin();
      while (it != multiPartParser.getParameters().end()) {
        auto a = it->first;
        std::transform(a.begin(), a.end(), a.begin(), tolower);

        cpr::Part data{"", ""};

        if (a == "name")
          data.name = it->second;
        else if (a == "filename")
          data.value = it->second;
        else if (a == "Content-Type")
          data.content_type = it->second;
        else if (a == "Content")
          data.value = it->second;

        MultipartItems.parts.push_back(data);
      }
    }

    std::string path_fix = "";
    for (auto i : req->getParameters()) {
      if (i == *req->getParameters().begin()) {
        path_fix += fmt::format("?{}={}", i.first, i.second);
      } else {
        path_fix += fmt::format("&{}={}", i.first, i.second);
      }
    }

    cpr::Header headers;

    if (auth == "key" || auth == "jwt") {
      headers = {
          {"Accept-Encoding", "gzip, deflate, br"},
          {"User-Agent", "CaNplay/1.0"},
          {"Authorization", req->getHeader("Authorization")},
          {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
      };
    } else if (auth == "logto") {
      headers = {
          {"Accept-Encoding", "gzip, deflate, br"},
          {"User-Agent", "CaNplay/1.0"},
          {"Authorization", req->getHeader("Authorization")},
          {"client_id",
           app().getCustomConfig()["auth"]["logto"]["id"].asString()},
          {"client_secret",
           app().getCustomConfig()["auth"]["logto"]["secret"].asString()},
          {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
      };
    } else {
      headers = {
          {"Accept-Encoding", "gzip, deflate, br"},
          {"User-Agent", "CaNplay/1.0"},
          {"X-Forwarded-For", req->getHeader("X-Forwarded-For")},
      };
    }

    auto ar = cpr::PostAsync(
        cpr::Url{fmt::format("{}{}{}", service_json["url"].asString(), path,
                             path_fix)},
        headers, cpr::Timeout{30000});
    auto res = ar.get();

    if (res.status_code == 200) {
      Json::Reader reader;
      Json::Value ret;
      reader.parse(res.text, ret);
      co_return ret;
    }

    common::utility::log(common::utility::LOGLEVEL::info, false,
                         "{}{}{}\n=====> {}:{} - {}:{}",
                         service_json["url"].asString(), path, path_fix,
                         res.status_code, res.text, __FILE__, __LINE__);
    Json::Value ret;
    ret["message"] = "POST方法无效";
    ret["status"] = 0;
    co_return ret;
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);

    Json::Value ret;
    ret["message"] = e.what();
    ret["status"] = 0;
    co_return ret;
  };
}
} // namespace api
