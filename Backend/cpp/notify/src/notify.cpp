#include "notify.h"
#include "include/utility.h"
#include "pch.h"

#include <drogon/WebSocketClient.h>

namespace api {
void Notify::count(
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
      fmt::format("SELECT COUNT(id) FROM notify WHERE topic LIKE '%{}%'",
                  (*json)["topic"].asString());

  if ((*json).find("msg"))
    stmt =
        fmt::format("{} AND msg LIKE '%{}%'", stmt, (*json)["msg"].asString());

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

void Notify::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  async_run([=, this]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");

      std::string stmt =
          fmt::format("SELECT * FROM notify WHERE topic LIKE '%{}%'",
                      (*json)["topic"].asString());

      if ((*json).find("content"))
        stmt = fmt::format("{} AND content LIKE '%{}%'", stmt,
                           (*json)["content"].asString());

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
        info["topic"] = it["topic"].as<std::string>();
        info["subscriber"] = it["subscriber"].as<std::string>();
        info["msg"] = it["msg"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["type"] = it["type"].as<std::string>();
        info["create_user"] =
            co_await get_user(it["create_user"].as<std::string>());
        info["create_date"] = it["create_date"].as<std::string>();
        info["update_user"] =
            co_await get_user(it["update_user"].as<std::string>());
        info["update_date"] = it["update_date"].as<std::string>();
        info["delete"] = it["delete"].as<bool>();
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

void Notify::info(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  const std::string &id) const {
  async_run([=, this]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");

      std::string stmt =
          fmt::format("SELECT * FROM notify WHERE \"id\" = '{}'", id);

      auto r = co_await db->execSqlCoro(stmt);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["topic"] = row["topic"].as<std::string>();
        info["subscriber"] = row["subscriber"].as<std::string>();
        info["content"] = row["content"].as<std::string>();
        info["status"] = row["status"].as<std::string>();
        info["type"] = row["type"].as<std::string>();
        info["create_user"] =
            co_await get_user(row["create_user"].as<std::string>());
        info["create_date"] = row["create_date"].as<std::string>();
        info["update_user"] =
            co_await get_user(row["update_user"].as<std::string>());
        info["update_date"] = row["update_date"].as<std::string>();
        info["delete"] = row["delete"].as<bool>();
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

void Notify::public_count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = fmt::format("SELECT COUNT(id) FROM notify "
                                 "WHERE topic = '{}' AND \"delete\" = 0",
                                 (*json)["topic"].asString());

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

void Notify::public_list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  async_run([=, this]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");

      std::string stmt = fmt::format("SELECT * FROM notify WHERE topic = '{}' "
                                     "AND \"delete\" = 0",
                                     (*json)["topic"].asString());

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
        info["topic"] = it["topic"].as<std::string>();
        info["subscriber"] = it["subscriber"].as<std::string>();
        info["msg"] = it["msg"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["type"] = it["type"].as<std::string>();
        info["create_user"] =
            co_await get_user(it["create_user"].as<std::string>());
        info["create_date"] = it["create_date"].as<std::string>();
        info["update_user"] =
            co_await get_user(it["update_user"].as<std::string>());
        info["update_date"] = it["update_date"].as<std::string>();
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

void Notify::public_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    const std::string &id) const {
  async_run([=, this]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");

      std::string stmt =
          fmt::format("SELECT * FROM notify WHERE \"id\" = '{}'", id);

      auto r = co_await db->execSqlCoro(stmt);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["topic"] = row["topic"].as<std::string>();
        info["subscriber"] = row["subscriber"].as<std::string>();
        info["content"] = row["content"].as<std::string>();
        info["status"] = row["status"].as<std::string>();
        info["type"] = row["type"].as<std::string>();
        info["create_user"] =
            co_await get_user(row["create_user"].as<std::string>());
        info["create_date"] = row["create_date"].as<std::string>();
        info["update_user"] =
            co_await get_user(row["update_user"].as<std::string>());
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

void Notify::update(
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

  if ((*json)["method"].asString() == "add") {
    if ((*json)["type"].asString() != "wx_water") {
      async_run([=]() -> Task<> {
        auto ws = WebSocketClient::newWebSocketClient("ws://127.0.0.1:51560");
        auto wsreq = HttpRequest::newHttpRequest();
        wsreq->setPath(fmt::format(
            "/api/ws/notify?auth=key&token={}&topic={}",
            app().getCustomConfig()["auth"]["m2m"]["key"].asString(),
            (*json)["topic"].asString()));

        ws->setMessageHandler([](const std::string &message,
                                 const WebSocketClientPtr &,
                                 const WebSocketMessageType &type) {
          std::string messageType = "Unknown";
          if (type == WebSocketMessageType::Text)
            messageType = "text";
          else if (type == WebSocketMessageType::Pong)
            messageType = "pong";
          else if (type == WebSocketMessageType::Ping)
            messageType = "ping";
          else if (type == WebSocketMessageType::Binary)
            messageType = "binary";
          else if (type == WebSocketMessageType::Close)
            messageType = "Close";
        });

        ws->setConnectionClosedHandler(
            [](const WebSocketClientPtr &conn) { conn->stop(); });

        ws->connectToServer(wsreq, [&](ReqResult r, const HttpResponsePtr &,
                                       const WebSocketClientPtr &conn) {
          if (r != ReqResult::Ok) {
            common::utility::log(
                common::utility::LOGLEVEL::info, false,
                "Failed to establish WebSocket connection, {}:{}", __FILE__,
                __LINE__);

            conn->stop();

            Json::Value ret;
            ret["message"] = "启用通知失败";
            ret["status"] = 0;

            return callback(HttpResponse::newHttpJsonResponse(ret));
          }

          Json::Value msg;
          msg["topic"] = (*json)["topic"].asString();
          msg["type"] = (*json)["type"].asString();
          msg["subscriber"] = (*json)["subscriber"].asString();
          msg["msg"] = (*json)["msg"].asString();
          conn->getConnection()->send(msg.toStyledString());

          auto db = app().getDbClient("postgresql-master");
          db->execSqlAsync(
              "INSERT INTO notify VALUES ($1, $2, $3, $4, "
              "'unread', $5, $6, $7, $8, false, $9)",
              [=](const orm::Result &r) {
                Json::Value ret;

                if (r.affectedRows() > 0) {
                  ret["message"] = "success";
                  ret["status"] = 1;
                } else {
                  ret["message"] = "新增失败";
                  ret["status"] = 0;
                }

                return callback(HttpResponse::newHttpJsonResponse(ret));
              },
              [=](const orm::DrogonDbException &e) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "{} - {}:{}", e.base().what(), __FILE__,
                                     __LINE__);

                Json::Value ret;
                ret["message"] = e.base().what();
                ret["status"] = 0;

                return callback(HttpResponse::newHttpJsonResponse(ret));
              },
              common::utility::uuid(), (*json)["topic"].asString(),
              (*json)["subscriber"].asString(), (*json)["msg"].asString(),
              (*json)["user"].asString(), timestamp, (*json)["user"].asString(),
              timestamp, (*json)["type"].asString());
        });

        ws->getLoop()->runAfter(15, [=]() { ws->stop(); });

        co_return;
      });
    } else {
      async_run([=, this]() -> Task<> {
        auto token = get_wx_access_token();
        if (token.empty()) {
          Json::Value ret;
          ret["message"] = "获取授权令牌失败";
          ret["status"] = 0;
          co_return callback(HttpResponse::newHttpJsonResponse(ret));
        }

        auto ar = cpr::GetAsync(
            cpr::Url{fmt::format("https://api.weixin.qq.com/cgi-bin/user/"
                                 "get?access_token={}&next_openid=",
                                 token)},
            cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
            cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
        auto res = ar.get();

        if (res.status_code == 200) {
          // std::string client_req = fmt::format(
          //     "{{\"touser\":\"o5c6A6S2wEHA58N95MLourb5ddq0\",\"template_id\":"
          //     "\"2j7EOMMpe8BOuAWGu-FFIH9sFWShFTFakpXI-w2WsIc\",\"data\":{{\"time1\":{"
          //     "{\"value\":\"2023-11-2514:00:00\"}},\"thing2\":{{\"value\":"
          //     "\"示例站点A、公安大厦-盘龙示例站点E\"}},\"thing3\":{{\"value\":"
          //     "\"管道维护\"}},\"thing4\":{{\"value\":\"这是备注信息\"}}}}}}");

          // auto ar = cpr::PostAsync(
          //     cpr::Url{fmt::format("https://api.weixin.qq.com/cgi-bin/message/"
          //                          "subscribe/bizsend?access_token={}",
          //                          token)},
          //     cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
          //     cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
          //     cpr::Body{client_req});
          // auto res = ar.get();

          // Json::Value ret;
          // ret["message"] = res.text;
          // ret["status"] = 1;
          // return callback(HttpResponse::newHttpJsonResponse(ret));

          Json::Reader reader;
          Json::Value body;
          if (reader.parse(res.text, body)) {
            wx_send(token, body["openid"], (*json)["time"].asString(),
                    (*json)["location"].asString(),
                    (*json)["reason"].asString(), (*json)["remark"].asString(),
                    body["next_openid"].asString());

            auto db = app().getDbClient("postgresql-master");
            db->execSqlAsync(
                "INSERT INTO notify VALUES ($1, $2, $3, $4, "
                "'read', $5, $6, $7, $8, false, $9)",
                [=](const orm::Result &r) {
                  Json::Value ret;

                  if (r.affectedRows() > 0) {
                    ret["message"] = "success";
                    ret["status"] = 1;
                  } else {
                    ret["message"] = "新增微信通知失败";
                    ret["status"] = 0;
                  }

                  return callback(HttpResponse::newHttpJsonResponse(ret));
                },
                [=](const orm::DrogonDbException &e) {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "{} - {}:{}", e.base().what(), __FILE__,
                                       __LINE__);

                  Json::Value ret;
                  ret["message"] = e.base().what();
                  ret["status"] = 0;

                  return callback(HttpResponse::newHttpJsonResponse(ret));
                },
                common::utility::uuid(), (*json)["topic"].asString(),
                (*json)["subscriber"].asString(),
                fmt::format(
                    "停水时间: {} | 停水区域: {} | 停水原因: {} | 温馨提示: {}",
                    (*json)["time"].asString(), (*json)["location"].asString(),
                    (*json)["reason"].asString(), (*json)["remark"].asString()),
                (*json)["user"].asString(), timestamp,
                (*json)["user"].asString(), timestamp,
                (*json)["type"].asString());

            Json::Value ret;
            ret["message"] = "success";
            ret["status"] = 1;
            co_return callback(HttpResponse::newHttpJsonResponse(ret));
          }
        }
      });
    }
  }

  Json::Value ret;
  ret["message"] = "参数错误";
  ret["status"] = 0;
  callback(HttpResponse::newHttpJsonResponse(ret));
}

Task<Json::Value> Notify::get_user(const std::string &id) const {
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

void Notify::wx_send(std::string token, Json::Value data, std::string time,
                     std::string location, std::string reason,
                     std::string remark, std::string next_openid) const {
  for (int n = 0; n < data["total"].asInt(); n++) {
    for (int i = 0; i < data["count"].asInt(); i++) {
      std::string user = data["openid"][i].asString();

      Json::Value client_req;
      client_req["touser"] = user;
      client_req["template_id"] = "2j7EOMMpe8BOuAWGu-FFIH9sFWShFTFakpXI-w2WsIc";
      Json::Value data;
      Json::Value value;
      value["value"] = time;
      data["time1"] = value;
      value["value"] = location;
      data["thing2"] = value;
      value["value"] = reason;
      data["thing3"] = value;
      value["value"] = remark;
      data["thing4"] = value;
      client_req["data"] = data;

      auto ar = cpr::PostAsync(
          cpr::Url{fmt::format("https://api.weixin.qq.com/cgi-bin/message/"
                               "subscribe/bizsend?access_token={}",
                               token)},
          cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
          cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
          cpr::Body{data.toStyledString()});
    }
  }

  auto ar = cpr::GetAsync(
      cpr::Url{fmt::format("https://api.weixin.qq.com/cgi-bin/user/"
                           "get?access_token={}&next_openid={}",
                           token, next_openid)},
      cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
      cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
  auto res = ar.get();

  if (res.status_code == 200) {
    Json::Reader reader;
    Json::Value body;
    if (reader.parse(res.text, body)) {
      wx_send(token, body["openid"], time, location, reason, remark,
              body["next_openid"].asString());
    }
  }
}

std::string Notify::get_wx_access_token() const {
  Json::Value client_req;
  client_req["grant_type"] = "client_credential";
  client_req["appid"] = "wx0000000000000000";
  client_req["secret"] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  client_req["force_refresh"] = false;

  auto ar =
      cpr::PostAsync(cpr::Url{"https://api.weixin.qq.com/cgi-bin/stable_token"},
                     cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                     cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                     cpr::Body{client_req.toStyledString()});
  auto res = ar.get();

  try {
    if (res.status_code == 200) {
      Json::Reader reader;
      Json::Value json_token;
      reader.parse(res.text, json_token);

      if (!json_token["access_token"].isNull()) {
        return json_token["access_token"].asString();
      }
    }
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);
    return "";
  }

  return "";
}
} // namespace api
