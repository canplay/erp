#include "public_assets.h"
#include "include/utility.h"
#include "pch.h"

using namespace drogon;

namespace api {
void PublicAssets::count(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM public_assets WHERE "
                     "\"delete\" = 0";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  if ((*json).find("create_date"))
    stmt = fmt::format("{} AND create_date LIKE '%{}%'", stmt,
                       (*json)["create_date"].asString());

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

void PublicAssets::add(
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
      "INSERT INTO public_assets VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, "
      "$10, $11, $12)",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.affectedRows() > 0) {
          info["date"] = timestamp;
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
      (*json)["id"].asString(), (*json)["name"].asString(),
      (*json)["status"].asString(), (*json)["create_user"].asString(),
      timestamp, (*json)["update_user"].asString(), timestamp, false,
      (*json)["remark"].asString(), (*json)["history"].asString(),
      (*json)["locate"].asString(), (*json)["type"].asString());
}

void PublicAssets::update(
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
      "UPDATE public_assets SET name = $1, status = $2, locate = $3, "
      "update_user = $4, update_date = $5, \"delete\" = $6, remark = $7, "
      "history = $8, \"type\" = $9 WHERE \"id\" = $10",
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
      (*json)["name"].asString(), (*json)["status"].asString(),
      (*json)["locate"].asString(), (*json)["update_user"].asString(),
      timestamp, (*json)["delete"].asBool(), (*json)["remark"].asString(),
      (*json)["history"].asString(), (*json)["type"].asString(),
      (*json)["id"].asString());
}

void PublicAssets::info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM public_assets WHERE \"delete\" = 0";

  if ((*json).find("id"))
    stmt = fmt::format("{} AND id LIKE '%{}%'", stmt, (*json)["id"].asString());

  if ((*json).find("name"))
    stmt = fmt::format("{} AND name LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

  if ((*json).find("status"))
    stmt = fmt::format("{} AND status LIKE '%{}%'", stmt,
                       (*json)["status"].asString());

  if ((*json).find("remark"))
    stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                       (*json)["remark"].asString());

  if ((*json).find("create_date"))
    stmt = fmt::format("{} AND create_date LIKE '%{}%'", stmt,
                       (*json)["create_date"].asString());

  if ((*json).find("sortBy")) {
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

    if ((*json)["descending"].asBool())
      stmt = fmt::format("{} DESC", stmt);
    else
      stmt = fmt::format("{} ASC", stmt);
  }

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          std::string str(row["history"].as<std::string>());
          std::regex delim(",");
          std::vector<std::string> v(
              std::sregex_token_iterator(str.begin(), str.end(), delim, -1),
              std::sregex_token_iterator());

          Json::Value info;
          info["id"] = row["id"].as<std::string>();
          info["name"] = row["name"].as<std::string>();
          info["status"] = row["status"].as<std::string>();
          info["locate"] = row["locate"].as<std::string>();
          info["type"] = row["type"].as<std::string>();
          info["history"] = row["history"].as<std::string>();
          info["history_count"] = v.size() - 1;
          info["delete"] = row["delete"].as<bool>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();

          infos.append(info);
        }

        ret["data"] = infos;
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
      });
}

