#include "user.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void User::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt =
      fmt::format("SELECT COUNT(id) FROM account WHERE \"delete\" = {}",
                  (*json)["isDelete"].asBool());

  if ((*json).find("id"))
    stmt = fmt::format("{} AND \"id\" = '{}'", stmt, (*json)["id"].asString());

  if ((*json).find("username"))
    stmt = fmt::format("{} AND username LIKE '%{}%'", stmt,
                       (*json)["username"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());

  if ((*json).find("nickname"))
    stmt = fmt::format("{} AND nickname LIKE '%{}%'", stmt,
                       (*json)["nickname"].asString());

  if ((*json).find("phone"))
    stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                       (*json)["phone"].asString());

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

void User::signup(
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

  Json::Value addition;

  std::string id = common::utility::uuid();
  std::string password =
      common::utility::bcryptGenerate((*json)["password"].asString());

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT COUNT(id) FROM account WHERE phone = $1",
      [=](const orm::Result &r) {
        Json::Value ret;

        if (r.size() > 0) {
          auto row = r[0];

          if (row[0].as<int>() > 0) {
            ret["message"] = "手机号已存在";
            ret["status"] = 0;
            callback(HttpResponse::newHttpJsonResponse(ret));
          } else {
            db->execSqlAsync(
                "SELECT COUNT(id) FROM account WHERE username = $1",
                [=](const orm::Result &r) {
                  Json::Value ret;

                  if (r.size() > 0) {
                    auto row = r[0];

                    if (row[0].as<int>() > 0) {
                      ret["message"] = "用户已存在";
                      ret["status"] = 0;
                      callback(HttpResponse::newHttpJsonResponse(ret));
                    } else {
                      auto redisClient = app().getRedisClient("redis-slave");
                      redisClient->execCommandAsync(
                          [=](const nosql::RedisResult &r) {
                            if (r.asString() == (*json)["verify"].asString()) {
                              auto db = app().getDbClient("postgresql-master");
                              db->execSqlAsync(
                                  "INSERT INTO account VALUES ($1, $2, $3, '', "
                                  "'', $4, '', '', '', '', "
                                  "'4107e41e-14cd-449c-ba29-d48343abd6d5', $5, "
                                  "$6, "
                                  "'eebf50f2-95a8-4cab-915e-45d58bf697a1', $7, "
                                  "'eebf50f2-95a8-4cab-915e-45d58bf697a1', $8, "
                                  "0, 'admin', 'admin', "
                                  "'b1be135b-a9ed-422c-a18f-2bbe2c56a6b0', $9, "
                                  "$10, '', '', '', '', '', '')",
                                  [=](const orm::Result &r) {
                                    if (r.affectedRows() > 0) {
                                      Json::Value info;
                                      info["id"] = id;
                                      info["username"] =
                                          (*json)["username"].asString();
                                      info["password"] =
                                          (*json)["password"].asString();
                                      info["nickname"] =
                                          (*json)["nickname"].asString();
                                      info["phone"] =
                                          (*json)["phone"].asString();
                                      info["permissions"] =
                                          "4107e41e-14cd-449c-ba29-"
                                          "d48343abd6d5";
                                      info["options"] = "b1be135b-a9ed-422c-"
                                                        "a18f-2bbe2c56a6b0";

                                      const auto &token =
                                          common::utility::jwtGenerate(
                                              app()
                                                  .getCustomConfig()["auth"]
                                                                    ["jwt"]
                                                                    ["issuer"]
                                                  .asString(),
                                              app()
                                                  .getCustomConfig()["auth"]
                                                                    ["jwt"]
                                                                    ["audience"]
                                                  .asString(),
                                              (*json).isMember("remember") &&
                                                  (*json)["remember"].asBool(),
                                              {
                                                  {"id",
                                                   picojson::value(
                                                       row["id"]
                                                           .as<std::string>())},
                                              });
                                      info["token"] = token;

                                      Json::Value ret;
                                      ret["data"] = info;
                                      ret["message"] = "success";
                                      ret["status"] = 1;

                                      callback(
                                          HttpResponse::newHttpJsonResponse(
                                              ret));
                                    } else {
                                      Json::Value ret;
                                      ret["message"] = "注册失败";
                                      ret["status"] = 0;

                                      callback(
                                          HttpResponse::newHttpJsonResponse(
                                              ret));
                                    }
                                  },
                                  [=](const orm::DrogonDbException &e) {
                                    common::utility::log(
                                        common::utility::LOGLEVEL::info, false,
                                        "{} - {}:{}", e.base().what(), __FILE__,
                                        __LINE__);

                                    Json::Value ret;
                                    ret["message"] = "注册失败";
                                    ret["status"] = 0;

                                    callback(
                                        HttpResponse::newHttpJsonResponse(ret));
                                  },
                                  id, (*json)["username"].asString(), password,
                                  (*json)["phone"].asString(), timestamp,
                                  req->peerAddr().toIpPort(), timestamp,
                                  timestamp, (*json)["password"].asString(),
                                  (*json)["nickname"].asString());
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
                            common::utility::log(
                                common::utility::LOGLEVEL::info, false,
                                "{} - {}:{}", e.what(), __FILE__, __LINE__);

                            Json::Value ret;
                            ret["message"] = e.what();
                            ret["status"] = 0;
                            callback(HttpResponse::newHttpJsonResponse(ret));
                          },
                          "GET sms:%s", (*json)["phone"].asString().c_str());
                    }
                  } else {
                    ret["message"] = "注册失败";
                    ret["status"] = 0;
                    callback(HttpResponse::newHttpJsonResponse(ret));
                  }
                },
                [=](const orm::DrogonDbException &e) {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "{} - {}:{}", e.base().what(), __FILE__,
                                       __LINE__);

                  Json::Value ret;
                  ret["message"] = "注册失败";
                  ret["status"] = 0;

                  callback(HttpResponse::newHttpJsonResponse(ret));
                },
                (*json)["username"].asString());
          }
        } else {
          ret["message"] = "注册失败";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = "注册失败";
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["phone"].asString());
}

void User::wechat(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto ar = cpr::GetAsync(
      cpr::Url{fmt::format("https://api.weixin.qq.com/sns/oauth2/"
                           "access_token?appid=wx0000000000000000&secret="
                           "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA&code={}&"
                           "grant_type=authorization_code",
                           (*json)["code"].asString())},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      auto ar = cpr::GetAsync(
          cpr::Url{fmt::format("https://api.weixin.qq.com/sns/oauth2/sns/"
                               "userinfo?access_token={}&openid={}&lang=zh_CN",
                               body["access_token"].asString(),
                               body["openid"].asString())},
          cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
          cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
      auto res = ar.get();

      if (res.status_code == 200) {
        if (reader.parse(res.text, body)) {
          Json::Value json;
          json["msg"] = body;
          json["status"] = 1;
          return callback(HttpResponse::newHttpJsonResponse(json));
        }
      }
    }
  }

  Json::Value ret;
  ret["message"] = "调用失败";
  ret["status"] = 0;
  callback(HttpResponse::newHttpJsonResponse(ret));
}

void User::signin(
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

  if ((*json)["type"].asString() == "wechat") {
    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "SELECT * FROM account WHERE wechat = $1 AND "
        "\"delete\" = 0",
        [=](const orm::Result &r) {
          if (r.size() > 0) {
            auto row = r[0];

            Json::Value info;
            info["id"] = row["id"].as<std::string>();
            info["wechat"] = row["wechat"].as<std::string>();
            info["plate"] = row["plate"].as<std::string>();
            info["image"] = row["image"].as<std::string>();
            info["level"] = row["level"].as<int64_t>();
            info["phone"] = row["phone"].as<int64_t>();
            info["money"] = row["money"].as<int64_t>();
            info["score"] = row["score"].as<int64_t>();
            info["hik_id"] = row["hik_id"].as<int64_t>();
            info["jc_numbering"] = row["jc_numbering"].as<std::string>();

            const auto &token = common::utility::jwtGenerate(
                app().getCustomConfig()["auth"]["jwt"]["issuer"].asString(),
                app().getCustomConfig()["auth"]["jwt"]["audience"].asString(),
                (*json).isMember("remember") && (*json)["remember"].asBool(),
                {
                    {"id", picojson::value(row["id"].as<std::string>())},
                });
            info["token"] = token;

            Json::Value ret;
            ret["data"] = info;
            ret["message"] = "success";
            ret["status"] = 1;

            callback(HttpResponse::newHttpJsonResponse(ret));
          } else {
            Json::Value addition;

            std::string id = common::utility::uuid();
            std::string wechat = (*json)["openid"].asString();
            std::string headimg = (*json)["headimgurl"].asString();

            auto db = app().getDbClient("postgresql-master");
            db->execSqlAsync(
                "INSERT INTO gzcw_pay VALUES ($1, $2, $3, 'admin', 'admin', 0, "
                "0, $4, 0, 0, '', $5, 0, 0, '', '', 0, -1, '', $6)",
                [=](const orm::Result &r) {
                  Json::Value info;
                  info["id"] = id;
                  info["wechat"] = wechat;
                  info["plate"] = "";
                  info["image"] = headimg;
                  info["level"] = 0;
                  info["phone"] = 0;
                  info["money"] = 0;
                  info["score"] = 0;
                  info["hik_id"] = -1;
                  info["jc_numbering"] = "";

                  auto ar = cpr::PostAsync(
                      cpr::Url{"http://localhost:51530/api/auth/request"},
                      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
                  auto res = ar.get();

                  if (res.status_code == 200) {
                    Json::Reader reader;
                    Json::Value body;
                    if (reader.parse(res.text, body)) {
                      Json::Value r = body["msg"];

                      if (r["msg"] == "success") {
                        Json::Value msg;
                        msg["info"] = info;
                        msg["token"] = r["token"];

                        Json::Value ret;
                        ret["data"] = msg;
                        ret["message"] = "success";
                        ret["status"] = 1;

                        return callback(HttpResponse::newHttpJsonResponse(ret));
                      }
                    }
                  }

                  Json::Value ret;
                  ret["message"] = "调用失败";
                  ret["status"] = 0;

                  callback(HttpResponse::newHttpJsonResponse(ret));
                },
                [=](const orm::DrogonDbException &e) {
                  Json::Value ret;
                  ret["message"] = "账号错误";
                  ret["status"] = 0;

                  callback(HttpResponse::newHttpJsonResponse(ret));
                },
                id, timestamp, timestamp, wechat, headimg, addition);
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
        (*json)["openid"].asString());
  } else if ((*json)["type"].asString() == "phone") {
    try {
      auto redisClient = app().getRedisClient("redis-slave");
      redisClient->execCommandAsync(
          [=](const nosql::RedisResult &r) {
            if (r.asString() == (*json)["verify"].asString()) {
              auto db = app().getDbClient("postgresql-slave");
              db->execSqlAsync(
                  "SELECT * FROM account WHERE phone = $1 AND "
                  "\"delete\" = 0",
                  [=](const orm::Result &r) {
                    if (r.size() > 0) {
                      auto row = r[0];

                      auto db = app().getDbClient("postgresql-master");
                      db->execSqlAsync(
                          "UPDATE account SET last_login = $1, last_ip "
                          "= $2 WHERE \"id\" = $3",
                          [=](const orm::Result &r) {},
                          [=](const orm::DrogonDbException &e) {
                            common::utility::log(
                                common::utility::LOGLEVEL::info, false,
                                "{} - {}:{}", e.base().what(), __FILE__,
                                __LINE__);
                          },
                          timestamp, req->getHeader("X-Forwarded-For"),
                          row["id"].as<std::string>());

                      Json::Value info;
                      info["id"] = row["id"].as<std::string>();
                      info["username"] = row["username"].as<std::string>();
                      info["password"] =
                          row["password_orgin"].as<std::string>();
                      info["name"] = row["name"].as<std::string>();
                      info["head"] = row["head"].as<std::string>();
                      info["phone"] = row["phone"].as<std::string>();
                      info["wechat"] = row["wechat"].as<std::string>();
                      info["alipay"] = row["alipay"].as<std::string>();
                      info["weibo"] = row["weibo"].as<std::string>();
                      info["email"] = row["email"].as<std::string>();
                      info["permissions"] =
                          row["permissions"].as<std::string>();
                      info["options"] = row["options"].as<std::string>();
                      info["nickname"] = row["nickname"].as<std::string>();
                      info["sex"] = row["sex"].as<std::string>();
                      info["province"] = row["province"].as<std::string>();
                      info["city"] = row["city"].as<std::string>();
                      info["country"] = row["country"].as<std::string>();
                      info["additive"] = row["additive"].as<Json::Value>();

                      const auto &token = common::utility::jwtGenerate(
                          app()
                              .getCustomConfig()["auth"]["jwt"]["issuer"]
                              .asString(),
                          app()
                              .getCustomConfig()["auth"]["jwt"]["audience"]
                              .asString(),
                          (*json).isMember("remember") &&
                              (*json)["remember"].asBool(),
                          {
                              {"id",
                               picojson::value(row["id"].as<std::string>())},
                          });
                      info["token"] = token;

                      redisClient->execCommandAsync(
                          [=](const nosql::RedisResult &r) {},
                          [=](const std::exception &e) {}, "DEL sms:%s",
                          (*json)["phone"].asString().c_str());

                      Json::Value ret;
                      ret["data"] = info;
                      ret["message"] = "success";
                      ret["status"] = 1;

                      callback(HttpResponse::newHttpJsonResponse(ret));
                    } else {
                      Json::Value ret;
                      ret["message"] = "用户不存在";
                      ret["status"] = 0;

                      callback(HttpResponse::newHttpJsonResponse(ret));
                    }
                  },
                  [=](const orm::DrogonDbException &e) {
                    common::utility::log(common::utility::LOGLEVEL::info, false,
                                         "{} - {}:{}", e.base().what(),
                                         __FILE__, __LINE__);

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
    } catch (const std::exception &e) {
      common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                           e.what(), __FILE__, __LINE__);

      Json::Value ret;
      ret["message"] = e.what();
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } else {
    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "SELECT * FROM account WHERE username = $1 AND "
        "\"delete\" = 0",
        [=](const orm::Result &r) {
          if (r.size() > 0) {
            auto row = r[0];

            if (common::utility::bcryptValidate(
                    (*json)["password"].asString(),
                    row["password"].as<std::string>())) {
              auto db = app().getDbClient("postgresql-master");
              db->execSqlAsync(
                  "UPDATE account SET last_login = $1, last_ip "
                  "= $2 WHERE \"id\" = $3",
                  [=](const orm::Result &r) {},
                  [=](const orm::DrogonDbException &e) {}, timestamp,
                  req->getHeader("X-Forwarded-For"),
                  row["id"].as<std::string>());

              Json::Value info;
              info["id"] = row["id"].as<std::string>();
              info["username"] = row["username"].as<std::string>();
              info["password"] = row["password_orgin"].as<std::string>();
              info["name"] = row["name"].as<std::string>();
              info["head"] = row["head"].as<std::string>();
              info["phone"] = row["phone"].as<std::string>();
              info["wechat"] = row["wechat"].as<std::string>();
              info["alipay"] = row["alipay"].as<std::string>();
              info["weibo"] = row["weibo"].as<std::string>();
              info["email"] = row["email"].as<std::string>();
              info["permissions"] = row["permissions"].as<std::string>();
              info["options"] = row["options"].as<std::string>();
              info["nickname"] = row["nickname"].as<std::string>();
              info["sex"] = row["sex"].as<std::string>();
              info["province"] = row["province"].as<std::string>();
              info["city"] = row["city"].as<std::string>();
              info["country"] = row["country"].as<std::string>();
              info["additive"] = row["additive"].as<Json::Value>();

              const auto &token = common::utility::jwtGenerate(
                  app().getCustomConfig()["auth"]["jwt"]["issuer"].asString(),
                  app().getCustomConfig()["auth"]["jwt"]["audience"].asString(),
                  (*json).isMember("remember") && (*json)["remember"].asBool(),
                  {
                      {"id", picojson::value(row["id"].as<std::string>())},
                  });
              info["token"] = token;

              Json::Value ret;
              ret["data"] = info;
              ret["message"] = "success";
              ret["status"] = 1;

              callback(HttpResponse::newHttpJsonResponse(ret));
            } else {
              Json::Value ret;
              ret["message"] = "密码错误";
              ret["status"] = 0;

              callback(HttpResponse::newHttpJsonResponse(ret));
            }
          } else {
            Json::Value ret;
            ret["message"] = "用户不存在";
            ret["status"] = 0;

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
        },
        (*json)["username"].asString());
  }
}

void User::signout(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   const std::string &id) const {
  auto timestamp = common::utility::timeNow();

  auto db = app().getDbClient("postgresql-master");
  db->execSqlAsync(
      "UPDATE account SET last_login = $1, last_ip = $2 WHERE "
      "\"id\" = $3",
      [=](const orm::Result &r) {
        Json::Value ret;
        ret["message"] = "success";
        ret["status"] = 1;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;
        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      timestamp, req->getHeader("X-Forwarded-For"), id);
}

void User::info(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                const std::string &id) const {
  async_run([=]() -> Task<> {
    try {
      auto redisClient = app().getRedisClient("redis-slave");
      auto redis_result =
          co_await redisClient->execCommandCoro("GET userinfo:%s", id.c_str());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(
            "SELECT * FROM account WHERE \"id\" = $1", id);

        if (r.size() > 0) {
          auto row = r[0];

          Json::Value user;
          user["id"] = row["id"].as<std::string>();
          user["username"] = row["username"].as<std::string>();
          user["password"] = row["password_orgin"].as<std::string>();
          user["name"] = row["name"].as<std::string>();
          user["head"] = row["head"].as<std::string>();
          user["phone"] = row["phone"].as<std::string>();
          user["wechat"] = row["wechat"].as<std::string>();
          user["alipay"] = row["alipay"].as<std::string>();
          user["weibo"] = row["weibo"].as<std::string>();
          user["email"] = row["email"].as<std::string>();
          user["last_login"] = row["last_login"].as<std::string>();
          user["last_ip"] = row["last_ip"].as<std::string>();
          user["delete"] = row["delete"].as<bool>();
          user["create_user"] = row["create_user"].as<std::string>();
          user["create_date"] = row["create_date"].as<std::string>();
          user["update_user"] = row["update_user"].as<std::string>();
          user["update_date"] = row["update_date"].as<std::string>();
          user["permissions"] = row["permissions"].as<std::string>();
          user["options"] = row["options"].as<std::string>();
          user["nickname"] = row["nickname"].as<std::string>();
          user["sex"] = row["sex"].as<std::string>();
          user["province"] = row["province"].as<std::string>();
          user["city"] = row["city"].as<std::string>();
          user["country"] = row["country"].as<std::string>();
          user["additive"] = row["additive"].as<Json::Value>();
          user["idcard"] = row["idcard"].as<std::string>();

          r = co_await db->execSqlCoro(
              "SELECT * FROM permissions WHERE \"id\" = $1",
              row["permissions"].as<std::string>());

          if (r.size() > 0) {
            auto row1 = r[0];
            Json::Value permissions;
            permissions["id"] = row1["id"].as<std::string>();
            permissions["name"] = row1["name"].as<std::string>();
            permissions["access"] = row1["access"].as<Json::Value>();

            r = co_await db->execSqlCoro(
                "SELECT * FROM options WHERE \"id\" = $1",
                row["options"].as<std::string>());

            if (r.size() > 0) {
              auto row2 = r[0];

              Json::Value options;
              options["id"] = row2["id"].as<std::string>();
              options["name"] = row2["name"].as<std::string>();
              options["options"] = row2["options"].as<Json::Value>();
              options["level"] = row2["level"].as<int64_t>();

              Json::Value info;
              info["user"] = user;
              info["permissions"] = permissions;
              info["options"] = options;

              auto redisClient = app().getRedisClient("redis-master");
              co_await redisClient->execCommandCoro(
                  "SET userinfo:%s %s EX 600", id.c_str(),
                  Json::FastWriter().write(info).c_str());

              Json::Value ret;
              ret["data"] = info;
              ret["message"] = "success";
              ret["status"] = 1;

              callback(HttpResponse::newHttpJsonResponse(ret));
            } else {
              Json::Value ret;
              ret["message"] = "无可用设置";
              ret["status"] = 0;

              callback(HttpResponse::newHttpJsonResponse(ret));
            }
          } else {
            Json::Value ret;
            ret["message"] = "无可用权限";
            ret["status"] = 0;

            callback(HttpResponse::newHttpJsonResponse(ret));
          }
        } else {
          Json::Value ret;
          ret["message"] = "无有效用户";
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      } else {
        Json::Value info;
        Json::Reader reader;
        if (reader.parse(redis_result.asString(), info)) {
          Json::Value ret;
          ret["data"] = info;
          ret["message"] = "success";
          ret["status"] = 1;
          callback(HttpResponse::newHttpJsonResponse(ret));
        } else {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "user info redis error - {}:{}", __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = "获取用户信息缓存失败";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
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

void User::public_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  async_run([=]() -> Task<> {
    try {
      std::shared_ptr<Json::Value> json = req->getJsonObject();
      if (!json) {
        Json::Value ret;
        ret["message"] = "参数错误";

        ret["status"] = 0;
        co_return callback(HttpResponse::newHttpJsonResponse(ret));
      }

      auto redisClient = app().getRedisClient("redis-slave");
      auto redis_result = co_await redisClient->execCommandCoro(
          "GET userinfo:%s", (*json)["username"].asString().c_str());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(
            "SELECT id FROM account WHERE username = $1 AND "
            "phone = $2 LIMIT 1",
            (*json)["username"].asString().c_str(),
            (*json)["phone"].asString().c_str());

        if (r.size() > 0) {
          auto row = r[0];

          auto redisClient = app().getRedisClient("redis-master");
          co_await redisClient->execCommandCoro(
              "SET userinfo:%s %s EX 600",
              (*json)["username"].asString().c_str(),
              row[0].as<std::string>().c_str());

          Json::Value ret;
          ret["data"] = row[0].as<std::string>();
          ret["message"] = "success";
          ret["status"] = 1;
          callback(HttpResponse::newHttpJsonResponse(ret));
        } else {
          Json::Value ret;
          ret["message"] = "无有效用户";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      } else {
        Json::Value ret;
        ret["message"] = redis_result.asString();
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

void User::list(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  async_run([=]() -> Task<> {
    std::string stmt =
        fmt::format("SELECT * FROM account WHERE \"delete\" = {}",
                    (*json)["isDelete"].asBool());

    if ((*json).find("id"))
      stmt =
          fmt::format("{} AND \"id\" = '{}'", stmt, (*json)["id"].asString());

    if ((*json).find("username"))
      stmt = fmt::format("{} AND username LIKE '%{}%'", stmt,
                         (*json)["username"].asString());

    if ((*json).find("name"))
      stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                         (*json)["name"].asString());

    if ((*json).find("nickname"))
      stmt = fmt::format("{} AND nickname LIKE '%{}%'", stmt,
                         (*json)["nickname"].asString());

    if ((*json).find("phone"))
      stmt = fmt::format("{} AND phone LIKE '%{}%'", stmt,
                         (*json)["phone"].asString());

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

    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(stmt);

      if (r.size() > 0) {
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;

          Json::Value user;
          user["id"] = row["id"].as<std::string>();
          user["username"] = row["username"].as<std::string>();
          user["password"] = row["password_orgin"].as<std::string>();
          user["name"] = row["name"].as<std::string>();
          user["head"] = row["head"].as<std::string>();
          user["phone"] = row["phone"].as<std::string>();
          user["wechat"] = row["wechat"].as<std::string>();
          user["alipay"] = row["alipay"].as<std::string>();
          user["weibo"] = row["weibo"].as<std::string>();
          user["email"] = row["email"].as<std::string>();
          user["last_login"] = row["last_login"].as<std::string>();
          user["last_ip"] = row["last_ip"].as<std::string>();
          user["delete"] = row["delete"].as<bool>();
          user["create_user"] =
              co_await get_user(row["create_user"].as<std::string>());
          user["create_date"] = row["create_date"].as<std::string>();
          user["update_user"] =
              co_await get_user(row["update_user"].as<std::string>());
          user["update_date"] = row["update_date"].as<std::string>();
          user["permissions"] = row["permissions"].as<std::string>();
          user["options"] = row["options"].as<std::string>();
          user["nickname"] = row["nickname"].as<std::string>();
          user["sex"] = row["sex"].as<std::string>();
          user["province"] = row["province"].as<std::string>();
          user["city"] = row["city"].as<std::string>();
          user["country"] = row["country"].as<std::string>();
          user["additive"] = row["additive"].as<Json::Value>();
          user["idcard"] = row["idcard"].as<std::string>();
          info["user"] = user;

          auto db = app().getDbClient("postgresql-slave");
          auto r = co_await db->execSqlCoro(
              "SELECT * FROM permissions WHERE \"id\" = $1",
              row["permissions"].as<std::string>());

          if (r.size() > 0) {
            auto row1 = r[0];
            Json::Value permissions;
            permissions["id"] = row1["id"].as<std::string>();
            permissions["name"] = row1["name"].as<std::string>();
            permissions["access"] = row1["access"].as<Json::Value>();

            auto r = co_await db->execSqlCoro(
                "SELECT * FROM options WHERE \"id\" = $1",
                row["options"].as<std::string>());

            if (r.size() > 0) {
              auto row2 = r[0];

              Json::Value options;
              options["id"] = row2["id"].as<std::string>();
              options["name"] = row2["name"].as<std::string>();
              options["options"] = row2["options"].as<Json::Value>();
              options["level"] = row2["level"].as<int64_t>();

              info["permissions"] = permissions;
              info["options"] = options;
            }
          }

          infos.append(info);
        }

        Json::Value ret;
        ret["data"] = infos;
        ret["message"] = "success";
        ret["status"] = 1;

        co_return callback(HttpResponse::newHttpJsonResponse(ret));
      }

      Json::Value ret;
      ret["message"] = "无有效用户信息";
      ret["status"] = 0;

      co_return callback(HttpResponse::newHttpJsonResponse(ret));
    } catch (const std::exception &e) {
      common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                           e.what(), __FILE__, __LINE__);

      Json::Value ret;
      ret["message"] = e.what();
      ret["status"] = 0;

      co_return callback(HttpResponse::newHttpJsonResponse(ret));
    }
  });
}

void User::password(
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

  std::string password =
      common::utility::bcryptGenerate((*json)["password"].asString());

  auto db = app().getDbClient("postgresql-master");
  db->execSqlAsync(
      "UPDATE account SET password = $1, password_orgin = $2, "
      "update_user = $3, update_date = $4 WHERE \"id\" = $5",
      [=](const orm::Result &r) {
        if (r.affectedRows() > 0) {
          async_run([=]() -> Task<> {
            co_await get_user((*json)["id"].asString());
            Json::Value ret;
            ret["message"] = "success";
            ret["status"] = 1;
            callback(HttpResponse::newHttpJsonResponse(ret));
          });
        } else {
          Json::Value ret;
          ret["message"] = "更新失败";
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      password, (*json)["password"].asString(),
      (*json)["update_user"].as<std::string>(), timestamp,
      (*json)["id"].as<std::string>());
}

void User::update(
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

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM account WHERE \"id\" = $1",
      [=](const orm::Result &r) {
        if (r.size() > 0) {
          auto row = r[0];

          std::string tmpPassword =
              row["password_orgin"].isNull()
                  ? ""
                  : row["password_orgin"].as<std::string>();
          std::string tmpName =
              row["name"].isNull() ? "" : row["name"].as<std::string>();
          std::string tmpNickname =
              row["nickname"].isNull() ? "" : row["nickname"].as<std::string>();
          std::string tmpSex =
              row["sex"].isNull() ? "" : row["sex"].as<std::string>();
          std::string tmpEmail =
              row["email"].isNull() ? "" : row["email"].as<std::string>();
          std::string tmpPhone =
              row["phone"].isNull() ? "" : row["phone"].as<std::string>();
          std::string tmpIdcard =
              row["idcard"].isNull() ? "" : row["idcard"].as<std::string>();

          std::string password = (*json)["password"].asString() != ""
                                     ? (*json)["password"].asString()
                                     : tmpPassword;
          std::string name = (*json)["name"].asString() != ""
                                 ? (*json)["name"].asString()
                                 : tmpName;
          std::string nickname = (*json)["nickname"].asString() != ""
                                     ? (*json)["nickname"].asString()
                                     : tmpNickname;
          std::string sex = (*json)["sex"].asString() != ""
                                ? (*json)["sex"].asString()
                                : tmpSex;
          std::string email = (*json)["email"].asString() != ""
                                  ? (*json)["email"].asString()
                                  : tmpEmail;
          std::string phone = (*json)["phone"].asString() != ""
                                  ? (*json)["phone"].asString()
                                  : tmpPhone;
          std::string idcard = (*json)["idcard"].asString() != ""
                                   ? (*json)["idcard"].asString()
                                   : tmpIdcard;

          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "UPDATE account SET password = $1, password_orgin = $2, name = "
              "$3, nickname = $4, sex = $5, email = $6, phone = $7, "
              "update_date = $8, update_user = $9, idcard = $10 WHERE \"id\" = "
              "$11",
              [=](const orm::Result &r) {
                if (r.affectedRows() > 0) {
                  async_run([=]() -> Task<> {
                    co_await get_user((*json)["id"].asString());
                    Json::Value ret;
                    ret["message"] = "success";
                    ret["status"] = 1;
                    callback(HttpResponse::newHttpJsonResponse(ret));
                  });
                } else {
                  Json::Value ret;
                  ret["message"] = "更新失败";
                  ret["status"] = 0;
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
              },
              common::utility::bcryptGenerate(password), password, name,
              nickname, sex, email, phone, timestamp, (*json)["id"].asString(),
              idcard, (*json)["id"].asString());
        } else {
          Json::Value ret;
          ret["message"] = "用户不存在";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["id"].asString());
}

void User::additive(
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

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM account WHERE \"id\" = $1",
      [=](const orm::Result &r) {
        if (r.size() > 0) {
          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "UPDATE account SET additive = $1 WHERE \"id\" = $2",
              [=](const orm::Result &r) {
                if (r.affectedRows() > 0) {
                  async_run([=]() -> Task<> {
                    co_await get_user((*json)["id"].asString());
                    Json::Value ret;
                    ret["message"] = "success";
                    ret["status"] = 1;
                    callback(HttpResponse::newHttpJsonResponse(ret));
                  });
                } else {
                  Json::Value ret;
                  ret["message"] = "更新失败";
                  ret["status"] = 0;
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
              },
              (*json)["additive"].toStyledString(), (*json)["id"].asString());
        } else {
          Json::Value ret;
          ret["message"] = "用户不存在";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["id"].asString());
}

void User::permission(
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

  std::string permissions = "4107e41e-14cd-449c-ba29-d48343abd6d5";

  if ((*json)["code"].asString() == "73L5c") {
    permissions = "ab48e34e-1437-4060-90c7-2db6e4ebce59";
  } else {
    Json::Value ret;
    ret["message"] = "兑换码不正确";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("postgresql-master");
  db->execSqlAsync(
      "UPDATE account SET permissions = $1, update_user = $2, "
      "update_date = $3 WHERE \"id\" = $4",
      [=](const orm::Result &r) {
        if (r.affectedRows() > 0) {
          async_run([=]() -> Task<> {
            co_await get_user((*json)["user"].asString());
            Json::Value ret;
            ret["message"] = "success";
            ret["status"] = 1;
            callback(HttpResponse::newHttpJsonResponse(ret));
          });
        } else {
          Json::Value ret;
          ret["message"] = "更新失败";
          ret["status"] = 0;

          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      permissions, (*json)["user"].asString(), timestamp,
      (*json)["user"].asString());
}

Task<Json::Value> User::get_user(const std::string &id) {
  try {
    auto redisClient = app().getRedisClient("redis-slave");
    auto redis_result =
        co_await redisClient->execCommandCoro("GET userinfo:%s", id.c_str());

    if (redis_result.isNil() || redis_result.asString().empty()) {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(
          "SELECT * FROM account WHERE \"id\" = $1", id);

      if (r.size() > 0) {
        auto row = r[0];

        Json::Value user;
        user["id"] = row["id"].as<std::string>();
        user["username"] = row["username"].as<std::string>();
        user["password"] = row["password_orgin"].as<std::string>();
        user["name"] = row["name"].as<std::string>();
        user["head"] = row["head"].as<std::string>();
        user["phone"] = row["phone"].as<std::string>();
        user["wechat"] = row["wechat"].as<std::string>();
        user["alipay"] = row["alipay"].as<std::string>();
        user["weibo"] = row["weibo"].as<std::string>();
        user["email"] = row["email"].as<std::string>();
        user["last_login"] = row["last_login"].as<std::string>();
        user["last_ip"] = row["last_ip"].as<std::string>();
        user["delete"] = row["delete"].as<bool>();
        user["create_user"] = row["create_user"].as<std::string>();
        user["create_date"] = row["create_date"].as<std::string>();
        user["update_user"] = row["update_user"].as<std::string>();
        user["update_date"] = row["update_date"].as<std::string>();
        user["permissions"] = row["permissions"].as<std::string>();
        user["options"] = row["options"].as<std::string>();
        user["nickname"] = row["nickname"].as<std::string>();
        user["sex"] = row["sex"].as<std::string>();
        user["province"] = row["province"].as<std::string>();
        user["city"] = row["city"].as<std::string>();
        user["country"] = row["country"].as<std::string>();
        user["additive"] = row["additive"].as<Json::Value>();
        user["idcard"] = row["idcard"].as<std::string>();

        r = co_await db->execSqlCoro(
            "SELECT * FROM permissions WHERE \"id\" = $1",
            row["permissions"].as<std::string>());

        if (r.size() > 0) {
          auto row1 = r[0];
          Json::Value permissions;
          permissions["id"] = row1["id"].as<std::string>();
          permissions["name"] = row1["name"].as<std::string>();
          permissions["access"] = row1["access"].as<Json::Value>();

          r = co_await db->execSqlCoro(
              "SELECT * FROM options WHERE \"id\" = $1",
              row["options"].as<std::string>());

          if (r.size() > 0) {
            auto row2 = r[0];

            Json::Value options;
            options["id"] = row2["id"].as<std::string>();
            options["name"] = row2["name"].as<std::string>();
            options["options"] = row2["options"].as<Json::Value>();
            options["level"] = row2["level"].as<int64_t>();

            Json::Value info;
            info["user"] = user;
            info["permissions"] = permissions;
            info["options"] = options;

            auto redisClient = app().getRedisClient("redis-master");
            co_await redisClient->execCommandCoro(
                "SET userinfo:%s %s EX 600", id.c_str(),
                Json::FastWriter().write(info).c_str());

            Json::Value ret;
            ret["data"] = info;
            ret["message"] = "success";
            ret["status"] = 1;

            co_return ret;
          } else {
            Json::Value ret;
            ret["message"] = "无可用设置";
            ret["status"] = 0;

            co_return ret;
          }
        } else {
          Json::Value ret;
          ret["message"] = "无可用权限";
          ret["status"] = 0;

          co_return ret;
        }
      } else {
        Json::Value ret;
        ret["message"] = "无有效用户";
        ret["status"] = 0;

        co_return ret;
      }
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
                             "user info redis error - {}:{}", __FILE__,
                             __LINE__);

        Json::Value ret;
        ret["message"] = "获取用户信息缓存失败";
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
} // namespace api
