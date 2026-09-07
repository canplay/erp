#pragma once

#include <drogon/PubSubService.h>
#include <drogon/WebSocketController.h>


using namespace drogon;

namespace api {
///
/// @brief Websocket消息通知
///
class Websocket : public WebSocketController<Websocket> {
public:
  WS_PATH_LIST_BEGIN
  WS_PATH_ADD("/api/ws/notify", Options, Get, "common::LogFilter");
  WS_PATH_LIST_END

public:
  virtual void handleNewMessage(const WebSocketConnectionPtr &conn,
                                std::string &&message,
                                const WebSocketMessageType &type) override;

  virtual void
  handleConnectionClosed(const WebSocketConnectionPtr &conn) override;

  virtual void handleNewConnection(const HttpRequestPtr &req,
                                   const WebSocketConnectionPtr &conn) override;

private:
  struct Subscriber {
    std::string topic;
    drogon::SubscriberID id;
  };

  PubSubService<std::string> m_subscriber;
};
} // namespace api
