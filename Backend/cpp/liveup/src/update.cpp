#include "include/update.h"

int Update::versionCompare(std::string v1, std::string v2) {
  // 0 - v1 = v2
  // 1 - v1 > v2
  // -1 - v1 < v2

  int vnum1 = 0, vnum2 = 0;

  for (int i = 0, j = 0; (i < v1.length() || j < v2.length());) {
    while (i < v1.length() && v1[i] != '.') {
      vnum1 = vnum1 * 10 + (v1[i] - '0');
      i++;
    }

    while (j < v2.length() && v2[j] != '.') {
      vnum2 = vnum2 * 10 + (v2[j] - '0');
      j++;
    }

    if (vnum1 > vnum2)
      return 1;
    if (vnum2 > vnum1)
      return -1;

    vnum1 = vnum2 = 0;
    i++;
    j++;
  }

  return 0;
}

bool Update::update() {
  bool result = true;

  Json::Value config;
  Json::Reader reader;
  std::ifstream is("config.json", std::ios::binary);
  reader.parse(is, config);
  is.close();

  teemo::Teemo::GlobalInit();
  teemo::Teemo efd;

  efd.setThreadNum(10);
  efd.setTmpFileExpiredTime(3600);

  auto split = config["url1"].asString().find_last_of("/") + 1;
  std::string file = config["url1"].asString().substr(
      split, config["url1"].asString().length() - split);
  common::utility::log(common::utility::LOGLEVEL::info, false, file);

  std::shared_future<teemo::Result> async_task =
      efd.start(config["url1"].asString(), fmt::format("temp/{}", file),
                nullptr, nullptr, nullptr);

  if (async_task.get() != teemo::Result::SUCCESSED) {
    result = updateBak();
  } else {
    Json::Value update;
    std::ifstream is("temp/update.json", std::ios::binary);
    reader.parse(is, update);
    is.close();

    if (versionCompare(config["version"].asString(),
                       update["version"].asString()) == -1) {
      auto files = update["files"];

      for (int i = 0; i < files.size(); i++) {
        auto file = files[i];

        common::utility::log(common::utility::LOGLEVEL::info, false, "{}/{}",
                             update["url"].asString(), file.asString());

        std::shared_future<teemo::Result> async_task = efd.start(
            fmt::format("{}/{}", update["url"].asString(), file.asString()),
            fmt::format("temp/{}", file.asString()), nullptr, nullptr, nullptr);

        if (async_task.get() != teemo::Result::SUCCESSED) {
          result = false;
          break;
        }
      }
    }
  }

  teemo::Teemo::GlobalUnInit();

  return result;
}

bool Update::updateBak() {
  bool result = true;

  Json::Value config;
  Json::Reader reader;
  std::ifstream is("config.json", std::ios::binary);
  reader.parse(is, config);
  is.close();

  teemo::Teemo::GlobalInit();
  teemo::Teemo efd;

  efd.setThreadNum(10);
  efd.setTmpFileExpiredTime(3600);

  auto split = config["url2"].asString().find_last_of("/") + 1;
  std::string file = config["url2"].asString().substr(
      split, config["url2"].asString().length() - split);
  common::utility::log(common::utility::LOGLEVEL::info, false, file);

  std::shared_future<teemo::Result> async_task =
      efd.start(config["url2"].asString(), fmt::format("temp/{}", file),
                nullptr, nullptr, nullptr);

  if (async_task.get() != teemo::Result::SUCCESSED) {
    result = updateBak();
  } else {
    Json::Value update;
    std::ifstream is("temp/update.json", std::ios::binary);
    reader.parse(is, update);
    is.close();

    if (versionCompare(config["version"].asString(),
                       update["version"].asString()) == -1) {
      auto files = update["files"];

      for (int i = 0; i < files.size(); i++) {
        auto file = files[i];

        common::utility::log(common::utility::LOGLEVEL::info, false, "{}/{}",
                             update["url"].asString(), file.asString());

        std::shared_future<teemo::Result> async_task = efd.start(
            fmt::format("{}/{}", update["url"].asString(), file.asString()),
            fmt::format("temp/{}", file.asString()), nullptr, nullptr, nullptr);

        if (async_task.get() != teemo::Result::SUCCESSED) {
          result = false;
          break;
        }
      }
    }
  }

  teemo::Teemo::GlobalUnInit();

  return result;
}

bool Update::execCopy() {
  Json::Reader reader;
  Json::Value update;
  std::ifstream is("temp/update.json", std::ios::binary);
  reader.parse(is, update);
  is.close();

  auto files = update["files"];

  for (int i = 0; i < files.size(); i++) {
    auto file = files[i];

    if (file) {
    }
  }

  return false;
}
