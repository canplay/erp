#include "ccb.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Ccb::query(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  try {
    Json::Reader reader;
    Json::Value config = app().getCustomConfig()["pay"]["ccb"];

    auto data = fmt::format(
        "MERCHANTID={}&BRANCHID={}&POSID={}&ORDERDATE={}&BEGORDERTIME={}&"
        "ENDORDERTIME={}&ORDERID={}&QUPWD=&TXCODE={}&TYPE={}&KIND={}&STATUS={}&"
        "SEL_TYPE={}&PAGE={}&OPERATOR=&CHANNEL=",
        config["merchantid"].asString(), config["branchid"].asString(),
        (*json)["posid"].asString(), (*json)["date"].asString(),
        (*json)["time_start"].asString(), (*json)["time_end"].asString(),
        (*json)["order"].asString(), "410408", (*json)["type"].asString(),
        (*json)["kind"].asString(), (*json)["status"].asString(), "3",
        (*json)["page"].asString());

    auto mac = common::utility::md5(data);
    std::transform(mac.begin(), mac.end(), mac.begin(), tolower);

    data = fmt::format(
        "MERCHANTID={}&BRANCHID={}&POSID={}&ORDERDATE={}&BEGORDERTIME={}&"
        "ENDORDERTIME={}&ORDERID={}&QUPWD={}&TXCODE={}&TYPE={}&KIND={}&STATUS={"
        "}&SEL_TYPE={}&PAGE={}&OPERATOR=&CHANNEL=&MAC={}",
        config["merchantid"].asString(), config["branchid"].asString(),
        (*json)["posid"].asString(), (*json)["date"].asString(),
        common::utility::urlEncode((*json)["time_start"].asString()),
        common::utility::urlEncode((*json)["time_end"].asString()),
        (*json)["order"].asString(), config["qupwd"].asString(), "410408",
        (*json)["type"].asString(), (*json)["kind"].asString(),
        (*json)["status"].asString(), "3", (*json)["page"].asString(), mac);

    auto ar = cpr::GetAsync(
        cpr::Url{
            fmt::format("https://ibsbjstar.ccb.com.cn/CCBIS/ccbMain?{}", data)},
        cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
        cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
    auto res = ar.get();

    tinyxml2::XMLDocument doc;
    auto e = doc.Parse(res.text.c_str());

    if (e == tinyxml2::XML_SUCCESS) {
      auto root = doc.RootElement();

      Json::Value infos;
      auto val = root->FirstChildElement("RETURN_CODE")->GetText();
      infos["RETURN_CODE"] = val == 0 ? "" : val;

      val = root->FirstChildElement("RETURN_MSG")->GetText();
      infos["RETURN_MSG"] = val == 0 ? "" : val;

      if (infos["RETURN_CODE"].asString() == "000000") {
        val = root->FirstChildElement("CURPAGE")->GetText();
        infos["CURPAGE"] = val == 0 ? "" : val;

        val = root->FirstChildElement("PAGECOUNT")->GetText();
        infos["PAGECOUNT"] = val == 0 ? "" : val;

        val = root->FirstChildElement("TOTAL")->GetText();
        infos["TOTAL"] = val == 0 ? "" : val;

        val = root->FirstChildElement("PAYAMOUNT")->GetText();
        infos["PAYAMOUNT"] = val == 0 ? "" : val;

        val = root->FirstChildElement("REFUNDAMOUNT")->GetText();
        infos["REFUNDAMOUNT"] = val == 0 ? "" : val;

        auto query_order = root->FirstChildElement("QUERYORDER");
        while (query_order) {
          Json::Value it;

          val = query_order->FirstChildElement("POSID")->GetText();
          it["POSID"] = val == 0 ? "" : val;

          val = query_order->FirstChildElement("ORDERID")->GetText();
          it["ORDERID"] = val == 0 ? "" : val;

          val = query_order->FirstChildElement("ORDERDATE")->GetText();
          it["ORDERDATE"] = val == 0 ? "" : val;

          val = query_order->FirstChildElement("ACCDATE")->GetText();
          it["ACCDATE"] = val == 0 ? "" : val;

          val = query_order->FirstChildElement("AMOUNT")->GetText();
          it["AMOUNT"] = val == 0 ? "" : val;

          val = query_order->FirstChildElement("STATUSCODE")->GetText();
          it["STATUSCODE"] = val == 0 ? "" : val;

          val = query_order->FirstChildElement("STATUS")->GetText();
          it["STATUS"] = val == 0 ? "" : val;

          val = query_order->FirstChildElement("REFUND")->GetText();
          it["REFUND"] = val == 0 ? "" : val;

          infos["QUERYORDER"].append(it);
          query_order = query_order->NextSiblingElement();
        }
      }

      Json::Value ret;
      ret["data"] = infos;
      ret["message"] = "success";
      ret["status"] = 1;
      callback(HttpResponse::newHttpJsonResponse(ret));
    } else {
      Json::Value ret;
      ret["message"] = "参数错误";
      ret["status"] = 1;
      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } catch (const std::exception &e) {
    Json::Value ret;
    ret["message"] = e.what();
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Ccb::order(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  Json::Reader reader;
  Json::Value config = app().getCustomConfig()["pay"]["ccb"];

  auto data = fmt::format(
      "MERCHANTID={}&POSID={}&BRANCHID={}&ORDERID={}&PAYMENT={}&CURCODE=01&"
      "TXCODE=530550&REMARK1={}&REMARK2={}&RETURNTYPE=3&TIMEOUT=&PUB={}",
      config["merchantid"].asString(), config["posid"].asString(),
      config["branchid"].asString(), (*json)["order"].asString(),
      (*json)["amount"].asFloat(), (*json)["remark1"].asString(),
      (*json)["remark2"].asString(), config["pub"].asString());

  auto mac = common::utility::md5(data);
  std::transform(mac.begin(), mac.end(), mac.begin(), tolower);

  data = fmt::format(
      "CCB_IBSVersion=V6&MERCHANTID={}&POSID={}&BRANCHID={}&ORDERID={}&PAYMENT="
      "{}&CURCODE=01&TXCODE=530550&REMARK1={}&REMARK2={}&RETURNTYPE=3&TIMEOUT=&"
      "PROINFO={}&MAC={}",
      config["merchantid"].asString(), config["posid"].asString(),
      config["branchid"].asString(), (*json)["order"].asString(),
      (*json)["amount"].asFloat(), (*json)["remark1"].asString(),
      (*json)["remark2"].asString(), (*json)["goods"].asString(), mac);

  try {
    auto timestamp = common::utility::timeNow();

    auto ar = cpr::GetAsync(
        cpr::Url{
            fmt::format("https://ibsbjstar.ccb.com.cn/CCBIS/ccbMain?{}", data)},
        cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
        cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
    auto res = ar.get();

    if (res.status_code == 200) {
      Json::Value body1;
      reader.parse(res.text, body1);

      if (body1["SUCCESS"].asString() == "true") {
        auto ar =
            cpr::GetAsync(cpr::Url{body1["PAYURL"].asString()},
                          cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                          cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
        auto res = ar.get();

        Json::Value body2;
        reader.parse(res.text, body2);

        if (body2["SUCCESS"].asString() == "true") {
          auto db = app().getDbClient("postgresql-slave");
          db->execSqlAsync(
              "SELECT * FROM pay WHERE \"order\" = $1",
              [=](const orm::Result &r) {
                Json::Value ret;

                Json::Value create_params;
                create_params["data"] = data;
                create_params["return"] = body2;

                if (r.size() > 0) {
                  auto db = app().getDbClient("postgresql-master");
                  db->execSqlAsync(
                      "UPDATE pay SET order_pay = $1, status = 'generate', "
                      "\"type\" = 'ccb', amount = $2, remark = $3, "
                      "create_service = 'pay', create_params = $4, create_date "
                      "= $5, update_date = $6 WHERE \"order\" = $7",
                      [=](const orm::Result &r) {
                        Json::Value ret;

                        if (r.affectedRows() > 0) {
                          ret["message"] = body2["QRURL"].asString();
                          ret["status"] = 1;
                        } else {
                          ret["message"] = "更新失败";
                          ret["status"] = 0;
                        }

                        callback(HttpResponse::newHttpJsonResponse(ret));
                      },
                      [=](const orm::DrogonDbException &e) {
                        common::utility::log(
                            common::utility::LOGLEVEL::info, false,
                            "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

                        Json::Value ret;
                        ret["message"] = e.base().what();
                        ret["status"] = 0;

                        callback(HttpResponse::newHttpJsonResponse(ret));
                      },
                      (*json)["order_pay"].toStyledString(),
                      int((*json)["amount"].asFloat() * 100),
                      fmt::format("{}-{}-{}", (*json)["remark"].asString(),
                                  (*json)["remark1"].asString(),
                                  (*json)["remark2"].asString()),
                      create_params.toStyledString(),
                      (*json)["date"].asString(), timestamp,
                      (*json)["order"].asString());
                } else {
                  auto db = app().getDbClient("postgresql-master");
                  db->execSqlAsync(
                      "INSERT INTO pay VALUES ($1, $2, 'generate', 'ccb', $3, "
                      "$4, $5, 'pay', $6, $7, $8)",
                      [=](const orm::Result &r) {
                        Json::Value ret;

                        if (r.affectedRows() > 0) {
                          ret["message"] = body2["QRURL"].asString();
                          ret["status"] = 1;
                        } else {
                          ret["message"] = "新增失败";
                          ret["status"] = 0;
                        }

                        callback(HttpResponse::newHttpJsonResponse(ret));
                      },
                      [=](const orm::DrogonDbException &e) {
                        common::utility::log(
                            common::utility::LOGLEVEL::info, false,
                            "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

                        Json::Value ret;
                        ret["message"] = e.base().what();
                        ret["status"] = 0;

                        callback(HttpResponse::newHttpJsonResponse(ret));
                      },
                      common::utility::uuid(), (*json)["order"].asString(),
                      (*json)["order_pay"].toStyledString(),
                      int((*json)["amount"].asFloat() * 100),
                      fmt::format("{}-{}-{}", (*json)["remark"].asString(),
                                  (*json)["remark1"].asString(),
                                  (*json)["remark2"].asString()),
                      create_params.toStyledString(),
                      (*json)["date"].asString(), timestamp);
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
              (*json)["no"].asString());
        } else {
          Json::Value ret;
          ret["data"] = body2;
          ret["message"] = "success";
          ret["status"] = 0;
          callback(HttpResponse::newHttpJsonResponse(ret));
        }
      } else {
        Json::Value ret;
        ret["message"] = "订单支付失败";
        ret["status"] = 0;
        callback(HttpResponse::newHttpJsonResponse(ret));
      }
    } else {
      Json::Value ret;
      ret["message"] = "调用订单支付失败";
      ret["status"] = 0;
      callback(HttpResponse::newHttpJsonResponse(ret));
    }
  } catch (const std::exception &e) {
    Json::Value ret;
    ret["message"] = e.what();
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}

void Ccb::notify(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  auto timestamp = common::utility::timeNow();

  auto resp = HttpResponse::newHttpResponse();
  resp->setStatusCode(k400BadRequest);

  if (req->getParameter("SUCCESS") == "Y") {
    auto db = app().getDbClient("postgresql-slave");
    db->execSqlAsync(
        "SELECT id, order_pay, amount FROM pay WHERE \"order\" "
        "= $1",
        [=](const orm::Result &r) {
          auto row = r[0];

          if (r.size() > 0) {
            auto db = app().getDbClient("postgresql-master");
            db->execSqlAsync(
                "UPDATE pay SET status = 'paid', update_date = "
                "$1 WHERE \"order\" = $2",
                [=](const orm::Result &r) {
                  if (r.affectedRows() > 0) {
                    common::utility::log(common::utility::LOGLEVEL::info, false,
                                         "update pay: {}",
                                         row["id"].as<std::string>());

                    async_run([=]() -> Task<> {
                      try {
                        auto id = row["order_pay"].as<Json::Value>();
                        std::string topic = id["id"][0].asString();

                        float val = (float)row["amount"].as<int>() / 100;
                        common::utility::wsNotify(
                            topic, topic, "pay",
                            fmt::format("收到转账{:.2f}元", val));

                        Json::Value ret;
                        ret["message"] = "success";
                        ret["status"] = "1";

                        co_return callback(
                            HttpResponse::newHttpJsonResponse(ret));
                      } catch (const std::exception &e) {
                        common::utility::log(common::utility::LOGLEVEL::info,
                                             false, "{} - {}:{}", e.what(),
                                             __FILE__, __LINE__);
                        co_return callback(resp);
                      }
                    });

                    // Json::Value client_req =
                    // row["order_pay"].as<Json::Value>(); client_req["order"] =
                    // req->getParameter("ORDERID");

                    // httplib::Client
                    // client("https://erp.example.com");
                    // httplib::Headers headers = {
                    //	{ "Accept-Encoding", "gzip, deflate, br" },
                    //	{ "User-Agent", "CaNplay/1.0" },
                    //	{ "Authorization",
                    // app().getCustomConfig()["auth"]["m2m"]["key"].asString()
                    // },
                    // };
                    // client.set_connection_timeout(180);
                    // client.set_read_timeout(180);
                    // client.set_write_timeout(180);

                    // if (auto res =
                    // client.Post("/api/route/post?auth=key&service=cbis&path=/api/ticket/notify",
                    // headers, client_req.toStyledString(),
                    // "application/json"))
                    //{
                    //	if (res->status == 200)
                    //	{
                    //		Json::Reader reader;
                    //		Json::Value body;
                    //		if (reader.parse(res->body, body))
                    //		{
                    //			if (body["msg"].asString() == "success")
                    //			{
                    //				Json::Value ret;
                    //				ret["message"] = "success";
                    //				ret["status"] = "1";

                    //				return
                    // callback(HttpResponse::newHttpJsonResponse(ret));
                    //			}
                    //		}
                    //	}
                    //}
                  } else {
                    common::utility::log(common::utility::LOGLEVEL::info, false,
                                         "no update pay: {}",
                                         req->getParameter("ORDERID"));

                    callback(resp);
                  }
                },
                [=](const orm::DrogonDbException &e) {
                  common::utility::log(common::utility::LOGLEVEL::info, false,
                                       "{} {} - {}:{}",
                                       req->getParameter("ORDERID"),
                                       e.base().what(), __FILE__, __LINE__);

                  callback(resp);
                },
                timestamp, req->getParameter("ORDERID"));
          } else {
            callback(resp);
          }
        },
        [=](const orm::DrogonDbException &e) {
          common::utility::log(common::utility::LOGLEVEL::info, false,
                               "{} - {}:{}", e.base().what(), __FILE__,
                               __LINE__);

          callback(resp);
        },
        req->getParameter("ORDERID"));
  } else {
    callback(resp);
  }
}

void Ccb::close(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const {
  Json::Value ret;
  ret["message"] = "参数错误";
  ret["status"] = 0;

  callback(HttpResponse::newHttpJsonResponse(ret));
}

void Ccb::refund(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  Json::Value ret;
  ret["message"] = "参数错误";
  ret["status"] = 0;

  callback(HttpResponse::newHttpJsonResponse(ret));
}

void Ccb::info(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback) const {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    Json::Value ret;
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  auto db = app().getDbClient("postgresql-slave");
  db->execSqlAsync(
      "SELECT * FROM pay WHERE \"order\" = $1",
      [=](const orm::Result &r) {
        Json::Value ret;
        Json::Value info;

        if (r.size() > 0) {
          auto row = r[0];
          info = row["order_pay"].as<Json::Value>();
        }

        ret = info;
        ret["status"] = 1;
        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      [=](const orm::DrogonDbException &e) {
        common::utility::log(common::utility::LOGLEVEL::info, false,
                             "{} - {}:{}", e.base().what(), __FILE__, __LINE__);

        Json::Value ret;
        ret["message"] = "查找支付记录失败";
        ret["status"] = 0;

        callback(HttpResponse::newHttpJsonResponse(ret));
      },
      (*json)["order"].asString());
}
} // namespace api
