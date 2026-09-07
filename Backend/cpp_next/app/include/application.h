#pragma once

#include "utils/singleton.h"
#include <drogon/drogon.h>
#include <grpcpp/grpcpp.h>

#include <memory>
#include <string>
#include <thread>

namespace common {
// 使用内联命名空间创建常量，避免宏定义
inline constexpr auto DEFAULT_GRPC_ADDRESS = "0.0.0.0:50051";
inline constexpr auto DEFAULT_CONFIG_DEBUG_PATH = "./config_debug.json";
inline constexpr auto DEFAULT_CONFIG_RELEASE_PATH = "./config.json";

/**
 * @brief 应用程序基类
 *
 * 提供通用的应用程序初始化和运行逻辑，避免重复代码
 * 使用现代C++特性优化性能和可读性
 */
class Application final : public Singleton<Application> {
  friend class Singleton<Application>;

public:
  void init(std::string_view appName, bool grpc = false);

  /**
   * @brief 运行应用程序
   * @param configDebugPath 调试配置文件路径
   * @param configReleasePath 发布配置文件路径
   */
  void run(std::string_view configDebugPath = DEFAULT_CONFIG_DEBUG_PATH,
           std::string_view configReleasePath = DEFAULT_CONFIG_RELEASE_PATH);

  void shutdown();

  /**
   * @brief 初始化gRPC服务
   * @param service gRPC服务实现
   * @param address 服务监听地址，默认为0.0.0.0:50051
   */
  template <typename ServiceType>
  void initGrpc(ServiceType *service,
                std::string_view address = DEFAULT_GRPC_ADDRESS) {
    m_grpc_address.assign(address.data(), address.size());
    m_grpc_builder = std::make_unique<grpc::ServerBuilder>();
    m_grpc_builder->AddListeningPort(m_grpc_address,
                                     grpc::InsecureServerCredentials());
    m_grpc_builder->RegisterService(service);
  }

  /**
   * @brief 启动gRPC服务
   */
  void startGrpc();

  /**
   * @brief 关闭gRPC服务
   */
  void stopGrpc();

private:
  std::unique_ptr<grpc::ServerBuilder> m_grpc_builder;
  std::unique_ptr<grpc::Server> m_grpc_server;
  std::unique_ptr<std::thread> m_grpc_thread;
  std::string m_grpc_address{DEFAULT_GRPC_ADDRESS};
  bool m_grpc_running = false;
  bool m_grpc_enabled = false;
};

} // namespace common