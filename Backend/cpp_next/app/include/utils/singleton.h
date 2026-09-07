#pragma once

#include <memory>

namespace common {

/**
 * @brief 通用单例模板类
 *
 * 使用方法：
 * @code
 * class MyClass : public common::Singleton<MyClass> {
 *     friend class common::Singleton<MyClass>;  // 声明友元以允许Singleton访问私有构造函数
 * private:
 *     MyClass() = default;  // 私有构造函数
 * public:
 *     void doSomething() { ... }
 * };
 * @endcode
 *
 * 调用方式：
 * @code
 * MyClass::getInstance().doSomething();
 * @endcode
 */
template <typename T> class Singleton {
public:
  /**
   * @brief 获取单例实例
   * @return 返回单例类的引用
   */
  static T &getInstance() {
    // 使用局部静态变量实现线程安全的单例（C++11标准保证）
    // 这是现代C++推荐的实现方式，既简洁又安全
    static T instance;
    return instance;
  }

  /**
   * @brief 获取单例实例的智能指针
   * @return 返回单例类的shared_ptr智能指针
   */
  static std::shared_ptr<T> getSharedInstance() {
    static std::shared_ptr<T> instance = std::make_shared<T>();
    return instance;
  }

protected:
  Singleton() = default;
  virtual ~Singleton() = default; // 虚析构函数，确保正确析构派生类

  // 禁止拷贝构造和赋值操作
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;
};

} // namespace common