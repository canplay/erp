#include "actor_system.h"
#include "logger.h"
#include <algorithm>
#include <chrono>

namespace common {
ActorSystem::ActorSystem() : running_(false) {
  last_update_time_ = std::chrono::steady_clock::now();
}

ActorSystem::~ActorSystem() {
  if (running_.load()) {
    shutdown();
  }
}

void ActorSystem::init() {
  if (running_.load()) {
    return; // 已经初始化
  }

  running_.store(true);
  last_update_time_ = std::chrono::steady_clock::now();

  // 创建工作线程
  const size_t num_threads = std::max(1u, std::thread::hardware_concurrency());
  worker_threads_.reserve(num_threads);
  for (size_t i = 0; i < num_threads; ++i) {
    worker_threads_.emplace_back(&ActorSystem::workerThread, this);
  }

  common::Logger::getInstance().log(
      common::Logger::LOGLEVEL::info, false,
      "[Actor] Actor系统初始化完成，创建了{}个工作线程", num_threads);
}

void ActorSystem::shutdown() {
  if (!running_.load()) {
    return; // 已经关闭
  }

  running_.store(false);

  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    condition_.notify_all();
  }

  for (auto &thread : worker_threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  worker_threads_.clear();
  actors_.clear(); // 清理所有Actor

  // 清理消息队列
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    decltype(message_queue_)().swap(message_queue_);
    decltype(delayed_messages_)().swap(delayed_messages_);
    timers_.clear();
  }

  common::Logger::getInstance().log(common::Logger::LOGLEVEL::info, false,
                                    "[Actor] Actor系统已关闭");
}

Entity ActorSystem::createActor(
    std::function<std::unique_ptr<Actor>(Entity)> factory) {
  auto entity = registry_.create();
  auto actor = factory(entity);
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    actors_[entity] = std::move(actor);
  }
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                    "[Actor] 创建Actor实体: {}",
                                    static_cast<uint32_t>(entity));
  return entity;
}

void ActorSystem::destroyActor(Entity entity) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  auto it = actors_.find(entity);
  if (it != actors_.end()) {
    actors_.erase(it);
    registry_.destroy(entity);
    common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                      "[Actor] 销毁Actor实体: {}",
                                      static_cast<uint32_t>(entity));
  }
}

bool ActorSystem::hasActor(Entity entity) const {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  return actors_.find(entity) != actors_.end();
}

void ActorSystem::send(Entity receiver, std::shared_ptr<Message> msg) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    message_queue_.emplace(receiver, std::move(msg));
  }
  condition_.notify_one();
}

void ActorSystem::sendDelayed(Entity receiver, std::shared_ptr<Message> msg,
                              float delaySeconds) {
  auto deliveryTime =
      std::chrono::steady_clock::now() +
      std::chrono::milliseconds(static_cast<int>(delaySeconds * 1000));

  DelayedMessage delayedMsg;
  delayedMsg.receiver = receiver;
  delayedMsg.msg = std::move(msg);
  delayedMsg.deliveryTime = deliveryTime;

  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    delayed_messages_.push(std::move(delayedMsg));
  }
}

void ActorSystem::sendPeriodic(Entity receiver, std::shared_ptr<Message> msg,
                               float intervalSeconds,
                               const std::string &timerId) {
  auto now = std::chrono::steady_clock::now();
  auto nextFireTime =
      now + std::chrono::milliseconds(static_cast<int>(intervalSeconds * 1000));

  TimerInfo timerInfo;
  timerInfo.receiver = receiver;
  timerInfo.msg = std::move(msg);
  timerInfo.interval = intervalSeconds;
  timerInfo.nextFireTime = nextFireTime;
  timerInfo.active = true;

  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    timers_.insert_or_assign(timerId, std::move(timerInfo));
  }
}

void ActorSystem::cancelTimer(const std::string &timerId) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  auto it = timers_.find(timerId);
  if (it != timers_.end()) {
    timers_.erase(it);
  }
}

