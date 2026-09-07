#include "shop.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void Shop::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = fmt::format("SELECT COUNT(id) FROM shop_item WHERE "
                                 "\"delete\" = 0 AND class = {}",
                                 (*json)["class"].asInt());

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

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

void Shop::list(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = fmt::format(
      "SELECT id, title, images, price, create_date, update_date FROM "
      "shop_item WHERE \"delete\" = 0 AND class = {}",
      (*json)["class"].asInt());

  if ((*json).find("title"))
    stmt = fmt::format("{} AND title LIKE '%{}%'", stmt,
                       (*json)["title"].asString());

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
        info["images"] = it["images"].as<Json::Value>();
        info["price"] = it["price"].as<int64_t>();
        info["create_date"] = it["create_date"].as<std::string>();
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

void Shop::info(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                const std::string &id) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM shop_item "
                                        "WHERE \"id\" = $1 AND \"delete\" = 0",
                                        id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["title"] = row["title"].as<std::string>();
        info["num"] = row["num"].as<int>();
        info["images"] = row["images"].as<Json::Value>();
        info["class"] = row["class"].as<std::string>();
        info["price"] = row["price"].as<int64_t>();
        info["desc"] = row["desc"].as<std::string>();
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

void Shop::update(
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
              "INSERT INTO shop_item VALUES ($1, $2, $3, "
              "$4, $5, $6, $7, false, $8, $9, $10, $11)",
              id, (*json)["title"].asString(),
              (*json)["images"].toStyledString(), (*json)["class"].asInt(),
              (*json)["price"].asInt64(), (*json)["desc"].asString(),
              (*json)["num"].asInt(), (*json)["create_user"].asString(),
              timestamp, (*json)["update_user"].asString(), timestamp);
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
    db->execSqlAsync(
        "UPDATE shop_item SET title = $1, images = $2, class = $3, price = $4, "
        "desc = $5, num = $6, update_user = $7, update_date = $8 WHERE \"id\" "
        "= "
        "$9",
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
        (*json)["title"].asString(), (*json)["images"].asString(),
        (*json)["class"].asInt(), (*json)["price"].asInt64(),
        (*json)["desc"].asString(), (*json)["num"].asInt(),
        (*json)["id"].asString());
  } break;
  case Delete:
    db->execSqlAsync(
        "UPDATE shop_item SET \"delete\" = 1, update_user "
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

void Shop::order(
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

  db->execSqlAsync(
      "INSERT INTO shop_order VALUES ($1, $2, $3, $4, $5, $6, $7, $8, "
      "0, $9, $10, $11, $12)",
      [=](const orm::Result &r) {
        if (r.affectedRows() > 0) {
          Json::Value ret;
          ret["message"] = "success";
          ret["status"] = 1;
          callback(HttpResponse::newHttpJsonResponse(ret));
        } else {
          Json::Value ret;
          ret["message"] = "新增失败";
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
      common::utility::uuid(), (*json)["item"].asString(),
      (*json)["owner"].asString(), (*json)["num"].asInt(),
      (*json)["price"].asInt64(), (*json)["amount"].asInt64(),
      (*json)["remark"].asString(), (*json)["pay_order"].asString(), timestamp,
      (*json)["create_user"].asString(), timestamp,
      (*json)["update_user"].asString());
}

void Shop::notify(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  if ((*json).find("id")) {
    for (auto it : (*json)["id"]) {
      auto db = app().getDbClient("postgresql-master");
      db->execSqlAsync(
          "UPDATE shop_order SET \"order\" = $1 WHERE \"id\" "
          "= $2",
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

void Shop::check_payment(const std::string &id) const {
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
