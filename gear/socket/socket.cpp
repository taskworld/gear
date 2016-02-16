#include "socket.hpp"

namespace gear {
connection_metadata::connection_metadata(websocketpp::connection_hdl hdl,
                                         string uri)
    : _hdl(hdl), _uri(uri) {}

void connection_metadata::on_open(client *c, websocketpp::connection_hdl hdl,
                                  function<void(client::connection_ptr)> f) {
  _status = status::open;

  client::connection_ptr con = c->get_con_from_hdl(hdl);
  _server = con->get_response_header("Server");
  f(con);
}

void connection_metadata::on_fail(client *c, websocketpp::connection_hdl hdl,
                                  function<void(client::connection_ptr)> f) {
  _status = status::fail;

  client::connection_ptr con = c->get_con_from_hdl(hdl);
  _server = con->get_response_header("Server");
  _error_reason = con->get_ec().message();
  f(con);
}

void connection_metadata::on_close(client *c, websocketpp::connection_hdl hdl,
                                   function<void(client::connection_ptr)> f) {
  _status = status::close;
  client::connection_ptr con = c->get_con_from_hdl(hdl);
  stringstream s;
  s << "close code: " << con->get_remote_close_code() << " ("
    << websocketpp::close::status::get_string(con->get_remote_close_code())
    << "), close reason: " << con->get_remote_close_reason();
  _error_reason = s.str();
  f(con);
}

void connection_metadata::on_message(
    websocketpp::connection_hdl, client::message_ptr msg,
    function<void(websocketpp::frame::opcode::value, string)> f) {
  f(msg->get_opcode(), msg->get_payload());
}

websocketpp::connection_hdl connection_metadata::get_hdl() { return _hdl; }

connection_metadata::status connection_metadata::get_status() const {
  return _status;
}

ostream &operator<<(ostream &out, connection_metadata const &data) {
  out << "> URI: " << data._uri << "\n"
      << "> Status: " << static_cast<int>(data._status) << "\n"
      << "> Remote Server: "
      << (data._server.empty() ? "None Specified" : data._server) << "\n"
      << "> Error/close reason: "
      << (data._error_reason.empty() ? "N/A" : data._error_reason) << "\n";

  return out;
}

websocket_endpoint::websocket_endpoint() {
  _endpoint.clear_access_channels(websocketpp::log::alevel::all);
  _endpoint.clear_error_channels(websocketpp::log::elevel::all);

  _endpoint.init_asio();
  _endpoint.start_perpetual();

  _thread = make_unique<thread>(&client::run, &_endpoint);
}

websocket_endpoint::~websocket_endpoint() {
  _endpoint.stop_perpetual();
  close(websocketpp::close::status::going_away, "destructor");

  _thread->join();
}

websocket_endpoint &
websocket_endpoint::on_open(function<void(client::connection_ptr)> handler) {
  _open_handler = handler;
  return *this;
}

websocket_endpoint &
websocket_endpoint::on_close(function<void(client::connection_ptr)> handler) {
  _close_handler = handler;
  return *this;
}

websocket_endpoint &
websocket_endpoint::on_fail(function<void(client::connection_ptr)> handler) {
  _fail_handler = handler;
  return *this;
}

websocket_endpoint &websocket_endpoint::on_message(
    function<void(websocketpp::frame::opcode::value, string)> handler) {
  _message_handler = handler;
  return *this;
}

websocket_endpoint &websocket_endpoint::with_tls(
    websocketpp::transport::asio::tls_socket::tls_init_handler hdl) {
  _endpoint.set_tls_init_handler(hdl);
  return *this;
}

void websocket_endpoint::connect(const string &uri) {
  error_code error;

  client::connection_ptr con = _endpoint.get_connection(uri, error);

  if (error) {
    cout << "> Connect initialization error: " << error.message() << endl;
    return;
  }

  _connection = make_shared<connection_metadata>(con->get_handle(), uri);

  con->set_open_handler(bind(&connection_metadata::on_open, _connection,
                             &_endpoint, placeholders::_1, _open_handler));

  con->set_fail_handler(bind(&connection_metadata::on_fail, _connection,
                             &_endpoint, placeholders::_1, _fail_handler));

  con->set_close_handler(bind(&connection_metadata::on_close, _connection,
                              &_endpoint, placeholders::_1, _close_handler));

  con->set_message_handler(bind(&connection_metadata::on_message, _connection,
                                placeholders::_1, placeholders::_2,
                                _message_handler));

  _endpoint.connect(con);
}

void websocket_endpoint::close(websocketpp::close::status::value code,
                               string reason) {
  if (!_connection ||
      _connection->get_status() != connection_metadata::status::open)
    return;
  error_code error;
  _endpoint.close(_connection->get_hdl(), code, reason, error);
  if (error) {
    cout << "> Error initiating close: " << error.message() << endl;
  }
}

void websocket_endpoint::send(string message) {
  error_code error;

  if (_connection &&
      _connection->get_status() == connection_metadata::status::open) {
    _endpoint.send(_connection->get_hdl(), message,
                   websocketpp::frame::opcode::text, error);
  } else {
    error = make_error_code(std::errc::not_connected);
  }

  if (error) {
    cout << "> Error sending message: " << error.message() << endl;
    return;
  }
}

connection_metadata *websocket_endpoint::get_metadata() const {
  return _connection.get();
}
}
