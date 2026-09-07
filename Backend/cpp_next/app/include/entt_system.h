#pragma once

#include "utils/singleton.h"

#include <entt/entt.hpp>
#include <functional>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace common {
// 组件注册回调类型
using ComponentRegistrationCallback = std::function<void()>;

class EnTTSystem : public common::Singleton<EnTTSystem> {
  friend class common::Singleton<EnTTSystem>;

public:
  void Init();
  entt::registry &GetRegistry() { return registry_; }

  // 实体管理
  entt::entity CreateEntity();
  void DestroyEntity(entt::entity entity);
  [[nodiscard]] bool IsValid(entt::entity entity) const;

  // 组件管理
  template <typename T> void RegisterComponent(const std::string &name);
  template <typename T, typename... Args>
  T &AddComponent(entt::entity entity, Args &&...args);
  template <typename T> T &GetComponent(entt::entity entity);
  template <typename T>
  [[nodiscard]] bool HasComponent(entt::entity entity) const;
  template <typename T> void RemoveComponent(entt::entity entity);

  // 组件注册（用于Luau绑定）
  void RegisterComponentType(const std::string &name,
                             std::function<void(entt::entity)> addFunc,
                             std::function<void(entt::entity)> removeFunc,
                             std::function<void *(entt::entity)> getFunc);

  // 组件操作（用于Luau绑定）
  void *GetComponentByType(entt::entity entity, const std::string &typeName);
  void AddComponentByType(entt::entity entity, const std::string &typeName);
  void RemoveComponentByType(entt::entity entity, const std::string &typeName);
  [[nodiscard]] bool HasComponentByType(entt::entity entity,
                                        const std::string &typeName) const;

  // 系统更新
  void Update(float deltaTime);

  // 获取组件注册映射
  [[nodiscard]] const std::unordered_map<std::string,
                                         ComponentRegistrationCallback> &
  GetComponentRegistrations() const {
    return componentRegistrations_;
  }

private:
  EnTTSystem() = default;
  ~EnTTSystem() = default;

  entt::registry registry_;

  // 组件类型映射
  std::unordered_map<std::string, std::type_index> componentTypes_;

  // 组件注册回调映射
  std::unordered_map<std::string, ComponentRegistrationCallback>
      componentRegistrations_;

  // 组件操作函数映射（用于Luau绑定）
  std::unordered_map<std::string, std::function<void(entt::entity)>>
      addComponentFunctions_;
  std::unordered_map<std::string, std::function<void(entt::entity)>>
      removeComponentFunctions_;
  std::unordered_map<std::string, std::function<void *(entt::entity)>>
      getComponentFunctions_;
};

// 模板实现
template <typename T>
void EnTTSystem::RegisterComponent(const std::string &name) {
  componentTypes_[name] = std::type_index(typeid(T));
  componentRegistrations_[name] = [this]() {
    // 注册组件到registry
    registry_.template emplace<T>();
  };
}

template <typename T, typename... Args>
T &EnTTSystem::AddComponent(entt::entity entity, Args &&...args) {
  return registry_.emplace<T>(entity, std::forward<Args>(args)...);
}

template <typename T> T &EnTTSystem::GetComponent(entt::entity entity) {
  if (!HasComponent<T>(entity)) {
    throw std::runtime_error("Entity does not have the requested component");
  }
  return registry_.get<T>(entity);
}

template <typename T> bool EnTTSystem::HasComponent(entt::entity entity) const {
  return registry_.all_of<T>(entity);
}

template <typename T> void EnTTSystem::RemoveComponent(entt::entity entity) {
  if (HasComponent<T>(entity)) {
    registry_.remove<T>(entity);
  }
}

} // namespace common