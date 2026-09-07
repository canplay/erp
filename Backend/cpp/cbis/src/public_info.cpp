#include "public_info.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void PublicInfo::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("postgresql-slave");

  if ((*json).find("name")) {
    std::string stmt;

    if ((*json).find("name")) {
      stmt = fmt::format("SELECT id FROM account WHERE "
                         "\"delete\" = 0 AND name LIKE '%{}%'",
                         (*json)["name"].asString());
    } else {
      stmt = fmt::format("SELECT id FROM account WHERE "
                         "\"delete\" = 0 AND phone LIKE '%{}%'",
                         (*json)["phone"].asString());
    }

    db->execSqlAsync(
        stmt,
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.size() > 0) {
            std::string id;

            for (auto it : r) {
              id = fmt::format("('{}','{}')", id, it["id"].as<std::string>());
            }

            auto stmt =
                fmt::format("SELECT COUNT(id) FROM public_info "
                            "WHERE \"delete\" = 0 AND create_user IN {}",
                            id);

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
                                       "{} - {}:{}", e.base().what(), __FILE__,
                                       __LINE__);

                  Json::Value ret;
                  ret["message"] = e.base().what();
                  ret["status"] = 0;

                  callback(HttpResponse::newHttpJsonResponse(ret));
                });
          } else {
            ret["data"] = 0;
            ret["message"] = "success";
            ret["status"] = 1;
            callback(HttpResponse::newHttpJsonResponse(ret));
          }
        },
        [=](const orm::DrogonDbException &e) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = e.base().what();
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        });
  } else {
    std::string stmt = "SELECT COUNT(id) FROM public_info WHERE "
                       "\"delete\" = 0";

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
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = e.base().what();
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        });
  }
}

