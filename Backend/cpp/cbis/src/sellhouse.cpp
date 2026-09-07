#include "sellhouse.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void SellHouse::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM sellhouse WHERE \"delete\" = 0";

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

  if ((*json).find("contact"))
    stmt = fmt::format("{} AND contact LIKE '%{}%'", stmt,
                       (*json)["contact"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

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

void SellHouse::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM sellhouse WHERE \"delete\" = 0";

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

  if ((*json).find("contact"))
    stmt = fmt::format("{} AND contact LIKE '%{}%'", stmt,
                       (*json)["contact"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

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
        info["title"] = it["title"].as<std::string>();
        info["contact"] = it["contact"].as<std::string>();
        info["phone"] = it["phone"].as<std::string>();
        info["zone"] = it["zone"].as<std::string>();
        info["building"] = it["building"].as<std::string>();
        info["room"] = it["room"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["remark"] = it["remark"].as<std::string>();
        info["preview"] = it["preview"].as<std::string>();
        info["price"] = it["price"].as<std::string>();
        info["area"] = it["area"].as<std::string>();
        info["amount"] = it["amount"].as<std::string>();
        info["create_date"] = it["create_date"].as<std::string>();
        info["create_user"] =
            co_await User::get_user(it["create_user"].as<std::string>());
        info["update_date"] = it["update_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(it["update_user"].as<std::string>());
        info["delete"] = it["delete"].as<int>();
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

void SellHouse::info(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback,
                     const std::string &id) const {
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM sellhouse "
                                        "WHERE \"id\" = $1 AND \"delete\" = 0",
                                        id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["title"] = row["title"].as<std::string>();
        info["contact"] = row["contact"].as<std::string>();
        info["phone"] = row["phone"].as<std::string>();
        info["zone"] = row["zone"].as<std::string>();
        info["building"] = row["building"].as<std::string>();
        info["room"] = row["room"].as<std::string>();
        info["status"] = row["status"].as<std::string>();
        info["remark"] = row["remark"].as<std::string>();
        info["preview"] = row["preview"].as<std::string>();
        info["price"] = row["price"].as<std::string>();
        info["area"] = row["area"].as<std::string>();
        info["amount"] = row["amount"].as<std::string>();
        info["create_date"] = row["create_date"].as<std::string>();
        info["create_user"] =
            co_await User::get_user(row["create_user"].as<std::string>());
        info["update_date"] = row["update_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(row["update_user"].as<std::string>());
        info["delete"] = row["delete"].as<int>();
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
void SellHouse::price(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &zone, const std::string &building,
                      const std::string &room) const {
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r =
          co_await db->execSqlCoro("SELECT * FROM sellhouse "
                                   "WHERE \"zone\" = $1 AND \"building\" = $2 "
                                   "AND \"room\" = $3 AND\"delete\" = 0",
                                   zone, building, room);

      Json::Value ret;
      auto row = r[0];

      Json::Value info;

      if (r.size() > 0) {
        info["price"] = row["price"].as<std::string>();
        info["area"] = row["area"].as<std::string>();
        info["amount"] = row["amount"].as<std::string>();
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

void SellHouse::public_list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  async_run([=]() -> Task<> {
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    if (!json) {
      Json::Value ret;
      ret["message"] = "参数错误";

      ret["status"] = 0;
      co_return callback(HttpResponse::newHttpJsonResponse(ret));
    }

    std::string stmt =
        "SELECT * FROM sellhouse WHERE \"delete\" = 0 AND \"status\" = '在售'";

    if ((*json).find("title"))
      stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                         (*json)["title"].asString());

    if ((*json).find("zone"))
      stmt = fmt::format("{} AND zone LIKE '%{}%'", stmt,
                         (*json)["zone"].asString());

    if ((*json).find("building"))
      stmt = fmt::format("{} AND building LIKE '%{}%'", stmt,
                         (*json)["building"].asString());

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
          info["room"] = it["room"].as<std::string>();
          infos.append(info);
        }

        ret["data"] = infos;
        ret["message"] = "success";
        ret["status"] = 1;
        callback(HttpResponse::newHttpJsonResponse(ret));
      } catch (const std::exception &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      }
    });
  });
}

void SellHouse::sell(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto redisClient = app().getRedisClient("redis-slave");
  redisClient->execCommandAsync(
      [=](const nosql::RedisResult &r) {
        if (r.asString() == (*json)["verify"].asString()) {
          auto timestamp = common::utility::timeNow();

          auto db = app().getDbClient("postgresql-slave");
          db->execSqlAsync(
              "SELECT id FROM sellhouse WHERE phone = $1 AND \"delete\" = 0",
              [=](const orm::Result &r) {
                if (r.size() > 0) {
                  Json::Value ret;
                  ret["message"] = "手机号已预订";
                  ret["status"] = 0;
                  callback(HttpResponse::newHttpJsonResponse(ret));
                } else {
                  auto db = app().getDbClient("postgresql-master");
                  db->execSqlAsync(
                      "UPDATE sellhouse SET contact = $1, phone = $2, "
                      "\"status\" "
                      "= $3, remark = $4, update_user = $5, update_date = $6 "
                      "WHERE title = $7 AND zone = $8 AND building = $9 AND "
                      "room = $10 AND \"delete\" = 0",
                      [=](const orm::Result &r) {
                        if (r.affectedRows() > 0) {
                          Json::Value client_req;
                          client_req["to"] = (*json)["phone"].asString();
                          client_req["templateId"] =
                              "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
                          client_req["type"] = "cm";
                          client_req["msg"] = fmt::format(
                              "[\"{}{}{}\"]", (*json)["zone"].asString(),
                              (*json)["building"].asString(),
                              (*json)["room"].asString());
                          client_req["user"] = "0";

                          auto ar = cpr::PostAsync(
                              cpr::Url{"https://erp.example.com/api/route/"
                                       "post?auth=key&service=notify&"
                                       "path=/api/sms/custom_send"},
                              cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                              cpr::Header{
                                  {"Content-type", "application/json"},
                                  {"Authorization",
                                   app()
                                       .getCustomConfig()["auth"]["m2m"]["key"]
                                       .asString()}},
                              cpr::UserAgent{"CaNplay/1.0"},
                              cpr::Timeout{30000},
                              cpr::Body{client_req.toStyledString()});
                          auto res = ar.get();

                          if (res.status_code == 200) {
                            Json::Reader reader;
                            Json::Value body;
                            if (reader.parse(res.text, body)) {
                              Json::Value ret;
                              ret["message"] = "success";
                              ret["status"] = 1;
                              callback(HttpResponse::newHttpJsonResponse(ret));
                            } else {
                              Json::Value ret;
                              ret["message"] = "调用短信发送失败";
                              ret["status"] = 0;
                              callback(HttpResponse::newHttpJsonResponse(ret));
                            }
                          } else {
                            Json::Value ret;
                            ret["message"] = "短信发送失败";
                            ret["status"] = 0;
                            callback(HttpResponse::newHttpJsonResponse(ret));
                          }
                        } else {
                          Json::Value ret;
                          ret["message"] = "更新失败";
                          ret["status"] = 0;
                          callback(HttpResponse::newHttpJsonResponse(ret));
                        }
                      },
                      [=](const orm::DrogonDbException &e) {
                        common::utility::log(
                            common::utility::LOGLEVEL::info, false,
                            "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

                        Json::Value ret;
                        ret["message"] = e.base().what();
                        ret["status"] = 0;

                        callback(HttpResponse::newHttpJsonResponse(ret));
                      },
                      (*json)["contact"].asString(),
                      (*json)["phone"].asString(), (*json)["status"].asString(),
                      (*json)["remark"].asString(), (*json)["user"].asString(),
                      timestamp, (*json)["title"].asString(),
                      (*json)["zone"].asString(),
                      (*json)["building"].asString(),
                      (*json)["room"].asString());
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
              },
              (*json)["phone"].asString());
        } else if (r.isNil() || r.asString().empty()) {
          Json::Value ret;
          ret["message"] = "未发送验证码";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        } else {
          Json::Value ret;
          ret["message"] = "验证码错误";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const std::exception &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.what();
        ret["status"] = 0;
        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      "GET sms:%s", (*json)["phone"].asString().c_str());
}

void SellHouse::update(
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
        "INSERT INTO sellhouse VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, "
        "$10, $11, $12, $13, $14, $15, $16, $17, 0, null)",
        [=](const orm::Result &r) {
          Json::Value ret;
          ret["message"] = "success";
          ret["status"] = 1;
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
        common::utility::uuid(), (*json)["title"].asString(),
        (*json)["contact"].asString(), (*json)["phone"].asString(),
        (*json)["zone"].asString(), (*json)["building"].asString(),
        (*json)["room"].asString(), (*json)["status"].asString(),
        (*json)["remark"].asString(), (*json)["preview"].asString(),
        (*json)["price"].asString(), (*json)["area"].asString(),
        (*json)["amount"].asString(), (*json)["user"].asString(), timestamp,
        (*json)["user"].asString(), timestamp);
  } else if ((*json)["method"].asString() == "update") {
    db->execSqlAsync(
        "UPDATE sellhouse SET title = $1, contact = $2, phone = $3, zone = $4, "
        "building = $5, room = $6, status = $7, remark = $8, preview = $9, "
        "price = $10, area = $11, amount = $12, additional = $13, update_user "
        "= $14, update_date = $15 WHERE \"id\" = $16",
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
        (*json)["title"].asString(), (*json)["contact"].asString(),
        (*json)["phone"].asString(), (*json)["zone"].asString(),
        (*json)["building"].asString(), (*json)["room"].asString(),
        (*json)["status"].asString(), (*json)["remark"].asString(),
        (*json)["preview"].asString(), (*json)["price"].asString(),
        (*json)["area"].asString(), (*json)["amount"].asString(),
        (*json)["additional"].asString(), (*json)["user"].asString(), timestamp,
        (*json)["id"].asString());
  } else if ((*json)["method"].asString() == "delete") {
    db->execSqlAsync(
        "UPDATE sellhouse SET \"delete\" = 1, update_user "
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
        (*json)["user"].asString(), timestamp, (*json)["id"].asString());
  } else {
    Json::Value ret;
    ret["message"] = "方法无效";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}
} // namespace api