void PublicAssets::dz_open(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto place = common::utility::utf8ToGBK((*json)["place"].asString().c_str());
  auto name = common::utility::utf8ToGBK((*json)["name"].asString().c_str());
  std::string park, mach, key, no, msg;

  common::utility::log(common::utility::LOGLEVEL::info, false,
                       "远程开闸: {} - {}", place, name);

  if (place == "示例停车场A") {
    park = "PARK_PHONE_01";
    key = "PARK_KEY_01";
  } else if (place == "示例停车场B") {
    park = "PARK_PHONE_02";
    key = "PARK_KEY_02";
  } else if (place == "示例停车场C") {
    park = "PARK_PHONE_03";
    key = "PARK_KEY_03";
  } else if (place == "示例停车场D") {
    park = "PARK_PHONE_04";
    key = "PARK_KEY_04";
  } else if (place == "示例停车场E") {
    park = "PARK_PHONE_05";
    key = "PARK_KEY_05";
  } else if (place == "示例停车场G") {
    park = "PARK_PHONE_06";
    key = "PARK_KEY_06";
  } else if (place == "示例停车场F") {
    park = "PARK_PHONE_07";
    key = "PARK_KEY_07";
  } else if (place == "示例停车场H") {
    park = "PARK_PHONE_08";
    key = "PARK_KEY_08";
  } else if (place == "示例停车场I") {
    park = "PARK_PHONE_09";
    key = "PARK_KEY_09";
  } else if (place == "示例停车场J") {
    park = "PARK_PHONE_10";
    key = "PARK_KEY_10";
  } else if (place == "城乡投停车场") {
    park = "PARK_PHONE_11";
    key = "PARK_KEY_11";
  } else if (place == "敬老院停车场") {
    park = "PARK_PHONE_12";
    key = "PARK_KEY_12";
  } else {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string url = fmt::format("http://www.etpcar.com/App/"
                                "UpOpenCmd.aspx?ParkNo={}&CarNo=&CarType=&"
                                "MachNo={}&Memo=远程开闸&OpenFlg=0",
                                park, name);

  std::transform(url.begin(), url.end(), url.begin(), ::tolower);
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);

  std::string token = fmt::format("{}{}", url, key);
  std::string hash = common::utility::md5(token);

  auto ar = cpr::GetAsync(cpr::Url{"http://www.etpcar.com/App/UpOpenCmd.aspx"},
                          cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                          cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000},
                          cpr::Payload{{"ParkNo", park},
                                       {"CarNo", ""},
                                       {"CarType", ""},
                                       {"MachNo", name},
                                       {"Memo", "远程开闸"},
                                       {"OpenFlg", "0"},
                                       {"Token", hash}});
  auto res = ar.get();

  if (res.status_code == 200) {
    tinyxml2::XMLDocument doc;
    doc.Parse(res.text.c_str());
    common::utility::log(common::utility::LOGLEVEL::info, false,
                         res.text.c_str());

    Json::Value info;
    info["name"] = name;
    info["no"] = no;
    info["msg"] = msg;

    Json::Value ret;
    ret["data"] = info;
    ret["message"] = "success";
    ret["status"] = 1;

    callback(HttpResponse::newHttpJsonResponse(ret));
  } else {
    tinyxml2::XMLDocument doc;
    doc.Parse(res.text.c_str());
    common::utility::log(common::utility::LOGLEVEL::info, false,
                         res.text.c_str());

    Json::Value ret;
    ret["message"] = "调用失败";
    ret["status"] = 0;

    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void PublicAssets::history_add(
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
      "INSERT INTO public_assets_history VALUES ($1, $2, $3, "
      "$4, $5, $6, $7, $8, $9, $10, $11, $12)",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.affectedRows() > 0) {
          info["date"] = timestamp;
        }

        ret["data"] = info;
        ret["message"] = "success";
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
      (*json)["id"].asString(), (*json)["type"].asString(),
      (*json)["status"].asString(), (*json)["date_start"].asString(),
      (*json)["date_end"].asString(), (*json)["create_user"].asString(),
      timestamp, (*json)["update_user"].asString(), timestamp, false,
      (*json)["remark"].asString(), (*json)["images"].asString());
}

void PublicAssets::history_update(
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
      "UPDATE public_assets_history SET \"type\" = $1, status = $2, "
      "date_start = $3, date_end = $4, update_user = $5, update_date = $6, "
      "\"delete\" = $7, remark = $8, images = $9 WHERE \"id\" = $10",
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
      (*json)["type"].asString(), (*json)["status"].asString(),
      (*json)["date_start"].asString(), (*json)["date_end"].asString(),
      (*json)["update_user"].asString(), timestamp, (*json)["delete"].asBool(),
      (*json)["remark"].asBool(), (*json)["images"].asString(),
      (*json)["id"].asString());
}

void PublicAssets::history_info(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string str((*json)["id"].asString());
  std::regex delim(",");
  std::vector<std::string> v(
      std::sregex_token_iterator(str.begin(), str.end(), delim, -1),
      std::sregex_token_iterator());

  for (auto &&s : v) {
    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "SELECT * FROM public_assets_history WHERE \"delete\" = "
        "false AND \"id\" = $1",
        [=](const orm::Result &r) {
          Json::Value ret;
          Json::Value info;

          auto row = r[0];

          if (r.size() > 0) {
            info["id"] = row["id"].as<std::string>();
            info["type"] = row["type"].as<std::string>();
            info["status"] = row["status"].as<std::string>();
            info["date_start"] = row["date_start"].as<std::string>();
            info["date_end"] = row["date_end"].as<std::string>();
            info["remark"] = row["remark"].as<std::string>();
            info["images"] = row["images"].as<std::string>();
            info["delete"] = row["delete"].as<bool>();
            info["create_user"] = row["create_user"].as<std::string>();
            info["create_date"] = row["create_date"].as<std::string>();
            info["update_user"] = row["update_user"].as<std::string>();
            info["update_date"] = row["update_date"].as<std::string>();
          }

          ret["data"] = info;
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
        s);
  }
}
} // namespace api
