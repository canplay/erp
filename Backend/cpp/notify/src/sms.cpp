#include "sms.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Sms::count(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM sms WHERE \"delete\" = 0";

  if ((*json).find("type"))
    stmt = fmt::format("{} AND type LIKE '%{}%'", stmt,
                       (*json)["type"].asString());

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

void Sms::list(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM sms WHERE \"delete\" = 0";

  if ((*json).find("type"))
    stmt = fmt::format("{} AND type LIKE '%{}%'", stmt,
                       (*json)["type"].asString());

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

  async_run([=, this]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(stmt);

      Json::Value ret;
      Json::Value infos;

      for (auto it : r) {
        Json::Value info;
        info["id"] = it["id"].as<std::string>();
        info["to"] = it["to"].as<std::string>();
        info["type"] = it["type"].as<std::string>();
        info["content"] = it["content"].as<std::string>();
        info["remark"] = it["remark"].as<std::string>();
        info["create_date"] = it["create_date"].as<std::string>();
        info["create_user"] =
            co_await get_user(it["create_user"].as<std::string>());
        info["update_date"] = it["update_date"].as<std::string>();
        info["update_user"] =
            co_await get_user(it["update_user"].as<std::string>());
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

void Sms::verify(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  if ((*json)["type"].asString() == "cm") {
    auto timestamp = common::utility::timeNow();

    std::string msg =
        fmt::format("{}", common::utility::random(100000, 999999));

    auto db = app().getDbClient("postgresql-master");
    db->execSqlAsync(
        "INSERT INTO sms VALUES ($1, $2, $3, $4, $5, $6, $7, $8)",
        [=](const orm::Result &r) {
          async_run([=]() -> Task<> {
            try {
              Json::Value client_req;
              client_req["ecName"] = "示例城市A市城市商业运营管理有限公司";
              client_req["apId"] = "wsmas1";
              client_req["templateId"] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
              client_req["mobiles"] = (*json)["to"].asString();
              client_req["params"] = fmt::format("[\"{}\"]", msg);
              client_req["sign"] = "aPGfhhrYe";
              client_req["addSerial"] = "";
              std::string serial = fmt::format(
                  "{}{}{}{}{}{}{}{}", client_req["ecName"].asString(),
                  client_req["apId"].asString(), "Ws123#@!",
                  client_req["templateId"].asString(), (*json)["to"].asString(),
                  fmt::format("[\"{}\"]", msg), client_req["sign"].asString(),
                  client_req["addSerial"].asString());
              client_req["mac"] =
                  common::utility::toLower(common::utility::md5(serial));

              auto ar = cpr::PostAsync(
                  cpr::Url{"http://100.100.100.101:1992/sms/"
                           "tmpsubmit"},
                  cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                  cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                  cpr::Body{common::utility::base64Encode(
                      client_req.toStyledString())});
              auto res = ar.get();

              if (res.status_code == 200) {
                Json::Reader reader;
                Json::Value body;
                if (reader.parse(res.text, body)) {
                  if (body["success"].asBool()) {
                    auto redisClient = app().getRedisClient("redis-master");

                    co_await redisClient->execCommandCoro(
                        "SET sms:%s %s EX 600",
                        (*json)["to"].asString().c_str(), msg.c_str());

                    Json::Value ret;
                    ret["message"] = "success";
                    ret["status"] = 1;
                    callback(HttpResponse::newHttpJsonResponse(ret));
                  }
                }
              }

              Json::Value ret;
              ret["message"] = "调用失败";
              ret["status"] = 0;
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
        common::utility::uuid(), (*json)["to"].asString(),
        (*json)["type"].asString(), msg, timestamp, (*json)["user"].asString(),
        timestamp, (*json)["user"].asString());
  } else {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Sms::send(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  if ((*json)["type"].asString() == "cm") {
    auto timestamp = common::utility::timeNow();

    auto db = app().getDbClient("postgresql-master");
    db->execSqlAsync(
        "INSERT INTO sms VALUES ($1, $2, $3, $4, $5, $6, $7, $7)",
        [=](const orm::Result &r) {
          Json::Value client_req;
          client_req["ecName"] = "示例城市A市城市商业运营管理有限公司";
          client_req["apId"] = "wsmas1";
          client_req["templateId"] = (*json)["templateId"].asString();
          client_req["mobiles"] = (*json)["to"].asString();
          client_req["params"] = (*json)["msg"].asString();
          client_req["sign"] = "aPGfhhrYe";
          client_req["addSerial"] = "";
          std::string serial = fmt::format(
              "{}{}{}{}{}{}{}{}", client_req["ecName"].asString(),
              client_req["apId"].asString(), "Ws123#@!",
              client_req["templateId"].asString(),
              client_req["mobiles"].asString(), client_req["params"].asString(),
              client_req["sign"].asString(),
              client_req["addSerial"].asString());
          client_req["mac"] =
              common::utility::toLower(common::utility::md5(serial));

          auto ar =
              cpr::PostAsync(cpr::Url{"http://100.100.100.101:1992/sms/tmpsubmit"},
                             cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                             cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                             cpr::Body{common::utility::base64Encode(
                                 client_req.toStyledString())});
          auto res = ar.get();

          if (res.status_code == 200) {
            Json::Reader reader;
            Json::Value body;

            if (reader.parse(res.text, body)) {
              if (body["success"].asBool()) {
                Json::Value ret;
                ret["message"] = "success";
                ret["status"] = 1;
                callback(HttpResponse::newHttpJsonResponse(ret));
              }
            }
          }

          Json::Value ret;
          ret["message"] = "调用失败";
          ret["status"] = 0;
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
        common::utility::uuid(), (*json)["to"].asString(),
        (*json)["type"].asString(), (*json)["msg"].toStyledString(), timestamp,
        (*json)["user"].asString(), timestamp, (*json)["user"].asString());
  } else {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

Task<Json::Value> Sms::get_user(const std::string &id) const {
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
