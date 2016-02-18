#pragma once

#include <chrono>
#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

using namespace std;

using client = websocketpp::client<websocketpp::config::asio_tls_client>;

namespace gear {
class WebSocketEndpoint;

class ConnectionMetaData final {
 public:
  enum class status { connecting = 0, open, fail, close };

  ConnectionMetaData(websocketpp::connection_hdl hdl, string uri);

  void onOpened(client *c, websocketpp::connection_hdl hdl, int retry_attempt,
                function<void(client::connection_ptr)> onOpened,
                function<void(client::connection_ptr)> onRetryOpened);

  void onFailed(client *c, websocketpp::connection_hdl hdl,
                function<void(client::connection_ptr)> onFailed);

  void onClosed(client *c, websocketpp::connection_hdl hdl, WebSocketEndpoint *endpoint,
                function<void(client::connection_ptr)> onClosed);

  void onMessageReceived(
      websocketpp::connection_hdl, client::message_ptr msg,
      function<void(websocketpp::frame::opcode::value, string)> onMessageReceived);

  websocketpp::connection_hdl getHandler();

  status getStatus() const;

  friend ostream &operator<<(ostream &out, ConnectionMetaData const &data);

 private:
  websocketpp::connection_hdl _hdl;
  status _status{status::connecting};
  string _uri;
  string _server{"N/A"};
  string _errorReason;
};

class WebSocketEndpoint final {
  friend class ConnectionMetaData;

 public:
  WebSocketEndpoint();

  ~WebSocketEndpoint();

  WebSocketEndpoint &withTls(websocketpp::transport::asio::tls_socket::tls_init_handler handler);

  WebSocketEndpoint &onOpened(function<void(client::connection_ptr)> handler);

  WebSocketEndpoint &onClosed(function<void(client::connection_ptr)> handler);

  WebSocketEndpoint &onFailed(function<void(client::connection_ptr)> handler);

  WebSocketEndpoint &onRetryStarted(function<void(int)> handler);

  WebSocketEndpoint &onRetryOpened(function<void(client::connection_ptr)> handler);

  WebSocketEndpoint &onMessageReceived(
      function<void(websocketpp::frame::opcode::value, string)> handler);

  void connect(string const &uri);

  void close(websocketpp::close::status::value code, string reason);

  void send(string message);

  ConnectionMetaData *getMetaData() const;

 private:
  client _endpoint;
  unique_ptr<thread> _thread;
  int _retryAttemptyCount;

  shared_ptr<ConnectionMetaData> _connection;

  function<void(client::connection_ptr)> _openHandler;
  function<void(client::connection_ptr)> _failHandler;
  function<void(client::connection_ptr)> _closeHandler;
  function<void(int)> _retryStartedHandler;
  function<void(client::connection_ptr)> _retryOpenedHandler;
  function<void(websocketpp::frame::opcode::value, string)> _messageHandler;

  void retry(string const &uri);
};
}
