#include "entt_system.h"
#include "logger.h"
#include <algorithm>
#include <functional>

namespace common {

void EnTTSystem::Init() {
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "[ECS] 初始化ECS系统...");
}

entt::entity EnTTSystem::CreateEntity() {
  entt::entity entity = registry_.create();
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                    "[ECS] 创建实体: {}",
                                    static_cast<std::uint32_t>(entity));
  return entity;
}

void EnTTSystem::DestroyEntity(entt::entity entity) {
  if (IsValid(entity)) {
    registry_.destroy(entity);
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                      "[ECS] 销毁实体: {}",
                                      static_cast<std::uint32_t>(entity));
  }
}

bool EnTTSystem::IsValid(entt::entity entity) const {
  return registry_.valid(entity);
}

void EnTTSystem::RegisterComponentType(
    const std::string &name, std::function<void(entt::entity)> addFunc,
    std::function<void(entt::entity)> removeFunc,
    std::function<void *(entt::entity)> getFunc) {
  addComponentFunctions_[name] = std::move(addFunc);
  removeComponentFunctions_[name] = std::move(removeFunc);
  getComponentFunctions_[name] = std::move(getFunc);

  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "[ECS] 注册组件类型: {}", name);
}

void *EnTTSystem::GetComponentByType(entt::entity entity,
                                     const std::string &typeName) {
  // 首先检查实体是否有效
  if (!IsValid(entity)) {
    return nullptr;
  }

  auto it = getComponentFunctions_.find(typeName);
  if (it != getComponentFunctions_.end()) {
    return it->second(entity);
  }
  return nullptr;
}

void EnTTSystem::AddComponentByType(entt::entity entity,
                                    const std::string &typeName) {
  // 首先检查实体是否有效
  if (!IsValid(entity)) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::warn, false,
                                      "[ECS] 无法为无效实体添加组件");
    return;
  }

  auto it = addComponentFunctions_.find(typeName);
  if (it != addComponentFunctions_.end()) {
    it->second(entity);
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                      "[ECS] 为实体 {} 添加组件类型 {}",
                                      static_cast<std::uint32_t>(entity),
                                      typeName);
  }
}

void EnTTSystem::RemoveComponentByType(entt::entity entity,
                                       const std::string &typeName) {
  // 首先检查实体是否有效
  if (!IsValid(entity)) {
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::warn, false,
                                      "[ECS] 无法从无效实体移除组件");
    return;
  }

  auto it = removeComponentFunctions_.find(typeName);
  if (it != removeComponentFunctions_.end()) {
    it->second(entity);
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                      "[ECS] 从实体 {} 移除组件类型 {}",
                                      static_cast<std::uint32_t>(entity),
                                      typeName);
  }
}

bool EnTTSystem::HasComponentByType(entt::entity entity,
                                    const std::string &typeName) const {
  // 检查实体是否有效
  if (!IsValid(entity)) {
    return false;
  }

  // 查找对应的检查函数
  auto it = getComponentFunctions_.find(typeName);
  if (it != getComponentFunctions_.end()) {
    // 尝试获取组件，如果不为空则表示拥有该组件
    return it->second(entity) != nullptr;
  }
  return false;
}

void EnTTSystem::Update([[maybe_unused]] float deltaTime) {
  // 执行所有注册的系统更新函数
  // 这里可以添加通用的ECS系统更新逻辑

  // 日志记录
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::trace, false,
                                    "[ECS] 更新系统，deltaTime: {}", deltaTime);

  // 示例：可以在此处添加基于组件的系统更新逻辑
  // 比如：
  // 1. 物理系统更新 - 更新带有物理组件的实体
  // registry_.view<TransformComponent, PhysicsComponent>().each(
  //     [deltaTime](auto entity, auto &transform, auto &physics) {
  //       // 更新位置基于速度和时间
  //       transform.position.x += physics.velocity.x * deltaTime;
  //       transform.position.y += physics.velocity.y * deltaTime;
  //       transform.position.z += physics.velocity.z * deltaTime;
  //     });
  //
  // 2. 生命周期系统更新 - 处理带有生命周期组件的实体
  // registry_.view<LifetimeComponent>().each(
  //     [this, deltaTime](auto entity, auto &lifetime) {
  //       lifetime.remaining -= deltaTime;
  //       if (lifetime.remaining <= 0.0f) {
  //         this->DestroyEntity(entity);
  //       }
  //     });
}

} // namespace common