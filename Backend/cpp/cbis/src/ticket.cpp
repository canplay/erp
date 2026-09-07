#include "ticket.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

using namespace std::chrono_literals;

namespace api {
void Ticket::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM ticket_seats WHERE "
                     "\"delete\" = 0";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

  if ((*json).find("status_use"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status_use"].asString());

  if (std::strcmp((*json)["status_recive"].asString().c_str(), "已领用\0") == 0)
    stmt = fmt::format("{} AND \"order\" IS NOT NULL", stmt,
                       (*json)["status_recive"].asString());
  else if (std::strcmp((*json)["status_recive"].asString().c_str(),
                       "未领用\0") == 0)
    stmt = fmt::format("{} AND \"order\" IS NULL", stmt,
                       (*json)["status_recive"].asString());

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

void Ticket::list(
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
      fmt::format("SELECT * FROM ticket_seats WHERE \"delete\" = 0");

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

  if ((*json).find("status_use"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status_use"].asString());

  if (std::strcmp((*json)["status_recive"].asString().c_str(), "已领用\0") == 0)
    stmt = fmt::format("{} AND \"order\" IS NOT NULL", stmt,
                       (*json)["status_recive"].asString());
  else if (std::strcmp((*json)["status_recive"].asString().c_str(),
                       "未领用\0") == 0)
    stmt = fmt::format("{} AND \"order\" IS NULL", stmt,
                       (*json)["status_recive"].asString());

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
        info["num"] = it["num"].as<int>();
        info["seats_start"] = it["seats_start"].as<std::string>();
        info["seats_mid"] = it["seats_mid"].as<std::string>();
        info["seats_end"] = it["seats_end"].as<std::string>();
        info["date_start"] = it["date_start"].as<std::string>();
        info["date_end"] = it["date_end"].as<std::string>();
        info["remark"] = it["remark"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["create_date"] = it["create_date"].as<std::string>();
        info["create_user"] =
            co_await User::get_user(it["create_user"].as<std::string>());
        info["update_date"] = it["update_date"].as<std::string>();
        info["update_user"] =
            co_await User::get_user(it["update_user"].as<std::string>());
        info["qrcode"] = it["qrcode"].as<std::string>();
        info["address"] = it["address"].as<std::string>();
        info["price"] = it["price"].as<std::string>();
        info["owner"] = co_await User::get_user(it["owner"].as<std::string>());
        info["desc"] = it["desc"].as<std::string>();
        info["verify_user"] =
            co_await User::get_user(it["verify_user"].as<std::string>());
        info["verify_date"] = it["verify_date"].as<std::string>();
        info["verify_amount"] = it["verify_amount"].as<std::string>();
        info["num_limit"] = it["num_limit"].as<std::string>();
        info["num_available"] = it["num_available"].as<std::string>();
        info["order"] = it["order"].as<std::string>();
        info["pay_need"] = it["pay_need"].as<bool>();
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

void Ticket::info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string id = (*json)["id"].asString();

  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM ticket_seats "
                                        "WHERE \"id\" = $1 AND \"delete\" = 0",
                                        id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["title"] = row["title"].as<std::string>();
        info["num"] = row["num"].as<int>();
        info["seats_start"] = row["seats_start"].as<std::string>();
        info["seats_mid"] = row["seats_mid"].as<std::string>();
        info["seats_end"] = row["seats_end"].as<std::string>();
        info["date_start"] = row["date_start"].as<std::string>();
        info["date_end"] = row["date_end"].as<std::string>();
        info["remark"] = row["remark"].as<std::string>();
        info["status"] = row["status"].as<std::string>();
        info["create_date"] = row["create_date"].as<std::string>();
        info["create_user"] = row["create_user"].as<std::string>();
        info["update_date"] = row["update_date"].as<std::string>();
        info["update_user"] = row["update_user"].as<std::string>();
        info["qrcode"] = row["qrcode"].as<std::string>();
        info["address"] = row["address"].as<std::string>();
        info["price"] = row["price"].as<std::string>();
        info["owner"] = co_await User::get_user(row["owner"].as<std::string>());
        info["desc"] = row["desc"].as<std::string>();
        info["verify_user"] =
            co_await User::get_user(row["verify_user"].as<std::string>());
        info["verify_date"] = row["verify_date"].as<std::string>();
        info["verify_amount"] = row["verify_amount"].as<std::string>();
        info["num_limit"] = row["num_limit"].as<std::string>();
        info["num_available"] = row["num_available"].as<std::string>();
        info["order"] = row["order"].as<std::string>();
        info["pay_need"] = row["pay_need"].as<bool>();
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

void Ticket::update(
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

  switch ((*req).method()) {
  case Post:
    async_run([=]() -> Task<> {
      Json::Value ret;

      for (size_t i = 0; i < (*json)["num"].asInt(); i++) {
        try {
          auto id = common::utility::uuid();
          auto qrcode = fmt::format("{}/{}", (*json)["qrcode"].asString(), id);

          co_await db->execSqlCoro(
              "INSERT INTO ticket_seats VALUES ($1, $2, 1, $3, $4, $5, $6, $7, "
              "$8, false, $9, $10, $11, $12, $13, $14, $15, NULL, $16, $17, "
              "NULL, NULL, NULL, $18, $19, NULL, $20)",
              id, (*json)["title"].asString(),
              (*json)["seats_start"].asString(),
              (*json)["seats_end"].asString(), (*json)["date_start"].asString(),
              (*json)["date_end"].asString(), (*json)["remark"].asString(),
              (*json)["status"].asString(), (*json)["create_user"].asString(),
              timestamp, (*json)["update_user"].asString(), timestamp, qrcode,
              (*json)["address"].asString(), std::to_string(i + 1),
              (*json)["price"].asString(), (*json)["desc"].asString(),
              (*json)["num_available"].asString(),
              (*json)["num_limit"].asString(), (*json)["pay_need"].asBool());
        } catch (const std::exception &e) {
          ret["message"] = e.what();
          ret["status"] = 0;

          co_return callback(HttpResponse::newHttpJsonResponse(ret));
        }
      }

      ret["message"] = "success";
      ret["status"] = 1;
      callback(HttpResponse::newHttpJsonResponse(ret));
    });
    break;
  case Put: {
    auto qrcode = fmt::format("{}/{}", (*json)["qrcode"].asString(),
                              (*json)["id"].asString());

    db->execSqlAsync(
        "UPDATE ticket_seats SET title = $1, num = $2, seats_start = $3, "
        "seats_end = $4, date_start = $5, date_end = $6, remark = $7', status "
        "= $8, update_user = $9, update_date = $10, qrcode = $11, address = "
        "$12, seats_mid = $13, \"owner\" = $14, price = $15, \"desc\" = $16, "
        "num_available = $17, pay_need = $18, \"order\" = $19 WHERE \"id\" = "
        "$20",
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
        (*json)["title"].asString(), (*json)["num"].asInt64(),
        (*json)["seats_start"].asString(), (*json)["seats_end"].asString(),
        (*json)["date_start"].asString(), (*json)["date_end"].asString(),
        (*json)["remark"].asString(), (*json)["status"].asString(),
        (*json)["update_user"].asString(), timestamp, qrcode,
        (*json)["address"].asString(), (*json)["seats_mid"].asString(),
        (*json)["owner"].asString(), (*json)["price"].asString(),
        (*json)["desc"].asString(), (*json)["num_available"].asString(),
        (*json)["pay_need"].asBool(), (*json)["order"].asString(),
        (*json)["id"].asString());
  } break;
  case Delete:
    db->execSqlAsync(
        "UPDATE ticket_seats SET \"delete\" = 1, update_user "
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
    break;
  default:
    Json::Value ret;
    ret["message"] = "方法无效";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
    break;
  }
}

void Ticket::public_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    const std::string &id) const {
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM ticket_seats "
                                        "WHERE \"id\" = $1 AND \"delete\" = 0",
                                        id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["title"] = row["title"].as<std::string>();
        info["num"] = row["num"].as<int>();
        info["seats_start"] = row["seats_start"].as<std::string>();
        info["seats_mid"] = row["seats_mid"].as<std::string>();
        info["seats_end"] = row["seats_end"].as<std::string>();
        info["date_start"] = row["date_start"].as<std::string>();
        info["date_end"] = row["date_end"].as<std::string>();
        info["remark"] = row["remark"].as<std::string>();
        info["status"] = row["status"].as<std::string>();
        info["qrcode"] = row["qrcode"].as<std::string>();
        info["address"] = row["address"].as<std::string>();
        info["owner"] = co_await User::get_user(row["owner"].as<std::string>());
        info["price"] = row["price"].as<std::string>();
        info["desc"] = row["desc"].as<std::string>();
        info["verify_user"] =
            co_await User::get_user(row["verify_user"].as<std::string>());
        info["verify_date"] = row["verify_date"].as<std::string>();
        info["verify_amount"] = row["verify_amount"].as<std::string>();
        info["num_limit"] = row["num_limit"].as<std::string>();
        info["num_available"] = row["num_available"].as<std::string>();
        info["order"] = row["order"].as<std::string>();
        info["pay_need"] = row["pay_need"].as<bool>();
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

void Ticket::available_count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT title, date_start, date_end, create_date, "
                     "COUNT(id) FROM ticket_seats WHERE "
                     "\"order\" IS NULL AND \"delete\" = 0";

  if ((*json).find("title")) {
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());
  }

  if ((*json).find("date_start")) {
    stmt = fmt::format("{} AND date_start >= '{}'", stmt,
                       (*json)["date_start"].asString());
  }

  if ((*json).find("date_end")) {
    stmt = fmt::format("{} AND date_end <= '{}'", stmt,
                       (*json)["date_end"].asString());
  }

  stmt =
      fmt::format("{} GROUP BY title, date_start, date_end, create_date", stmt);

  async_run([=]() -> Task<> {
    try {
      std::stringstream ss;
      ss << (*json)["date_start"].asString();
      std::tm tm{};
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

      auto redisClient = app().getRedisClient("redis-slave");
      auto redis_result = co_await redisClient->execCommandCoro(
          "GET ticket_seats:count_%d", tp.time_since_epoch().count());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(stmt);

        Json::Value ret;
        ret["data"] = r.size();
        ret["message"] = "success";
        ret["status"] = 1;

        auto redisClient = app().getRedisClient("redis-master");
        co_await redisClient->execCommandCoro(
            "SET ticket_seats:count_%d %d EX 600",
            tp.time_since_epoch().count(), r.size());

        callback(HttpResponse::newHttpJsonResponse(ret));
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
          common::utility::log(
              common::utility::LOGLEVEL::info, false,
              "ticket seats available count redis error - {}:{}", __FILE__,
              __LINE__);

          Json::Value ret;
          ret["message"] = "获取数量缓存失败";
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

void Ticket::available_list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT title, date_start, date_end, create_date,"
                     "COUNT(\"id\") FROM ticket_seats WHERE"
                     "\"order\" IS NULL AND \"delete\" = 0";

  if ((*json).find("title")) {
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());
  }

  if ((*json).find("date_start")) {
    stmt = fmt::format("{} AND date_start >= '{}'", stmt,
                       (*json)["date_start"].asString());
  }

  if ((*json).find("date_end")) {
    stmt = fmt::format("{} AND date_end <= '{}'", stmt,
                       (*json)["date_end"].asString());
  }

  stmt =
      fmt::format("{} GROUP BY title, date_start, date_end, create_date", stmt);

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
      std::stringstream ss;
      ss << (*json)["date_start"].asString();
      std::tm tm{};
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

      auto redisClient = app().getRedisClient("redis-slave");
      auto redis_result = co_await redisClient->execCommandCoro(
          "GET ticket_seats:available_list_%d_%d",
          tp.time_since_epoch().count(), (*json)["curPage"].asInt64());

      if (redis_result.isNil() || redis_result.asString().empty()) {
        auto db = app().getDbClient("postgresql-slave");
        auto r = co_await db->execSqlCoro(stmt);

        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          std::string stmt = fmt::format(
              "SELECT * FROM ticket_seats WHERE title = '{}' "
              "AND create_date = '{}' AND \"delete\" = 0 AND date_start "
              ">= '{}' AND date_end <= '{}' LIMIT 1",
              row["title"].as<std::string>(),
              row["create_date"].as<std::string>(),
              row["date_start"].as<std::string>(),
              row["date_end"].as<std::string>());

          auto r1 = co_await db->execSqlCoro(stmt);

          if (r1.size() > 0) {
            auto row1 = r1[0];

            Json::Value info;
            info["title"] = row1["title"].as<std::string>();
            info["date_start"] = row1["date_start"].as<std::string>();
            info["date_end"] = row1["date_end"].as<std::string>();
            info["address"] = row1["address"].as<std::string>();
            info["price"] = row1["price"].as<std::string>();
            info["desc"] = row1["desc"].as<std::string>();
            info["remark"] = row1["remark"].as<std::string>();
            info["num_limit"] = row1["num_limit"].as<std::string>();
            info["pay_need"] = row1["pay_need"].as<bool>();
            info["num"] = row["count"].as<int>();
            infos.append(info);
          }
        }

        Json::Value ret;
        ret["data"] = infos;
        ret["message"] = "success";
        ret["status"] = 1;

        auto redisClient = app().getRedisClient("redis-master");
        co_await redisClient->execCommandCoro(
            "SET ticket_seats:available_list_%d_%d %s EX 600",
            tp.time_since_epoch().count(), (*json)["curPage"].asInt64(),
            Json::FastWriter().write(infos).c_str());

        callback(HttpResponse::newHttpJsonResponse(ret));
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
                               "ticket available redis error - {}:{}", __FILE__,
                               __LINE__);

          Json::Value ret;
          ret["message"] = "获取可用数量缓存失败";
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

void Ticket::order(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  check_limit(
      (*json)["title_group"].asString(), (*json)["user"].asString(),
      [=, this](const bool limit) {
        if (!limit) {
          auto timestamp = common::utility::timeNow();

          auto db = app().getDbClient("postgresql-slave");
          db->execSqlAsync(
              "SELECT * FROM ticket_seats WHERE title = $1 AND "
              "\"order\" IS NULL AND \"delete\" = 0 AND date_start >= $2 "
              "AND date_end <= $3 ORDER BY CAST (seats_mid as integer) ASC",
              [=, this](const orm::Result &r) {
                if (r.size() > 0) {
                  async_run([=, this]() -> Task<> {
                    try {
                      Json::Value infos;

                      for (size_t i = 0; i < (*json)["num"].asInt(); i++) {
                        auto row = r[i];

                        auto db = app().getDbClient("postgresql-master");
                        auto r = co_await db->execSqlCoro(
                            "UPDATE ticket_seats SET \"owner\" = $1, "
                            "update_user = $2, update_date = $3 WHERE \"id\" "
                            "= "
                            "$4",
                            (*json)["user"].asString(),
                            (*json)["user"].asString(), timestamp,
                            row["id"].as<std::string>());

                        if (r.affectedRows() > 0) {
                          Json::Value info;
                          info["id"] = row["id"].as<std::string>();
                          info["title"] = row["title"].as<std::string>();
                          info["num"] = row["num"].as<int>();
                          info["seats_start"] =
                              row["seats_start"].as<std::string>();
                          info["seats_mid"] =
                              row["seats_mid"].as<std::string>();
                          info["seats_end"] =
                              row["seats_end"].as<std::string>();
                          info["date_start"] =
                              row["date_start"].as<std::string>();
                          info["date_end"] = row["date_end"].as<std::string>();
                          info["remark"] = row["remark"].as<std::string>();
                          info["status"] = row["status"].as<std::string>();
                          info["qrcode"] = row["qrcode"].as<std::string>();
                          info["address"] = row["address"].as<std::string>();
                          info["owner"] = co_await User::get_user(
                              row["owner"].as<std::string>());
                          info["price"] = row["price"].as<std::string>();
                          info["desc"] = row["desc"].as<std::string>();
                          info["verify_user"] = co_await User::get_user(
                              row["verify_user"].as<std::string>());
                          info["verify_date"] =
                              row["verify_date"].as<std::string>();
                          info["verify_amount"] =
                              row["verify_amount"].as<std::string>();
                          info["num_limit"] =
                              row["num_limit"].as<std::string>();
                          info["num_available"] =
                              row["num_available"].as<std::string>();
                          info["order"] = row["order"].as<std::string>();
                          info["pay_need"] = row["pay_need"].as<bool>();
                          infos.append(info);
                          check_payment(row["order"].as<std::string>());
                        }
                      }

                      Json::Value ret;
                      ret["data"] = infos;
                      ret["message"] = "success";
                      ret["status"] = 1;
                      callback(HttpResponse::newHttpJsonResponse(ret));
                    } catch (const std::exception &e) {
                      common::utility::log(common::utility::LOGLEVEL::info,
                                           false, "{} - {}:{}", e.what(),
                                           __FILE__, __LINE__);

                      Json::Value ret;
                      ret["message"] = e.what();
                      ret["status"] = 0;
                      callback(HttpResponse::newHttpJsonResponse(ret));
                    }
                  });
                } else {
                  Json::Value ret;
                  Json::Value info;
                  ret["data"] = info;
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
              },
              (*json)["title"].asString(), (*json)["date_start"].asString(),
              (*json)["date_end"].asString());
        } else {
          Json::Value ret;
          ret["message"] = "已超出可购买数量";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      });
}

void Ticket::notify(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  if ((*json)["id"].isArray()) {
    for (auto it : (*json)["id"]) {
      auto db = app().getDbClient("postgresql-master");
      db->execSqlAsync(
          "UPDATE ticket_seats SET \"order\" = $1 WHERE \"id\" = $2",
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
          (*json)["order"].asString(), it.asString());
    }
  } else {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Ticket::recive(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  check_limit(
      (*json)["title_group"].asString(), (*json)["user"].asString(),
      [=](const bool limit) {
        if (!limit) {
          auto timestamp = common::utility::timeNow();

          auto db = app().getDbClient("postgresql-slave");
          db->execSqlAsync(
              "SELECT * FROM ticket_seats WHERE title = $1 AND "
              "\"order\" IS NULL AND \"delete\" = 0 AND date_start >= $2 "
              "AND date_end <= $3 ORDER BY CAST (seats_mid as integer) ASC",
              [=](const orm::Result &r) {
                if (r.size() > 0) {
                  async_run([=]() -> Task<> {
                    try {
                      Json::Value ret;

                      for (size_t i = 0; i < (*json)["num"].asInt(); i++) {
                        auto row = r[i];

                        auto db = app().getDbClient("postgresql-master");
                        auto r = co_await db->execSqlCoro(
                            "UPDATE ticket_seats SET \"owner\" = $1, "
                            "update_user "
                            "= $2, update_date = $3 WHERE \"id\" = $4",
                            (*json)["user"].asString(),
                            (*json)["user"].asString(), timestamp,
                            row["id"].as<std::string>());
                      }

                      ret["message"] = "success";
                      ret["status"] = 1;
                      callback(HttpResponse::newHttpJsonResponse(ret));
                    } catch (const std::exception &e) {
                      common::utility::log(common::utility::LOGLEVEL::info,
                                           false, "{} - {}:{}", e.what(),
                                           __FILE__, __LINE__);

                      Json::Value ret;
                      ret["message"] = e.what();
                      ret["status"] = 0;

                      callback(HttpResponse::newHttpJsonResponse(ret));
                    }
                  });
                } else {
                  Json::Value ret;
                  Json::Value info;
                  ret["data"] = info;
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
              },
              (*json)["title"].asString(), (*json)["date_start"].asString(),
              (*json)["date_end"].asString());
        } else {
          Json::Value ret;
          ret["message"] = "已超出可购买数量";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      });
}

void Ticket::recived(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback,
                     const std::string &id) const {
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(
          "SELECT * FROM ticket_seats WHERE \"owner\" = $1 AND "
          "\"delete\" = 0",
          id);

      Json::Value ret;
      Json::Value infos;

      for (auto it : r) {
        Json::Value info;
        info["id"] = it["id"].as<std::string>();
        info["title"] = it["title"].as<std::string>();
        info["num"] = it["num"].as<int>();
        info["seats_start"] = it["seats_start"].as<std::string>();
        info["seats_mid"] = it["seats_mid"].as<std::string>();
        info["seats_end"] = it["seats_end"].as<std::string>();
        info["date_start"] = it["date_start"].as<std::string>();
        info["date_end"] = it["date_end"].as<std::string>();
        info["remark"] = it["remark"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["qrcode"] = it["qrcode"].as<std::string>();
        info["address"] = it["address"].as<std::string>();
        info["owner"] = co_await User::get_user(it["owner"].as<std::string>());
        info["price"] = it["price"].as<std::string>();
        info["desc"] = it["desc"].as<std::string>();
        info["verify_user"] =
            co_await User::get_user(it["verify_user"].as<std::string>());
        info["verify_date"] = it["verify_date"].as<std::string>();
        info["verify_amount"] = it["verify_amount"].as<std::string>();
        info["num_limit"] = it["num_limit"].as<std::string>();
        info["num_available"] = it["num_available"].as<std::string>();
        info["order"] = it["order"].as<std::string>();
        info["pay_need"] = it["pay_need"].as<bool>();
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

void Ticket::verify_history(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    const std::string &id) const {
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro(
          "SELECT * FROM ticket_seats WHERE verify_user = $1 "
          "AND \"delete\" = 0");

      Json::Value ret;
      Json::Value infos;

      for (auto it : r) {
        Json::Value info;
        info["id"] = it["id"].as<std::string>();
        info["title"] = it["title"].as<std::string>();
        info["num"] = it["num"].as<int>();
        info["seats_start"] = it["seats_start"].as<std::string>();
        info["seats_mid"] = it["seats_mid"].as<std::string>();
        info["seats_end"] = it["seats_end"].as<std::string>();
        info["date_start"] = it["date_start"].as<std::string>();
        info["date_end"] = it["date_end"].as<std::string>();
        info["remark"] = it["remark"].as<std::string>();
        info["status"] = it["status"].as<std::string>();
        info["qrcode"] = it["qrcode"].as<std::string>();
        info["address"] = it["address"].as<std::string>();
        info["owner"] = co_await User::get_user(it["owner"].as<std::string>());
        info["price"] = it["price"].as<std::string>();
        info["desc"] = it["desc"].as<std::string>();
        info["verify_user"] =
            co_await User::get_user(it["verify_user"].as<std::string>());
        info["verify_date"] = it["verify_date"].as<std::string>();
        info["verify_amount"] = it["verify_amount"].as<std::string>();
        info["num_limit"] = it["num_limit"].as<std::string>();
        info["num_available"] = it["num_available"].as<std::string>();
        info["order"] = it["order"].as<std::string>();
        info["pay_need"] = it["pay_need"].as<bool>();
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

void Ticket::verify(
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
  check_available((*json)["id"].as<std::string>(), [=](const int num) {
    if (num >= 1) {
      db->execSqlAsync(
          "UPDATE ticket_seats SET update_user = $1, update_date = $2, "
          "verify_user = $3, verify_date = $4, verify_amount = $5 WHERE "
          "\"id\" "
          "= "
          "$6",
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
          (*json)["user"].asString(), timestamp, (*json)["user"].asString(),
          timestamp, (*json)["verify_amount"].asString(),
          (*json)["id"].as<std::string>());
    } else if (num == 0) {
      db->execSqlAsync(
          "UPDATE ticket_seats SET status = $1, update_user = $2, "
          "update_date "
          "= $3, verify_user = $4, verify_date = $5, verify_amount = $6 "
          "WHERE "
          "\"id\" = $7",
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
          "已使用", (*json)["user"].asString(), timestamp,
          (*json)["user"].asString(), timestamp,
          (*json)["verify_amount"].asString(), (*json)["id"].as<std::string>());
    } else {
      Json::Value ret;
      ret["message"] = "已核销";
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  });
}

void Ticket::refund(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &id) const {
  auto db = app().getDbClient("postgresql-master");
  db->execSqlAsync(
      "UPDATE ticket_seats SET \"owner\" = NULL, \"order\" = "
      "NULL, verify_user = NULL, verify_date = NULL, verify_amount = NULL "
      "WHERE \"id\" = $1 AND \"delete\" = 0",
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
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = e.base().what();
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      id);
}

void Ticket::check_limit(const std::string &title, const std::string &user,
                         std::function<void(const bool)> &&callback) const {
  std::string stmt = fmt::format("SELECT num_limit FROM ticket_seats WHERE "
                                 "title LIKE '%{}%' AND \"delete\" = 0",
                                 title);

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        if (r.size() > 0) {
          auto row = r[0];

          std::string stmt =
              fmt::format("SELECT COUNT(id) FROM ticket_seats WHERE title "
                          "LIKE '%{}%' AND \"owner\" = '{}' AND \"delete\" = 0",
                          title, user);

          db->execSqlAsync(
              stmt,
              [=](const orm::Result &r) {
                auto row1 = r[0];

                if (row1[0].as<int>() >= row[0].as<int>()) {
                  callback(true);
                } else {
                  callback(false);
                }
              },
              [=](const orm::DrogonDbException &e) {
                common::utility::log(common::utility::LOGLEVEL::info, false,
                                     "{} - {}:{}", e.base().what(), __FILE__,
                                     __LINE__);

                callback(false);
              });
        } else {
          callback(false);
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        callback(false);
      });
}

void Ticket::check_available(const std::string &id,
                             std::function<void(const int)> &&callback) const {
  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT num_available FROM ticket_seats WHERE "
      "\"id\" = $1 AND \"delete\" = 0 AND status = '待使用'",
      [=](const orm::Result &r) {
        if (r.size() > 0) {
          auto row = r[0];
          int num = std::stoi(row[0].as<std::string>());

          if (num >= 1) {
            num -= 1;

            auto db = app().getDbClient("postgresql-master");
            db->execSqlAsync(
                "UPDATE ticket_seats SET num_available = $1 "
                "WHERE \"id\" = $2",
                [=](const orm::Result &r) { callback(num); },
                [=](const orm::DrogonDbException &e) {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "{} - {}:{}", e.base().what(), __FILE__,
                                       __LINE__);

                  callback(num);
                },
                std::to_string(num), id);
          } else {
            callback(num);
          }
        } else {
          callback(-1);
        }
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        callback(-1);
      },
      id);
}

void Ticket::check_payment(const std::string &id) const {
  async_run([=]() -> Task<> {
    auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    time_t time_old = std::chrono::system_clock::to_time_t(now);
    auto time_diff = std::difftime(time, time_old);

    while (time_diff <= 60) {
      std::this_thread::sleep_for(std::chrono::milliseconds(30));

      common::utility::log(common::utility::LOGLEVEL::info, false,
                           "========== start check payment: {} ==========", id);

      auto db = app().getDbClient("postgresql-slave");
      db->execSqlAsync(
          "SELECT \"order\", \"status\" FROM pay WHERE \"id\" = $1",
          [&](const orm::Result &r) {
            auto db = app().getDbClient("postgresql-master");

            if (r.size() > 0) {
              auto row = r[0];

              if (row["status"].as<std::string>() == "paid") {
                db->execSqlAsync(
                    "UPDATE ticket_seats SET \"order\" = $1 "
                    "WHERE \"id\" = $2",
                    [&](const orm::Result &r) {
                      common::utility::log(
                          common::utility::LOGLEVEL::info, false,

                          "========== check payment update: {} ==========",
                          row["order"].as<std::string>(), id);
                      time_diff = 8000;
                    },
                    [&](const orm::DrogonDbException &e) {
                      common::utility::log(common::utility::LOGLEVEL::info,
                                           false, "{} - {}:{}", e.base().what(),
                                           __FILE__, __LINE__);
                    },
                    id);
              } else {
                db->execSqlAsync(
                    "UPDATE ticket_seats SET \"order\" = NULL "
                    "WHERE \"id\" = $1",
                    [&](const orm::Result &r) {
                      common::utility::log(
                          common::utility::LOGLEVEL::info, false,

                          "========== check payment update: {} ==========", id);
                      now = std::chrono::system_clock::now();
                      time = std::chrono::system_clock::to_time_t(now);
                      time_diff = std::difftime(time, time_old);
                    },
                    [&](const orm::DrogonDbException &e) {
                      common::utility::log(common::utility::LOGLEVEL::info,
                                           false, "{} - {}:{}", e.base().what(),
                                           __FILE__, __LINE__);
                    },
                    id);
              }
            } else {
              common::utility::log(
                  common::utility::LOGLEVEL::info, false,
                  "========== end check payment: {} ==========", id);
            }
          },
          [&](const orm::DrogonDbException &e) {
            common::utility::log(common::utility::LOGLEVEL::info, false,
                                 "{} - {}:{}", e.base().what(), __FILE__,
                                 __LINE__);
            common::utility::log(
                common::utility::LOGLEVEL::info, false,
                "========== end check payment: {} ==========", id);
          },
          id);
    }
    co_return;
  });
}
} // namespace api
