#include "project.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void Project::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM project WHERE \"delete\" = 0";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

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

void Project::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = fmt::format("SELECT * FROM project WHERE \"delete\" = 0");

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("sortBy")) {
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

    if ((*json)["descending"].asBool())
      stmt = fmt::format("{} DESC", stmt);
    else
      stmt = fmt::format("{} ASC", stmt);
  }

  stmt =
      fmt::format("{} LIMIT {} OFFSET {}", stmt, (*json)["maxPage"].asInt64(),
                  (*json)["curPage"].asInt64());

  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(stmt);

      Json::Value ret;
      Json::Value infos;

      for (auto it : r) {
        Json::Value info;
        info["id"] = it["id"].as<std::string>();
        info["name"] = it["name"].as<std::string>();
        info["class"] = it["class"].as<std::string>();
        info["license"] = it["license"].as<std::string>();
        info["pledge"] = it["pledge"].as<std::string>();
        info["cost"] = it["cost"].as<double>();
        info["deed"] = it["deed"].as<std::string>();
        info["type"] = it["type"].as<std::string>();
        info["purpose"] = it["purpose"].as<std::string>();
        info["reply"] = it["reply"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["area"] = it["area"].as<std::string>();
        info["street"] = it["street"].as<std::string>();
        info["location"] = it["location"].as<Json::Value>();
        info["income"] = it["income"].as<std::string>();
        info["remark"] = it["remark"].as<std::string>();
        info["attachment"] = it["attachment"].as<Json::Value>();
        info["public"] = it["public"].as<bool>();
        info["create_date"] = it["create_date"].as<std::string>();
        info["create_user"] =
            co_await User::get_user(it["create_user"].as<std::string>());
        info["update_date"] = it["update_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(it["update_user"].as<std::string>());
        info["preview"] = it["preview"].as<std::string>();
        infos.append(info);
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

void Project::public_list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = fmt::format(
      "SELECT * FROM project WHERE \"delete\" = 0 AND \"public\" = 1");

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("sortBy")) {
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

    if ((*json)["descending"].asBool())
      stmt = fmt::format("{} DESC", stmt);
    else
      stmt = fmt::format("{} ASC", stmt);
  }

  stmt =
      fmt::format("{} LIMIT {} OFFSET {}", stmt, (*json)["maxPage"].asInt64(),
                  (*json)["curPage"].asInt64());

  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(stmt);

      Json::Value ret;
      Json::Value infos;

      for (auto it : r) {
        Json::Value info;
        info["id"] = it["id"].as<std::string>();
        info["name"] = it["name"].as<std::string>();
        info["class"] = it["class"].as<std::string>();
        info["license"] = it["license"].as<std::string>();
        info["pledge"] = it["pledge"].as<std::string>();
        info["cost"] = it["cost"].as<double>();
        info["deed"] = it["deed"].as<std::string>();
        info["type"] = it["type"].as<std::string>();
        info["purpose"] = it["purpose"].as<std::string>();
        info["reply"] = it["reply"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["area"] = it["area"].as<std::string>();
        info["street"] = it["street"].as<std::string>();
        info["location"] = it["location"].as<Json::Value>();
        info["income"] = it["income"].as<std::string>();
        info["remark"] = it["remark"].as<std::string>();
        info["attachment"] = it["attachment"].as<Json::Value>();
        info["public"] = it["public"].as<bool>();
        info["create_date"] = it["create_date"].as<std::string>();
        info["create_user"] =
            co_await User::get_user(it["create_user"].as<std::string>());
        info["update_date"] = it["update_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(it["update_user"].as<std::string>());
        info["preview"] = it["preview"].as<std::string>();
        infos.append(info);
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
} // namespace api
