#pragma once

#include "utils/singleton.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

namespace common {

// 前向声明
class Actor;

// 类型别名 - 使用现代C++风格
using Entity = entt::entity;
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;

// 消息基类
struct Message {
  Entity sender;
  TimePoint timestamp;

  Message() : sender(entt::null), timestamp(std::chrono::steady_clock::now()) {}
  Message(Entity sender_entity)
      : sender(sender_entity), timestamp(std::chrono::steady_clock::now()) {}
  virtual ~Message() = default;
};

// 延迟消息结构
struct DelayedMessage {
  Entity receiver;
  std::shared_ptr<Message> msg;
  TimePoint deliveryTime;

  // 用于优先队列的比较操作符
  bool operator>(const DelayedMessage &other) const {
    return deliveryTime > other.deliveryTime;
  }
};

// 定时器信息结构
struct TimerInfo {
  Entity receiver;
  std::shared_ptr<Message> msg;
  float interval;
  TimePoint nextFireTime;
  bool active{true};
};

// Actor基类
class Actor {
public:
  explicit Actor(Entity entity) : entity_(entity), active_(true) {}
  virtual ~Actor() = default;

  // 使用override关键字明确重写
  virtual void receive([[maybe_unused]] std::shared_ptr<Message> msg) {}
  virtual void update([[maybe_unused]] float deltaTime) {}

  [[nodiscard]] Entity getEntity() const noexcept { return entity_; }

  void setActive(bool active) noexcept { active_ = active; }
  [[nodiscard]] bool isActive() const noexcept { return active_; }

private:
  Entity entity_;
  std::atomic<bool> active_;
};

// Actor系统类
class ActorSystem final : public Singleton<ActorSystem> {
  friend class Singleton<ActorSystem>;

public:
  ActorSystem();
  ~ActorSystem();

  // 禁止拷贝和赋值
  ActorSystem(const ActorSystem &) = delete;
  ActorSystem &operator=(const ActorSystem &) = delete;

  void init();
  void shutdown();
  void update(float deltaTime);

  // Actor管理
  Entity createActor(std::function<std::unique_ptr<Actor>(Entity)> factory);
  void destroyActor(Entity entity);
  [[nodiscard]] bool hasActor(Entity entity) const;

  // 消息发送
  void send(Entity receiver, std::shared_ptr<Message> msg);
  void sendDelayed(Entity receiver, std::shared_ptr<Message> msg,
                   float delaySeconds);
  void sendPeriodic(Entity receiver, std::shared_ptr<Message> msg,
                    float intervalSeconds, const std::string &timerId);

  // 取消定时器
  void cancelTimer(const std::string &timerId);

private:
  void workerThread();
  void processDelayedMessages();
  void processPeriodicTimers(float deltaTime);

  // EnTT注册表
  entt::registry registry_;

  // Actor映射
  std::unordered_map<Entity, std::unique_ptr<Actor>> actors_;

  // 消息队列
  std::queue<std::pair<Entity, std::shared_ptr<Message>>> message_queue_;
  std::priority_queue<DelayedMessage, std::vector<DelayedMessage>,
                      std::greater<DelayedMessage>>
      delayed_messages_;
  std::unordered_map<std::string, TimerInfo> timers_;

  // 线程相关
  std::vector<std::thread> worker_threads_;
  mutable std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::atomic<bool> running_;
  TimePoint last_update_time_;
};

} // namespace common