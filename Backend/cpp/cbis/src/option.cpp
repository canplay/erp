#include "option.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void Option::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM options";

  if ((*json).find("isDelete"))
    stmt = fmt::format("{} WHERE \"delete\" = {}", stmt,
                       (*json)["isDelete"].asBool());

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];
          ret["data"] = row[0].as<int>();
          ret["message"] = "success";
          ret["status"] = 1;
        } else {
          ret["data"] = 0;
          ret["message"] = "success";
          ret["status"] = 1;
        }

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      });
}

void Option::info(const HttpRequestPtr &,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  const std::string &id) const {
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(
          "SELECT * FROM options WHERE \"id\" = $1", id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["name"] = row["name"].as<std::string>();
        info["options"] = row["options"].as<Json::Value>();
        info["level"] = row["level"].as<int64_t>();
        info["delete"] = row["delete"].as<bool>();
        info["create_user"] =
            co_await User::get_user(row["create_user"].as<std::string>());
        info["create_date"] = row["create_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(row["update_user"].as<std::string>());
        info["update_date"] = row["update_date"].as<std::string>();
      }

      ret["data"] = info;
      ret["message"] = "success";
      ret["status"] = 1;
      callback(HttpResponse::newHttpJsonResponse(ret));
    } catch (const std::exception &e) {
      common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                           e.what(), __FILE__, __LINE__);

      Json::Value ret;
      ret["message"] = e.what();
      ret["status"] = 0;

      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  });
}

void Option::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  async_run([=]() -> Task<> {
    try {
      std::string stmt = "SELECT * FROM options";

      if ((*json).find("isDelete"))
        stmt = fmt::format("{} WHERE \"delete\" = {}", stmt,
                           (*json)["isDelete"].asBool());

      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(stmt);

      Json::Value ret;
      Json::Value infos;

      if (r.size() > 0) {
        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<std::string>();
          info["name"] = row["name"].as<std::string>();
          info["options"] = row["options"].as<Json::Value>();
          info["level"] = row["level"].as<int64_t>();
          info["delete"] = row["delete"].as<bool>();
          info["create_user"] =
              co_await User::get_user(row["create_user"].as<std::string>());
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_user"] =
              co_await User::get_user(row["update_user"].as<std::string>());
          info["update_date"] = row["update_date"].as<std::string>();
          infos.append(info);
        }
      }

      ret["data"] = infos;
      ret["message"] = "success";
      ret["status"] = 1;
      callback(HttpResponse::newHttpJsonResponse(ret));
    } catch (const std::exception &e) {
      common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                           e.what(), __FILE__, __LINE__);

      Json::Value ret;
      ret["message"] = e.what();
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  });
}

void Option::update(const HttpRequestPtr &,
                    std::function<void(const HttpResponsePtr &)> &&) const {}
} // namespace api