void PublicInfo::list(
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
      auto db = app().getDbClient("postgresql-slave");

      if ((*json).find("name")) {
        std::string stmt;

        if ((*json).find("name")) {
          stmt = fmt::format("SELECT id FROM account WHERE "
                             "\"delete\" = 0 AND name LIKE '%{}%'",
                             (*json)["name"].asString());
        } else {
          stmt = fmt::format("SELECT id FROM account WHERE "
                             "\"delete\" = 0 AND phone LIKE '%{}%'",
                             (*json)["phone"].asString());
        }

        auto r = co_await db->execSqlCoro(stmt);

        Json::Value ret;
        Json::Value infos;

        if (r.size() > 0) {
          std::string id;

          for (auto it : r) {
            id = fmt::format("('{}','{}')", id, it["id"].as<std::string>());
          }

          auto stmt = fmt::format("SELECT * FROM public_info WHERE "
                                  "\"delete\" = 0 AND create_user IN {}",
                                  id);

          if ((*json).find("sortBy")) {
            stmt = fmt::format("{} ORDER BY {}", stmt,
                               (*json)["sortBy"].asString());

            if ((*json)["descending"].asBool())
              stmt = fmt::format("{} DESC", stmt);
            else
              stmt = fmt::format("{} ASC", stmt);
          }

          stmt = fmt::format("{} LIMIT {} OFFSET {}", stmt,
                             (*json)["maxPage"].asInt64(),
                             (*json)["curPage"].asInt64());

          auto r = co_await db->execSqlCoro(stmt);

          for (auto it : r) {
            Json::Value info;
            info["id"] = it["id"].as<std::string>();
            info["name"] = it["name"].as<std::string>();
            info["type"] = it["type"].as<std::string>();
            info["info"] = it["info"].as<Json::Value>();
            info["create_date"] = it["create_date"].as<std::string>();
            info["create_user"] =
                co_await User::get_user(it["create_user"].as<std::string>());
            info["update_date"] = it["update_date"].as<std::string>();
            info["update_user"] =
                co_await User::get_user(it["update_user"].as<std::string>());
            infos.append(info);
          }
        }

        ret["data"] = infos;
        ret["message"] = "success";
        ret["status"] = 1;
        callback(HttpResponse::newHttpJsonResponse(ret));
      } else {
        std::string stmt = fmt::format("SELECT * FROM public_info WHERE "
                                       "\"delete\" = 0");

        if ((*json).find("sortBy")) {
          stmt =
              fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

          if ((*json)["descending"].asBool())
            stmt = fmt::format("{} DESC", stmt);
          else
            stmt = fmt::format("{} ASC", stmt);
        }

        stmt = fmt::format("{} LIMIT {} OFFSET {}", stmt,
                           (*json)["maxPage"].asInt64(),
                           (*json)["curPage"].asInt64());

        auto r = co_await db->execSqlCoro(stmt);

        Json::Value ret;
        Json::Value infos;

        for (auto it : r) {
          Json::Value info;
          info["id"] = it["id"].as<std::string>();
          info["name"] = it["name"].as<std::string>();
          info["type"] = it["type"].as<std::string>();
          info["info"] = it["info"].as<Json::Value>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["create_user"] =
              co_await User::get_user(it["create_user"].as<std::string>());
          info["update_date"] = it["update_date"].as<std::string>();
          info["update_user"] =
              co_await User::get_user(it["update_user"].as<std::string>());
          infos.append(info);
        }

        ret["data"] = infos;
        ret["message"] = "success";
        ret["status"] = 1;
        callback(HttpResponse::newHttpJsonResponse(ret));
      }
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

void PublicInfo::info(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &id) const {
  async_run([=]() -> Task<> {
    auto db = app().getDbClient("postgresql-slave");
    auto r = co_await db->execSqlCoro("SELECT * FROM public_info "
                                      "WHERE \"id\" = $1 AND \"delete\" = 0",
                                      id);

    Json::Value ret;
    Json::Value info;

    if (r.size() > 0) {
      auto row = r[0];
      info["id"] = row["id"].as<std::string>();
      info["name"] = row["name"].as<std::string>();
      info["type"] = row["type"].as<std::string>();
      info["info"] = row["info"].as<Json::Value>();
      info["create_date"] = row["create_date"].as<std::string>();
      info["create_user"] =
          co_await User::get_user(row["create_user"].as<std::string>());
      info["update_date"] = row["update_date"].as<std::string>();
      info["update_user"] =
          co_await User::get_user(row["update_user"].as<std::string>());
    }

    ret["data"] = info;
    ret["message"] = "success";
    ret["status"] = 1;
    callback(HttpResponse::newHttpJsonResponse(ret));
  });
}

void PublicInfo::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto timestamp = common::utility::timeNow();

  auto db = app().getDbClient("postgresql-master");

  if ((*json)["method"].asString() == "add") {
    db->execSqlAsync(
        "INSERT INTO public_info VALUES ($1, $2, $3, $4, $5, "
        "$6, $7, $8, false)",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "更新失败";
            ret["status"] = 0;
          }

          callback(HttpResponse::newHttpJsonResponse(ret));
        },
        [=](const orm::DrogonDbException &e) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = e.base().what();
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        },
        common::utility::uuid(), (*json)["name"].asString(),
        (*json)["type"].asString(), (*json)["info"].toStyledString(),
        (*json)["user"].asString(), timestamp, (*json)["user"].asString(),
        timestamp);
  } else if ((*json)["method"].asString() == "update") {
    auto qrcode = fmt::format("{}/{}", (*json)["qrcode"].asString(),
                              (*json)["id"].asString());

    db->execSqlAsync(
        "UPDATE public_info SET name = $1, \"type\" = $2, info = $3, "
        "update_user = $4, update_date = $5 WHERE \"id\" = $6",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "更新失败";
            ret["status"] = 0;
          }

          callback(HttpResponse::newHttpJsonResponse(ret));
        },
        [=](const orm::DrogonDbException &e) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = e.base().what();
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        },
        (*json)["name"].asString(), (*json)["type"].asString(),
        (*json)["info"].toStyledString(), (*json)["user"].asString(), timestamp,
        (*json)["id"].asString());
  } else if ((*json)["method"].asString() == "delete") {
    db->execSqlAsync(
        "UPDATE public_info SET \"delete\" = 1, update_user "
        "= $1, update_date = $2 WHERE \"id\" = $3",
        [=](const orm::Result &r) {
          Json::Value ret;

          if (r.affectedRows() > 0) {
            ret["message"] = "success";
            ret["status"] = 1;
          } else {
            ret["message"] = "更新失败";
            ret["status"] = 0;
          }

          callback(HttpResponse::newHttpJsonResponse(ret));
        },
        [=](const orm::DrogonDbException &e) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = e.base().what();
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        },
        req->getParameter("user"), timestamp, req->getParameter("id"));
  } else {
    Json::Value ret;
    ret["message"] = "方法无效";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}
} // namespace api
