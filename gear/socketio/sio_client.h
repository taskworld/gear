//
//  sio_client.h
//
//  Created by Melo Yao on 3/25/15.
//

#ifndef SIO_CLIENT_H
#define SIO_CLIENT_H
#include "sio_message.h"
#include "sio_socket.h"
#include <websocketpp/config/asio_client.hpp>
#include <functional>
#include <string>

namespace sio {
class client_impl;

class client {
 public:
  enum close_reason { close_reason_normal, close_reason_drop };

  typedef std::function<void(void)> connectionListener;

  typedef std::function<void(close_reason const& reason)> closeListener;

  typedef std::function<void(unsigned, unsigned)> reconnectListener;

  typedef std::function<void(std::string const& nsp)> socketListener;

  client();
  client(asio::ssl::context::method);
  ~client();

  // set listeners and event bindings.
  void setOpenListener(connectionListener const& l);

  void setFailListener(connectionListener const& l);

  void setReconnectingListener(connectionListener const& l);

  void setReconnectListener(reconnectListener const& l);

  void setCloseListener(closeListener const& l);

  void setSocketOpenListener(socketListener const& l);

  void setSocketCloseListener(socketListener const& l);

  void clearConnectionListeners();

  void clearSocketListeners();

  // Client Functions - such as send, etc.
  void connect(const std::string& uri);

  void connect(const std::string& uri, const std::map<std::string, std::string>& query);

  void setReconnectAttempts(int attempts);

  void setReconnectDelay(unsigned millis);

  void setReconnectDelayMax(unsigned millis);

  sio::socket::ptr const& socket(const std::string& nsp = "");

  // Closes the connection
  void close();

  void syncClose();

  bool opened() const;

  std::string const& getSessionId() const;

 private:
  // disable copy constructor and assign operator.
  client(client const& cl) = delete;
  void operator=(client const& cl) = delete;

  unique_ptr<client_impl> m_impl;
};
}

#endif  // __SIO_CLIENT__H__
