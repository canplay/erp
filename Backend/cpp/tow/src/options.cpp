#include "options.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Options::CarClass(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM tow_car_class",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<int8_t>();
          info["cpt"] = row["cpt"].as<std::string>();
          info["remark"] = row["remark"].as<std::string>();
          info["free_time"] = row["free_time"].as<int8_t>();
          info["hm10"] = row["hm10"].as<std::string>();
          info["hm24"] = row["hm24"].as<std::string>();
          info["gratis_day"] = row["gratis_day"].as<int8_t>();
          info["cost_day"] = row["cost_day"].as<std::string>();

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

void Options::CarType(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM tow_car_type",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<int8_t>();
          info["type"] = row["type"].as<std::string>();
          info["ccid"] = row["ccid"].as<int8_t>();
          info["py"] = row["py"].as<std::string>();
          info["cost"] = row["cost"].as<std::string>();
          info["index"] = row["index"].as<int8_t>();
          info["dc_start_m"] = row["dc_start_m"].as<std::string>();
          info["dc_start_k"] = row["dc_start_k"].as<int8_t>();
          info["dc_bstart_m"] = row["dc_bstart_m"].as<std::string>();
          info["p_start_m"] = row["p_start_m"].as<std::string>();
          info["p_start_h"] = row["p_start_h"].as<int8_t>();
          info["p_bstart_m"] = row["p_bstart_m"].as<std::string>();
          info["p_bstart_dm"] = row["p_bstart_dm"].as<std::string>();

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

void Options::CarColor(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM tow_car_color",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<int8_t>();
          info["cpt"] = row["cpt"].as<std::string>();
          info["py"] = row["py"].as<std::string>();
          info["index"] = row["index"].as<int8_t>();

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

void Options::CausesType(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM tow_dc_type",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<int8_t>();
          info["cpt"] = row["cpt"].as<std::string>();
          info["unit_id"] = row["unit_id"].as<std::string>();
          info["py"] = row["py"].as<std::string>();
          info["index"] = row["index"].as<int8_t>();

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

void Options::Causes(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM tow_dc_causes",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value infos;

        for (size_t i = 0; i < r.size(); i++) {
          auto row = r[i];

          Json::Value info;
          info["id"] = row["id"].as<int8_t>();
          info["cpt"] = row["cpt"].as<std::string>();
          info["index"] = row["index"].as<int8_t>();
          info["dct_id"] = row["dct_id"].as<int8_t>();
          info["py"] = row["py"].as<std::string>();

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
