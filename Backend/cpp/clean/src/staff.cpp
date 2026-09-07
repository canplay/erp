#include "staff.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Staff::info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("jc");
  db->execSqlAsync(
      "SELECT * FROM t_s_base_user WHERE \"id\" = ?",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info["activitiSync"] = row["activitiSync"].as<std::string>();
          info["browser"] = row["browser"].as<std::string>();
          info["realname"] = row["realname"].as<std::string>();
          info["status"] = row["status"].as<int>();
          info["userkey"] = row["userkey"].as<std::string>();
          info["username"] = row["username"].as<std::string>();
          info["departid"] = row["departid"].as<std::string>();
          info["location"] = row["location"].as<std::string>();
        }

        ret["data"] = info;
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
      (*json)["id"].asString());
}
} // namespace api
