#include "actor_system.h"
#include "application.h"
#include "entt_system.h"
#include "logger.h"

int main() {
  common::Application::getInstance().init("app");

  // 初始化ECS系统
  common::EnTTSystem::getInstance().Init();

  // 初始化Actor系统
  common::ActorSystem::getInstance().init();

  common::Application::getInstance().run();
  common::Application::getInstance().shutdown();

  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "应用已退出");
  return 0;
}