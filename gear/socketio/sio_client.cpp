//
//  sio_client.h
//
//  Created by Melo Yao on 3/25/15.
//

#include "sio_client.h"
#include "internal/sio_client_impl.h"

using namespace websocketpp;
using asio::chrono::milliseconds;
using std::stringstream;

namespace sio {
client::client() : m_impl(std::make_unique<client_impl>()) {}

client::client(method method) : m_impl(std::make_unique<client_impl>(method)) {}

client::~client() = default;

void client::setOpenListener(connectionListener const& l) { m_impl->set_open_listener(l); }

void client::setFailListener(connectionListener const& l) { m_impl->set_fail_listener(l); }

void client::setCloseListener(closeListener const& l) { m_impl->set_close_listener(l); }

void client::setSocketOpenListener(socketListener const& l) { m_impl->set_socket_open_listener(l); }

void client::setReconnectListener(reconnectListener const& l) { m_impl->set_reconnect_listener(l); }

void client::setReconnectingListener(connectionListener const& l) {
  m_impl->set_reconnecting_listener(l);
}

void client::setSocketCloseListener(socketListener const& l) {
  m_impl->set_socket_close_listener(l);
}

void client::clearConnectionListeners() { m_impl->clear_con_listeners(); }

void client::clearSocketListeners() { m_impl->clear_socket_listeners(); }

void client::connect(const std::string& uri) {
  const std::map<string, string> query;
  m_impl->connect(uri, query);
}

void client::connect(const std::string& uri, const std::map<string, string>& query) {
  m_impl->connect(uri, query);
}

socket::ptr const& client::socket(const std::string& nsp) { return m_impl->socket(nsp); }

// Closes the connection
void client::close() { m_impl->close(); }

void client::syncClose() { m_impl->sync_close(); }

bool client::opened() const { return m_impl->opened(); }

std::string const& client::getSessionId() const { return m_impl->get_sessionid(); }

void client::setReconnectAttempts(int attempts) { m_impl->set_reconnect_attempts(attempts); }

void client::setReconnectDelay(unsigned millis) { m_impl->set_reconnect_delay(millis); }

void client::setReconnectDelayMax(unsigned millis) { m_impl->set_reconnect_delay_max(millis); }
}
