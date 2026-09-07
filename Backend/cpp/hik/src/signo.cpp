#include "signo.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
Signo::Signo() {
  tags.insert(std::make_pair("示例花园地下停车场",
                             SignoTag{"PARK_PHONE_01", "PARK_KEY_01"}));
  tags.insert(
      std::make_pair("示例停车场B", SignoTag{"PARK_PHONE_02", "PARK_KEY_02"}));
  tags.insert(
      std::make_pair("示例停车场C", SignoTag{"PARK_PHONE_03", "PARK_KEY_03"}));
  tags.insert(
      std::make_pair("示例停车场D", SignoTag{"PARK_PHONE_04", "PARK_KEY_04"}));
  tags.insert(
      std::make_pair("示例停车场E", SignoTag{"PARK_PHONE_05", "PARK_KEY_05"}));
  tags.insert(
      std::make_pair("示例停车场F", SignoTag{"PARK_PHONE_06", "PARK_KEY_06"}));
  tags.insert(
      std::make_pair("盘龙示例站点E停车场", SignoTag{"PARK_PHONE_07", "PARK_KEY_07"}));
}

void Signo::open(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  Json::Value ret;

  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    ret["message"] = "参数错误";
    ret["status"] = 0;
    return callback(HttpResponse::newHttpJsonResponse(ret));
  }

  std::string place = (*json)["place"].asString();
  std::string march_no = (*json)["name"].asString();
  std::string park_no = tags.find(place)->second.phone;
  std::string pkey = tags.find(place)->second.key;

  std::string url = fmt::format("http://www.etpcar.com/App/"
                                "UpOpenCmd.aspx?ParkNo={}&CarNo=&CarType=&"
                                "MachNo={}&Memo=远程开闸OpenFlg=0",
                                park_no, march_no);

  std::transform(url.begin(), url.end(), url.begin(), tolower);
  std::transform(pkey.begin(), pkey.end(), pkey.begin(), tolower);
  std::string token = common::utility::md5(fmt::format("{}{}", url, pkey));
  url = fmt::format("{}&Token={}", url, token);

  auto ar = cpr::GetAsync(cpr::Url{url},
                          cpr::AcceptEncoding{{"deflate", "gzip", "zlib"}},
                          cpr::UserAgent{"CaNplay/1.0"}, cpr::Timeout{30000});
  auto res = ar.get();

  if (res.status_code == 200) {
    ret["message"] = res.text;
    ret["status"] = 1;
    callback(HttpResponse::newHttpJsonResponse(ret));
  } else {
    ret["message"] = "调用失败";
    ret["status"] = 0;
    callback(HttpResponse::newHttpJsonResponse(ret));
  }
}
} // namespace api
