#pragma once

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

using namespace std;

using client = websocketpp::client<websocketpp::config::asio_tls_client>;

namespace gear {
class connection_metadata final {
public:
  enum class status { connecting = 0, open, fail, close };

  connection_metadata(websocketpp::connection_hdl hdl, string uri);

  void on_open(client *c, websocketpp::connection_hdl hdl,
               function<void(client::connection_ptr)> f);

  void on_fail(client *c, websocketpp::connection_hdl hdl,
               function<void(client::connection_ptr)> f);

  void on_close(client *c, websocketpp::connection_hdl hdl,
                function<void(client::connection_ptr)> f);

  void on_message(websocketpp::connection_hdl, client::message_ptr msg,
                  function<void(websocketpp::frame::opcode::value, string)> f);

  websocketpp::connection_hdl get_hdl();

  status get_status() const;

  friend ostream &operator<<(ostream &out, connection_metadata const &data);

private:
  websocketpp::connection_hdl _hdl;
  status _status{status::connecting};
  string _uri;
  string _server{"N/A"};
  string _error_reason;
};

class websocket_endpoint final {
public:
  websocket_endpoint();

  ~websocket_endpoint();

  websocket_endpoint &
  with_tls(websocketpp::transport::asio::tls_socket::tls_init_handler handler);

  websocket_endpoint &on_open(function<void(client::connection_ptr)> handler);

  websocket_endpoint &on_close(function<void(client::connection_ptr)> handler);

  websocket_endpoint &on_fail(function<void(client::connection_ptr)> handler);

  websocket_endpoint &
  on_message(function<void(websocketpp::frame::opcode::value, string)> handler);

  void connect(string const &uri);

  void close(websocketpp::close::status::value code, string reason);

  void send(string message);

  connection_metadata *get_metadata() const;

private:
  client _endpoint;
  unique_ptr<thread> _thread;

  shared_ptr<connection_metadata> _connection;

  function<void(client::connection_ptr)> _open_handler;
  function<void(client::connection_ptr)> _fail_handler;
  function<void(client::connection_ptr)> _close_handler;
  function<void(websocketpp::frame::opcode::value, string)> _message_handler;
};
}
