#include "car.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Car::count(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT COUNT(id) FROM tow_car WHERE \"delete\" = false";

  if (std::strcmp((*json)["status"].asString().c_str(), "未放行\0") == 0) {
    stmt = fmt::format("{} AND (rs_date IS NULL OR rs_date = "
                       "'1000-01-01 00:00:00')",
                       stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "已放行\0") ==
             0) {
    stmt = fmt::format("{} AND (rs_date IS NOT NULL OR "
                       "rs_date != '1000-01-01 00:00:00')",
                       stmt);
  }

  std::vector<std::string> in_date_list;
  if ((*json).find("in_date")) {
    auto index = (*json)["in_date"].asString().find(" - ");

    if (!index) {
      in_date_list.push_back((*json)["in_date"].asString());
    } else {
      in_date_list.push_back((*json)["in_date"].asString().substr(0, index));
      in_date_list.push_back((*json)["in_date"].asString().substr(index + 1));
    }
  }

  std::vector<std::string> out_date_list;
  if ((*json).find("out_date")) {
    auto index = (*json)["out_date"].asString().find(" - ");

    if (!index) {
      out_date_list.push_back((*json)["out_date"].asString());
    } else {
      out_date_list.push_back((*json)["out_date"].asString().substr(0, index));
      out_date_list.push_back((*json)["out_date"].asString().substr(index + 1));
    }
  }

  if ((*json)["in_date"].asString().length() == 1 &&
      (*json)["out_date"].asString().length() == 1) {
    stmt = fmt::format("{} AND (dc_date LIKE '{}%' OR rs_date LIKE '{}%')",
                       stmt, in_date_list[0], out_date_list[0]);
  } else {
    if ((*json)["in_date"].asString().length() == 1) {
      stmt = fmt::format("{} AND dc_date LIKE '{}%'", stmt, in_date_list[0]);
    } else if ((*json)["in_date"].asString().length() == 2) {
      stmt = fmt::format("{} AND dc_date between '{}' and '{}'", stmt,
                         in_date_list[0], in_date_list[1]);
    }

    if ((*json)["out_date"].asString().length() == 1) {
      stmt = fmt::format("{} AND rs_date LIKE '{}%'", stmt, out_date_list[0]);
    } else if ((*json)["out_date"].asString().length() == 2) {
      stmt = fmt::format("{} AND rs_date between '{}' and '{}'", stmt,
                         out_date_list[0], out_date_list[1]);
    }
  }

  if ((*json).find("content")) {
    if (std::strcmp((*json)["model"].asString().c_str(), "车牌\0") == 0) {
      stmt = fmt::format("{} AND license LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车辆类型\0") ==
               0) {
      stmt = fmt::format("{} AND car_type LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车身颜色\0") ==
               0) {
      stmt = fmt::format("{} AND car_color LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车架号\0") ==
               0) {
      stmt = fmt::format("{} AND vehicle LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "发动机号\0") ==
               0) {
      stmt = fmt::format("{} AND engine LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "记录人\0") ==
               0) {
      stmt = fmt::format("{} AND dc_name LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车辆备注\0") ==
               0) {
      stmt = fmt::format("{} AND car_remark LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "拖移备注\0") ==
               0) {
      stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    }
  }

  if ((*json).find("name")) {
    stmt = fmt::format("{} AND dc_name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());
  }

  if (std::strcmp((*json)["unit"].asString().c_str(), "城管部门\0") == 0) {
    stmt =
        fmt::format("{} AND (cmd_unit = '城管部门' OR cmd_unit = '卧龙执法点' "
                    "OR cmd_unit = '开化执法点' OR cmd_unit = '新平执法点')",
                    stmt);
  } else if ((*json).find("unit")) {
    stmt = fmt::format("{} AND cmd_unit LIKE '{}'", stmt,
                       (*json)["unit"].asString());
  }

  if ((*json).find("key")) {
    stmt =
        fmt::format("{} AND dc_key LIKE '{}'", stmt, (*json)["key"].asString());
  }

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

void Car::list(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string stmt = "SELECT * FROM tow_car WHERE \"delete\" = false";

  if (std::strcmp((*json)["status"].asString().c_str(), "未放行\0") == 0) {
    stmt = fmt::format("{} AND (rs_date IS NULL OR rs_date = "
                       "'1000-01-01 00:00:00')",
                       stmt);
  } else if (std::strcmp((*json)["status"].asString().c_str(), "已放行\0") ==
             0) {
    stmt = fmt::format("{} AND (rs_date IS NOT NULL OR "
                       "rs_date != '1000-01-01 00:00:00')",
                       stmt);
  }

  std::vector<std::string> in_date_list;
  if ((*json).find("in_date")) {
    auto index = (*json)["in_date"].asString().find(" - ");

    if (!index) {
      in_date_list.push_back((*json)["in_date"].asString());
    } else {
      in_date_list.push_back((*json)["in_date"].asString().substr(0, index));
      in_date_list.push_back((*json)["in_date"].asString().substr(index + 1));
    }
  }

  std::vector<std::string> out_date_list;
  if ((*json).find("out_date")) {
    auto index = (*json)["out_date"].asString().find(" - ");

    if (!index) {
      out_date_list.push_back((*json)["out_date"].asString());
    } else {
      out_date_list.push_back((*json)["out_date"].asString().substr(0, index));
      out_date_list.push_back((*json)["out_date"].asString().substr(index + 1));
    }
  }

  if ((*json).find("in_date") && (*json).find("out_date")) {
    stmt = fmt::format("{} AND (dc_date LIKE '{}%' OR rs_date LIKE '{}%')",
                       stmt, in_date_list[0], out_date_list[0]);
  } else {
    if ((*json)["in_date"].asString().length() == 1) {
      stmt = fmt::format("{} AND dc_date LIKE '{}%'", stmt, in_date_list[0]);
    } else if ((*json)["in_date"].asString().length() == 2) {
      stmt = fmt::format("{} AND dc_date between '{}' and '{}'", stmt,
                         in_date_list[0], in_date_list[1]);
    }

    if ((*json)["out_date"].asString().length() == 1) {
      stmt = fmt::format("{} AND rs_date LIKE '{}%'", stmt, out_date_list[0]);
    } else if ((*json)["out_date"].asString().length() == 2) {
      stmt = fmt::format("{} AND rs_date between '{}' and '{}'", stmt,
                         out_date_list[0], out_date_list[1]);
    }
  }

  if ((*json).find("content")) {
    if (std::strcmp((*json)["model"].asString().c_str(), "车牌\0") == 0) {
      stmt = fmt::format("{} AND license LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车辆类型\0") ==
               0) {
      stmt = fmt::format("{} AND car_type LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车身颜色\0") ==
               0) {
      stmt = fmt::format("{} AND car_color LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车架号\0") ==
               0) {
      stmt = fmt::format("{} AND vehicle LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "发动机号\0") ==
               0) {
      stmt = fmt::format("{} AND engine LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "记录人\0") ==
               0) {
      stmt = fmt::format("{} AND dc_name LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "车辆备注\0") ==
               0) {
      stmt = fmt::format("{} AND car_remark LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    } else if (std::strcmp((*json)["model"].asString().c_str(), "拖移备注\0") ==
               0) {
      stmt = fmt::format("{} AND remark LIKE '%{}%'", stmt,
                         (*json)["content"].asString());
    }
  }

  if ((*json).find("name")) {
    stmt = fmt::format("{} AND dc_name LIKE '%{}%'", stmt,
                       (*json)["name"].asString());
  }

  if (std::strcmp((*json)["unit"].asString().c_str(), "城管部门\0") == 0) {
    stmt =
        fmt::format("{} AND (cmd_unit = '城管部门' OR cmd_unit = '卧龙执法点' "
                    "OR cmd_unit = '开化执法点' OR cmd_unit = '新平执法点')",
                    stmt);
  } else if (std::strcmp((*json)["unit"].asString().c_str(), "全部\0") == 0) {
  } else if ((*json).find("unit")) {
    stmt = fmt::format("{} AND cmd_unit LIKE '{}'", stmt,
                       (*json)["unit"].asString());
  }

  if ((*json).find("key")) {
    stmt =
        fmt::format("{} AND dc_key LIKE '{}'", stmt, (*json)["key"].asString());
  }

  if ((*json).find("sortBy")) {
    stmt = fmt::format("{} ORDER BY {}", stmt, (*json)["sortBy"].asString());

    if ((*json)["descending"].asBool())
      stmt = fmt::format("{} DESC", stmt);
    else
      stmt = fmt::format("{} ASC", stmt);
  }

  stmt =
      fmt::format("{} LIMIT {} OFFSET {}", stmt, (*json)["maxPage"].asString(),
                  (*json)["curPage"].asString());

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<int>();
          info["license"] = row["license"].as<std::string>();
          info["vehicle"] = row["vehicle"].as<Json::Value>();
          info["engine"] = row["engine"].as<std::string>();
          info["car_type"] = row["car_type"].as<std::string>();
          info["dc_type"] = row["dc_type"].as<std::string>();
          info["dc_causes"] = row["dc_causes"].as<std::string>();
          info["car_color"] = row["car_color"].as<std::string>();
          info["dc_date"] = row["dc_date"].as<std::string>();
          info["dc_address"] = row["dc_address"].as<std::string>();
          info["dc_key"] = row["dc_key"].as<std::string>();
          info["dc_party_name"] = row["dc_party_name"].as<std::string>();
          info["dc_party_cardid"] = row["dc_party_cardid"].as<std::string>();
          info["dc_party_tel"] = row["dc_party_tel"].as<std::string>();
          info["p_name"] = row["p_name"].as<std::string>();
          info["p_id"] = row["p_id"].as<std::string>();
          info["dc_acc"] = row["dc_acc"].as<std::string>();
          info["dc_name"] = row["dc_name"].as<std::string>();
          info["dc_into_date"] = row["dc_into_date"].as<std::string>();
          info["car_remark"] = row["car_remark"].as<std::string>();
          info["driver"] = row["driver"].as<std::string>();
          info["operator"] = row["operator"].as<std::string>();
          info["drag_km"] = row["drag_km"].as<std::string>();
          info["drag_unit"] = row["drag_unit"].as<std::string>();
          info["drag_money"] = row["drag_money"].as<std::string>();
          info["cmd_unit"] = row["cmd_unit"].as<std::string>();
          info["cmd_user"] = row["cmd_user"].as<std::string>();
          info["cv"] = row["cv"].as<std::string>();
          info["cv_acc"] = row["cv_acc"].as<std::string>();
          info["cv_name"] = row["cv_name"].as<std::string>();
          info["cv_date"] = row["cv_date"].as<std::string>();
          info["cv_opinion"] = row["cv_opinion"].as<std::string>();
          info["tv"] = row["tv"].as<std::string>();
          info["tv_acc"] = row["tv_acc"].as<std::string>();
          info["tv_name"] = row["tv_name"].as<std::string>();
          info["tv_date"] = row["tv_date"].as<std::string>();
          info["tv_opinion"] = row["tv_opinion"].as<std::string>();
          info["rc_name"] = row["rc_name"].as<std::string>();
          info["rc_idcard"] = row["rc_idcard"].as<std::string>();
          info["rc_tel"] = row["rc_tel"].as<std::string>();
          info["parking_date"] = row["parking_date"].as<std::string>();
          info["parking_unit"] = row["parking_unit"].as<std::string>();
          info["parking_money"] = row["parking_money"].as<std::string>();
          info["parking_payable"] = row["parking_payable"].as<std::string>();
          info["parking_paidin"] = row["parking_paidin"].as<std::string>();
          info["remark"] = row["remark"].as<std::string>();
          info["rs_name"] = row["rs_name"].as<std::string>();
          info["rs_acc"] = row["rs_acc"].as<std::string>();
          info["rs_date"] = row["rs_date"].as<std::string>();
          info["attachment"] = row["attachment"].as<std::string>();
          info["create_date"] = row["create_date"].as<std::string>();
          info["create_user"] = row["create_user"].as<std::string>();
          info["update_date"] = row["update_date"].as<std::string>();
          info["update_user"] = row["update_user"].as<std::string>();
          info["delete"] = row["delete"].as<bool>();

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

void Car::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {}

void Car::detain(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto stmt = fmt::format("SELECT * FROM tow_car WHERE license "
                          "LIKE '%{}%' AND \"delete\" = false",
                          (*json)["plate"].asString());

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      stmt,
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];
          Json::Value info;
          info["id"] = row["id"].as<int64_t>();
          info["license"] = row["license"].as<std::string>();
          info["dc_causes"] = row["dc_causes"].as<std::string>();
          info["dc_date"] = row["dc_date"].as<std::string>();
          info["dc_address"] = row["dc_address"].as<std::string>();
          info["dc_into_date"] = row["dc_into_date"].as<std::string>();
          info["cmd_unit"] = row["cmd_unit"].as<std::string>();
          info["attachment"] = row["attachment"].as<Json::Value>();
          info["cv"] = row["cv"].as<std::string>();
          info["tv"] = row["tv"].as<std::string>();
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
} // namespace api
