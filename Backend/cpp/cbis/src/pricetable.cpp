#include "pricetable.h"
#include "include/utility.h"
#include "pch.h"
#include "user.h"

namespace api {
void PriceTable::count(
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
      "SELECT COUNT(id) FROM public_pricetable WHERE \"delete\" = 0";

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

void PriceTable::list(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM public_pricetable WHERE \"delete\" = 0";

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

      if (r.size() > 0) {
        for (auto it : r) {
          Json::Value info;
          info["id"] = it["id"].as<std::string>();
          info["title"] = it["title"].as<std::string>();
          info["spec"] = it["spec"].as<std::string>();
          info["unit"] = it["unit"].as<std::string>();
          info["price"] = it["price"].as<int>();
          info["remark"] = it["remark"].as<std::string>();
          info["create_date"] = it["create_date"].as<std::string>();
          info["create_user"] =
              co_await User::get_user(it["create_user"].as<std::string>());
          info["update_date"] = it["update_date"].as<std::string>();
          info["update_user"] =
              co_await User::get_user(it["update_user"].as<std::string>());
          info["delete"] = it["delete"].as<int>();
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

void PriceTable::info(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &id) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  async_run([=]() -> Task<> {
    try {
      auto db = app().getDbClient("postgresql-slave");
      auto r = co_await db->execSqlCoro("SELECT * FROM public_pricetable "
                                        "WHERE \"id\" = $1 AND \"delete\" = 0",
                                        id);

      Json::Value ret;
      Json::Value info;

      if (r.size() > 0) {
        auto row = r[0];
        info["id"] = row["id"].as<std::string>();
        info["title"] = row["title"].as<std::string>();
        info["spec"] = row["spec"].as<std::string>();
        info["unit"] = row["unit"].as<std::string>();
        info["price"] = row["price"].as<int>();
        info["remark"] = row["remark"].as<std::string>();
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

void PriceTable::update(
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
    auto id = common::utility::uuid();

    db->execSqlAsync(
        "INSERT INTO public_pricetable VALUES ($1, $2, $3, $4, $5, $6, $7, $8, "
        "$9, "
        "$10, 0)",
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
        id, (*json)["title"].asString(), (*json)["spec"].asString(),
        (*json)["unit"].asString(), (*json)["price"].asInt(),
        (*json)["remark"].toStyledString(), (*json)["user"].asString(),
        timestamp, (*json)["user"].asString(), timestamp);
  } else if ((*json)["method"].asString() == "update") {
    db->execSqlAsync(
        "UPDATE public_pricetable SET title = $1, spec = $2, unit = $3, price "
        "= "
        "$4, remark = $5, update_user = $6, update_date = $7 WHERE \"id\" = "
        "$8",
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
        (*json)["title"].asString(), (*json)["spec"].asString(),
        (*json)["unit"].asString(), (*json)["price"].asInt(),
        (*json)["remark"].asString(), (*json)["user"].asString(), timestamp,
        (*json)["id"].asString());
  } else if ((*json)["method"].asString() == "delete") {
    db->execSqlAsync(
        "UPDATE public_pricetable SET \"delete\" = 1, update_user "
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
