#include "application.h"
#include "logger.h"
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#endif

#include <chrono>
#include <fstream>
#include <json/value.h>
#include <thread>

namespace common {
void Application::init(const std::string_view appName, bool grpc) {
#ifdef _WIN32
  // Windows特定的控制台设置
  auto hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(hStdin, &mode);
  mode &= ~ENABLE_QUICK_EDIT_MODE;
  mode &= ~ENABLE_INSERT_MODE;
  mode &= ~ENABLE_MOUSE_INPUT;
  SetConsoleMode(hStdin, mode);
#endif

  // 初始化日志系统
  common::Logger::getInstance().init("./logs", appName.data());
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "========== start server ==========");

  m_grpc_enabled = grpc;
}

void Application::run(const std::string_view configDebugPath,
                      const std::string_view configReleasePath) {
  // 启动gRPC服务（如果已初始化）
  if (m_grpc_builder && !m_grpc_running && m_grpc_enabled) {
    startGrpc();
  }

  try {
    // 设置自定义404页面
    Json::Value ret;
    ret["data"] = Json::nullValue;
    ret["message"] = "404";
    ret["status"] = 0;
    drogon::app().setCustom404Page(
        drogon::HttpResponse::newHttpJsonResponse(ret));

    // 设置CORS预处理，处理OPTIONS预检请求
    drogon::app().registerPreRoutingAdvice(
        [](const drogon::HttpRequestPtr &req, drogon::AdviceCallback &&callback,
           drogon::AdviceChainCallback &&chain) {
          if (req->method() == drogon::Options) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k204NoContent);
            resp->addHeader("Access-Control-Allow-Origin",
                            req->getHeader("Origin"));
            resp->addHeader("Access-Control-Allow-Methods", "*");
            resp->addHeader("Access-Control-Allow-Headers", "*");
            resp->addHeader("Access-Control-Expose-Headers", "*");
            resp->addHeader("Access-Control-Allow-Credentials", "true");
            resp->addHeader("Access-Control-Max-Age", "1728000");
            callback(resp);
            return;
          }
          chain();
        });

    // 设置CORS响应头
    drogon::app().registerPreSendingAdvice(
        [](const drogon::HttpRequestPtr &req,
           const drogon::HttpResponsePtr &resp) {
          resp->addHeader("Access-Control-Allow-Origin",
                          req->getHeader("Origin"));
          resp->addHeader("Access-Control-Allow-Methods", "*");
          resp->addHeader("Access-Control-Allow-Headers", "*");
          resp->addHeader("Access-Control-Expose-Headers", "*");
          resp->addHeader("Access-Control-Allow-Credentials", "true");
          resp->addHeader("Access-Control-Max-Age", "1728000");
        });

    // 加载配置文件
    std::string configPath;
#ifdef _DEBUG
    configPath = configDebugPath;
#else
    configPath = configReleasePath;
#endif

    drogon::app().loadConfigFile(configPath);

    // 手动读取配置文件获取监听地址和端口
    std::ifstream configFile(configPath);
    if (configFile.is_open()) {
      Json::Value configRoot;
      Json::Reader reader;
      if (reader.parse(configFile, configRoot)) {
        if (configRoot.isMember("listeners") &&
            configRoot["listeners"].isArray()) {
          const Json::Value &listeners = configRoot["listeners"];
          for (const auto &listener : listeners) {
            if (listener.isObject() && listener.isMember("address") &&
                listener.isMember("port")) {
              std::string address = listener["address"].asString();
              int port = listener["port"].asInt();
              common::Logger::getInstance().log(
                  common::Logger::LOGLEVEL::info, false,
                  "HTTP Server listening on {}:{}", address, port);
            }
          }
        }
      }
      configFile.close();
    } else {
      common::Logger::getInstance().log(common::Logger::LOGLEVEL::warn, false,
                                        "Failed to open config file: {}",
                                        configPath);
    }

    drogon::app().setIntSignalHandler([this]() {
      drogon::app().quit();
      Application::shutdown();
    });

    // 运行应用
    drogon::app().run();

    common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                      "HTTP Server Closed");
  } catch (const std::exception &e) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "fatal error: {}", e.what());
  } catch (...) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "unknown fatal error");
  }
}

void Application::startGrpc() {
  if (!m_grpc_builder || m_grpc_running) {
    return;
  }

  m_grpc_thread = std::make_unique<std::thread>([this]() {
    m_grpc_server = m_grpc_builder->BuildAndStart();
    if (m_grpc_server) {
      common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                        "gRPC Server listening on {}",
                                        m_grpc_address);
      m_grpc_running = true;
      m_grpc_server->Wait();
    } else {
      common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                        "Failed to start gRPC Server on {}",
                                        m_grpc_address);
    }
  });

  // 确保gRPC服务有时间初始化
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  if (!m_grpc_server) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                      "gRPC Server failed to start");
  } else {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                      "gRPC service started successfully");
  }
}

void Application::stopGrpc() {
  if (!m_grpc_running || !m_grpc_server) {
    return;
  }

  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "Shutting down gRPC service...");
  m_grpc_server->Shutdown();
  m_grpc_running = false;

  if (m_grpc_thread && m_grpc_thread->joinable()) {
    m_grpc_thread->join();
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                      "gRPC service closed");
  }
}

void Application::shutdown() {
  // 关闭gRPC服务
  stopGrpc();

  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "========== stop server ==========");
}
} // namespace common