void ActorSystem::workerThread() {
  // 为每个线程设置线程名称，便于调试
  static std::atomic<uint32_t> thread_counter{0};
  uint32_t thread_id = thread_counter.fetch_add(1);
  common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                    "[Actor] 启动Actor工作线程 #{}", thread_id);

  while (running_.load()) {
    std::unique_lock<std::mutex> lock(queue_mutex_);

    // 等待消息或关闭信号
    condition_.wait_for(lock, std::chrono::milliseconds(100), [this] {
      return !message_queue_.empty() || !running_.load();
    });

    if (message_queue_.empty()) {
      if (!running_.load()) {
        break;
      }
      continue; // 超时或虚假唤醒
    }

    auto message = std::move(message_queue_.front());
    message_queue_.pop();
    lock.unlock();

    // 处理消息
    auto receiver = message.first;
    auto msg = std::move(message.second);

    std::unique_ptr<Actor> *actor = nullptr;
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      auto it = actors_.find(receiver);
      if (it != actors_.end() && it->second->isActive()) {
        actor = &it->second;
      }
    }

    if (actor) {
      try {
        (*actor)->receive(std::move(msg));
      } catch (const std::exception &e) {
        common::Logger::getInstance().log(
            common::Logger::LOGLEVEL::error, false,
            "[Actor] Actor消息处理异常: {}", e.what());
      } catch (...) {
        common::Logger::getInstance().log(common::Logger::LOGLEVEL::error,
                                          false,
                                          "[Actor] Actor消息处理未知异常");
      }
    } else {
      common::Logger::getInstance().log(common::Logger::LOGLEVEL::warn, false,
                                        "[Actor] 未找到接收者实体: {}",
                                        static_cast<uint32_t>(receiver));
    }
  }

  common::Logger::getInstance().log(common::Logger::LOGLEVEL::debug, false,
                                    "[Actor] 退出Actor工作线程 #{}", thread_id);
}

void ActorSystem::processDelayedMessages() {
  auto now = std::chrono::steady_clock::now();

  std::lock_guard<std::mutex> lock(queue_mutex_);
  while (!delayed_messages_.empty() &&
         delayed_messages_.top().deliveryTime <= now) {
    auto delayedMsg = delayed_messages_.top();
    delayed_messages_.pop();

    message_queue_.emplace(delayedMsg.receiver, std::move(delayedMsg.msg));
  }
}

void ActorSystem::processPeriodicTimers(float deltaTime) {
  auto now = std::chrono::steady_clock::now();

  std::lock_guard<std::mutex> lock(queue_mutex_);
  for (auto &[id, timerInfo] : timers_) {
    if (timerInfo.active && timerInfo.nextFireTime <= now) {
      // 发送消息
      message_queue_.emplace(timerInfo.receiver, timerInfo.msg);

      // 更新下次触发时间
      timerInfo.nextFireTime = now + std::chrono::milliseconds(static_cast<int>(
                                         timerInfo.interval * 1000));
    }
  }
}

void ActorSystem::update(float deltaTime) {
  // 处理延迟消息
  processDelayedMessages();

  // 处理定时器
  processPeriodicTimers(deltaTime);

  // 更新所有活跃的Actor
  std::vector<std::pair<Entity, Actor *>> active_actors;
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    active_actors.reserve(actors_.size());
    for (const auto &[entity, actor] : actors_) {
      if (actor->isActive()) {
        active_actors.emplace_back(entity, actor.get());
      }
    }
  }

  for (const auto &[entity, actor] : active_actors) {
    try {
      actor->update(deltaTime);
    } catch (const std::exception &e) {
      common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                        "[Actor] Actor({}) 更新异常: {}",
                                        static_cast<uint32_t>(entity),
                                        e.what());
    } catch (...) {
      common::Logger::getInstance().log(common::Logger::LOGLEVEL::error, false,
                                        "[Actor] Actor({}) 更新未知异常",
                                        static_cast<uint32_t>(entity));
    }
  }
}
} // namespace common