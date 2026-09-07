#include "websocket.h"
#include "include/utility.h"
#include "pch.h"

namespace api {
void Websocket::handleNewMessage(const WebSocketConnectionPtr &conn,
                                 std::string &&message,
                                 const WebSocketMessageType &type) {
  try {
    Json::Value info;
    Json::Reader reader;
    if (reader.parse(message, info)) {
      if (type == WebSocketMessageType::Text) {
        auto &s = conn->getContextRef<Subscriber>();

        m_subscriber.publish(s.topic, message);

        auto timestamp = common::utility::timeNow();

        if (info["topic"].asString().empty() ||
            info["subscriber"].asString().empty() ||
            info["type"].asString().empty() || info["msg"].asString().empty())
          return;

        auto db = app().getDbClient("postgresql-master");

        db->execSqlAsync(
            "INSERT INTO notify VALUES ($1, $2, $3, "
            "$4, 'unread', $5, $6, $7, $8, false, $9)",
            [=](const orm::Result &r) {},
            [=](const orm::DrogonDbException &e) {}, common::utility::uuid(),
            info["topic"].asString(), info["subscriber"].asString(),
            info["msg"].asString(), info["subscriber"].asString(), timestamp,
            info["subscriber"].asString(), timestamp, info["type"].asString());
      }
    }
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);
  }
}

void Websocket::handleConnectionClosed(const WebSocketConnectionPtr &conn) {
  try {
    auto &s = conn->getContextRef<Subscriber>();

    m_subscriber.unsubscribe(s.topic, s.id);
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);
  }
}

void Websocket::handleNewConnection(const HttpRequestPtr &req,
                                    const WebSocketConnectionPtr &conn) {
  try {
    if (req->getParameter("auth").empty() ||
        req->getParameter("token").empty() ||
        req->getParameter("topic").empty()) {
      conn->forceClose();
      return;
    }

    if (req->getParameter("auth") == "key") {
      if (req->getParameter("token") !=
          app().getCustomConfig()["auth"]["m2m"]["key"].asString()) {
        conn->forceClose();
        return;
      }
    }

    Subscriber s;
    s.topic = req->getParameter("topic");
    s.id = m_subscriber.subscribe(
        s.topic,
        [req, conn](const std::string &topic, const std::string &message) {
          req->addHeader("Sec-WebSocket-Protocol",
                         req->getHeader("Sec-WebSocket-Protocol"));
          (void)topic;
          conn->send(message);
        });
    conn->setContext(std::make_shared<Subscriber>(std::move(s)));
  } catch (const std::exception &e) {
    common::utility::log(common::utility::LOGLEVEL::info, false, "{} - {}:{}",
                         e.what(), __FILE__, __LINE__);
  }
}
} // namespace api
