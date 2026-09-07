#include "include/utility.h"
#include "pch.h"

using namespace drogon;

int main() {
#ifdef _WIN32
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(hStdin, &mode);
  mode &= ~ENABLE_QUICK_EDIT_MODE;
  mode &= ~ENABLE_INSERT_MODE;
  mode &= ~ENABLE_MOUSE_INPUT;
  SetConsoleMode(hStdin, mode);
#endif

  common::utility::initLogger("./logs", "pay");
  common::utility::log(common::utility::LOGLEVEL::info, false,
                       "========== start server ==========");

  try {
    Json::Value ret;
    ret["message"] = "404";
    ret["status"] = 0;
    app().setCustom404Page(HttpResponse::newHttpJsonResponse(ret));

    app().registerPreSendingAdvice([](const HttpRequestPtr &req,
                                      const HttpResponsePtr &resp) {
      resp->addHeader("Access-Control-Allow-Origin", req->getHeader("origin"));
      resp->addHeader("Access-Control-Allow-Methods",
                      "OPTIONS, POST, PUT, GET, DELETE");
      resp->addHeader(
          "Access-Control-Allow-Headers",
          "Origin, Content-Type, Authorization, X-Requested-With, "
          "X-Cookie, X-Forwarded-For, X-Forwarded-For, "
          "HTTP_X_FORWARDED_FOR, cache-control, Baggage, sentry-trace");
      resp->addHeader("Access-Control-Allow-Credentials", "true");
      resp->addHeader("Access-Control-Max-Age", "1728000");
    });

#ifdef _DEBUG
    app().loadConfigFile("./config_debug.json");
#else
    app().loadConfigFile("./config.json");
#endif

    app().run();
  } catch (...) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "fatal error");
  }

  common::utility::log(common::utility::LOGLEVEL::info, false,
                       "========== stop server ==========");
  common::utility::closeLogger();
  return 0;
}
