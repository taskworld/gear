#include "SocketClient.hpp"

namespace gear {
ConnectionMetaData::ConnectionMetaData(websocketpp::connection_hdl hdl, string uri)
    : _hdl(hdl), _uri(uri) {}

void ConnectionMetaData::onOpened(client *c, websocketpp::connection_hdl hdl, int retry_attempt,
                                  function<void(client::connection_ptr)> onOpened,
                                  function<void(client::connection_ptr)> onRetryOpened) {
  _status = status::open;

  client::connection_ptr con = c->get_con_from_hdl(hdl);
  _server = con->get_response_header("Server");
  if (retry_attempt > 0) {
    onRetryOpened(con);
  } else {
    onOpened(con);
  }
}

void ConnectionMetaData::onFailed(client *c, websocketpp::connection_hdl hdl,
                                  function<void(client::connection_ptr)> onFailed) {
  _status = status::fail;

  client::connection_ptr con = c->get_con_from_hdl(hdl);
  _server = con->get_response_header("Server");
  _errorReason = con->get_ec().message();
  onFailed(con);
}

void ConnectionMetaData::onClosed(client *c, websocketpp::connection_hdl hdl,
                                  WebSocketEndpoint *endpoint,
                                  function<void(client::connection_ptr)> onClosed) {
  _status = status::close;
  client::connection_ptr con = c->get_con_from_hdl(hdl);
  stringstream s;
  s << "close code: " << con->get_remote_close_code() << " ("
    << websocketpp::close::status::get_string(con->get_remote_close_code())
    << "), close reason: " << con->get_remote_close_reason();
  _errorReason = s.str();

  // If socket is not closed properly, we should retry to connect to server
  // again. This will not notify back to onClosed() since the socket in not
  // intentionally closed.
  if (con->get_remote_close_code() != websocketpp::close::status::normal &&
      !(con->get_remote_close_code() == websocketpp::close::status::going_away &&
        con->get_remote_close_reason() == "destructor")) {
    endpoint->retry(_uri);
  } else {
    onClosed(con);
  }
}

void ConnectionMetaData::onMessageReceived(
    websocketpp::connection_hdl, client::message_ptr msg,
    function<void(websocketpp::frame::opcode::value, string)> onMessageReceived) {
  onMessageReceived(msg->get_opcode(), msg->get_payload());
}

websocketpp::connection_hdl ConnectionMetaData::getHandler() { return _hdl; }

ConnectionMetaData::status ConnectionMetaData::getStatus() const { return _status; }

ostream &operator<<(ostream &out, ConnectionMetaData const &data) {
  out << "> URI: " << data._uri << "\n"
      << "> Status: " << static_cast<int>(data._status) << "\n"
      << "> Remote Server: " << (data._server.empty() ? "None Specified" : data._server) << "\n"
      << "> Error/close reason: " << (data._errorReason.empty() ? "N/A" : data._errorReason)
      << "\n";

  return out;
}

WebSocketEndpoint::WebSocketEndpoint() {
  _endpoint.clear_access_channels(websocketpp::log::alevel::all);
  _endpoint.clear_error_channels(websocketpp::log::elevel::all);

  _retryAttemptyCount = 0;

  _retryStartedHandler = [](int) {};
  _retryOpenedHandler = [](client::connection_ptr) {};

  _endpoint.init_asio();
  _endpoint.start_perpetual();

  _thread = make_unique<thread>(&client::run, &_endpoint);
}

WebSocketEndpoint::~WebSocketEndpoint() {
  _endpoint.stop_perpetual();
  close(websocketpp::close::status::going_away, "destructor");

  _thread->join();
}

WebSocketEndpoint &WebSocketEndpoint::onOpened(function<void(client::connection_ptr)> handler) {
  _openHandler = handler;
  return *this;
}

WebSocketEndpoint &WebSocketEndpoint::onRetryStarted(function<void(int)> handler) {
  _retryStartedHandler = handler;
  return *this;
}

WebSocketEndpoint &WebSocketEndpoint::onRetryOpened(
    function<void(client::connection_ptr)> handler) {
  _retryOpenedHandler = handler;
  return *this;
}

WebSocketEndpoint &WebSocketEndpoint::onClosed(function<void(client::connection_ptr)> handler) {
  _closeHandler = handler;
  return *this;
}

WebSocketEndpoint &WebSocketEndpoint::onFailed(function<void(client::connection_ptr)> handler) {
  _failHandler = handler;
  return *this;
}

WebSocketEndpoint &WebSocketEndpoint::onMessageReceived(
    function<void(websocketpp::frame::opcode::value, string)> handler) {
  _messageHandler = handler;
  return *this;
}

WebSocketEndpoint &WebSocketEndpoint::withTls(
    websocketpp::transport::asio::tls_socket::tls_init_handler hdl) {
  _endpoint.set_tls_init_handler(hdl);
  return *this;
}

void WebSocketEndpoint::connect(const string &uri) {
  error_code error;

  client::connection_ptr con = _endpoint.get_connection(uri, error);

  if (error) {
    cout << "> Connect initialization error: " << error.message() << endl;
    return;
  }

  _connection = make_shared<ConnectionMetaData>(con->get_handle(), uri);

  con->set_open_handler(bind(&ConnectionMetaData::onOpened, _connection, &_endpoint,
                             placeholders::_1, _retryAttemptyCount, _openHandler,
                             _retryOpenedHandler));

  con->set_fail_handler(
      bind(&ConnectionMetaData::onFailed, _connection, &_endpoint, placeholders::_1, _failHandler));

  con->set_close_handler(bind(&ConnectionMetaData::onClosed, _connection, &_endpoint,
                              placeholders::_1, this, _closeHandler));

  con->set_message_handler(bind(&ConnectionMetaData::onMessageReceived, _connection,
                                placeholders::_1, placeholders::_2, _messageHandler));

  _endpoint.connect(con);
}

void WebSocketEndpoint::retry(const string &uri) {
  _retryAttemptyCount++;
  this_thread::sleep_for(chrono::seconds(3 * _retryAttemptyCount));
  _retryStartedHandler(_retryAttemptyCount);
  connect(uri);
}

void WebSocketEndpoint::close(websocketpp::close::status::value code, string reason) {
  if (!_connection || _connection->getStatus() != ConnectionMetaData::status::open) return;
  error_code error;
  _endpoint.close(_connection->getHandler(), code, reason, error);
  if (error) {
    cout << "> Error initiating close: " << error.message() << endl;
  }
}

void WebSocketEndpoint::send(string message) {
  error_code error;

  if (_connection && _connection->getStatus() == ConnectionMetaData::status::open) {
    _endpoint.send(_connection->getHandler(), message, websocketpp::frame::opcode::text, error);
  } else {
    error = make_error_code(std::errc::not_connected);
  }

  if (error) {
    cout << "> Error sending message: " << error.message() << endl;
    return;
  }
}

ConnectionMetaData *WebSocketEndpoint::getMetaData() const { return _connection.get(); }
}
