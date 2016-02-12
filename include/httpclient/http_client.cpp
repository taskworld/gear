#include <iostream>
#include <thread>

#include "http_client.hpp"
#include "string_utils.hpp"

namespace gear {
class http_client::impl {
 public:
  impl(asio::io_context& io_context, asio::ssl::context& context,
       const gear::http_request request, completion_handler handler)
      : _socket(io_context, context),
        _resolver(io_context),
        _request(request),
        _handler(handler) {
    _socket.set_verify_mode(asio::ssl::verify_peer);
    // TODO: When use this in production, we need to verify our host ...
    // #ifdef DEBUG
    _socket.set_verify_callback(
        [](bool, asio::ssl::verify_context&) { return true; });
    // #else
    // _socket.set_verify_callback(
    // [](bool verified, asio::ssl::verify_context&) { return verified; });
    // #endif

    _resolver.async_resolve(
        _request.host(), "443",
        bind(&impl::handle_resolve, this, placeholders::_1, placeholders::_2));
  }

  http_request get_request() { return _request; }

  friend class http_client;

 private:
  static unique_ptr<impl> execute(asio::io_context& io_context,
                                  asio::ssl::context& context,
                                  const http_request& request,
                                  const completion_handler& handler) {
    return make_unique<impl>(io_context, context, request, handler);
  }

  void handle_resolve(const error_code& error,
                      asio::ip::tcp::resolver::results_type endpoints) {
    if (!error) {
      asio::async_connect(_socket.lowest_layer(), endpoints,
                          bind(&impl::handle_connect, this, placeholders::_1));
    } else {
      handle_error(error);
    }
  }

  void handle_connect(const error_code& error) {
    if (!error) {
      _socket.async_handshake(
          asio::ssl::stream_base::client,
          bind(&impl::handle_handshake, this, placeholders::_1));
    } else {
      handle_error(error);
    }
  }

  void handle_handshake(const error_code& error) {
    if (!error) {
      auto request = write_request();
      asio::async_write(
          _socket, asio::buffer(request),
          bind(&impl::handle_write, this, placeholders::_1, placeholders::_2));
    } else {
      handle_error(error);
    }
  }

  void handle_write(const error_code& error, size_t /*bytes_transferred*/) {
    if (!error) {
      asio::async_read_until(_socket, _stream_response, "\r\n",
                             bind(&impl::handle_read_code_and_message, this,
                                  placeholders::_1, placeholders::_2));
    } else {
      handle_error(error);
    }
  }

  void handle_read_code_and_message(const error_code& error,
                                    size_t /*bytes_transferred*/) {
    if (!error) {
      istream response_stream(&_stream_response);
      string http_version;
      unsigned int status_code;
      string status_message;
      response_stream >> http_version;
      response_stream >> status_code;
      getline(response_stream, status_message);
      _response.code(status_code).message(status_message);
      asio::async_read_until(_socket, _stream_response, "\r\n\r\n",
                             bind(&impl::handle_read_headers, this,
                                  placeholders::_1, placeholders::_2));
    } else {
      handle_error(error);
    }
  }

  void handle_read_headers(const error_code& error,
                           size_t /*bytes_transferred*/) {
    if (!error) {
      // Process the response headers.
      istream response_stream(&_stream_response);
      string header_string;
      while (std::getline(response_stream, header_string) &&
             header_string != "\r") {
        auto header = gear_utils::split(header_string, ':');
        auto header_key = header[0];
        auto header_value = gear_utils::trim(header[0]);
        _response.add_header(header_key, header_value);
      }
      //  check if stream_response get some part of body ,it's should write to
      //  body response
      if (_stream_response.size() > 0) _response_stream << &_stream_response;
      handle_read_body();
    } else {
      handle_error(error);
    }
  }

  void handle_read_body() {
    asio::async_read(_socket, _stream_response, asio::transfer_at_least(1),
                     bind(&impl::recursive_read_body, this, placeholders::_1,
                          placeholders::_2));
  }

  void recursive_read_body(const error_code& error,
                           size_t /*bytes_transferred*/) {
    if (!error) {
      _response_stream << &_stream_response;
      handle_read_body();
    } else if (error == asio::error::eof || error.value() == 1) {
      _response.body(_response_stream.str());
      _handler(_request, _response);
    }
  }

  void handle_error(const error_code& error) {
    _response.message(error.message());
    _response.code(error.value());
    _handler(_request, _response);
  }

  string write_request() {
    string verb;
    switch (_request.method()) {
      case http_method::GET:
        verb = "GET";
        break;
      case http_method::PUT:
        verb = "PUT";
        break;
      case http_method::POST:
        verb = "POST";
        break;
      case http_method::DELETE:
        verb = "DELETE";
        break;
      case http_method::PATCH:
        verb = "PATCH";
        break;
      default:
        break;
    }
    string body = _request.body();
    ostringstream os;
    os << verb << " " << _request.path();
    if (_request.method() == http_method::GET ||
        _request.method() == http_method::DELETE) {
      set_queries_to_stream(os);
    }
    os << " HTTP/1.1\r\n";
    if ((_request.method() == http_method::POST ||
         _request.method() == http_method::PUT ||
         _request.method() == http_method::PATCH) &&
        _request.body().size() > 0) {
      _request.add_header("Content-Length", to_string(body.length()));
      _request.add_header("Content-Type", "application/json");
    } else {
      body = "";
    }
    set_headers_to_stream(os);
    os << "\r\n";
    os << body;
    cout << os.str() << endl;
    return os.str();
  }

  void set_headers_to_stream(ostringstream& os) {
    for (const auto& header : _request.headers()) {
      os << header.first + ": " + header.second + "\r\n";
    }
  }

  void set_queries_to_stream(ostringstream& os) {
    if (_request.queries().size() > 0) {
      os << "?";
      bool isFirst = true;
      for (const auto& query : _request.queries()) {
        if (isFirst) {
          isFirst = false;
        } else {
          os << "&";
        }
        os << gear_utils::encode_url(query.first) + "=" +
                  gear_utils::encode_url(query.second);
      }
    }
  }

  asio::ssl::stream<asio::ip::tcp::socket> _socket;
  asio::ip::tcp::resolver _resolver;
  gear::http_request _request;
  gear::http_response _response;
  ostringstream _response_stream;
  asio::streambuf _stream_response;
  unique_ptr<thread> _thread;
  completion_handler _handler;
};

http_client::http_client() : _ssl_context(asio::ssl::context::sslv23) {
  _ssl_context.set_default_verify_paths();
}

http_client::http_client(const string& uri) : http_client() {
  _config.host(uri);
  set_config_to_request();
}

http_client::http_client(http_config config) : http_client() {
  _config = config;
  set_config_to_request();
}

http_client::~http_client() {
  if (_thread) _thread->join();
}

void http_client::run() {
  _io_context.reset();
  _io_context.run();
}

void http_client::execute(const http_request& request_execute,
                          const completion_handler& handler) {
  _request = request_execute;
  if (_request.host() == "") {
    _request.host(_config.host());
  }
  if (_config.base_path() != "") {
    _request.path(_config.base_path() + _request.path());
  }
  for (auto header : _config.base_headers()) {
    _request.add_header(header.first, header.second);
  }
  for (auto query : _config.base_queries()) {
    _request.add_query(query.first, query.second);
  }
  execute(handler);
}

void http_client::execute(const completion_handler& handler) {
  reset();
  _pimpl = impl::execute(_io_context, _ssl_context, _request, handler);
  _thread = make_unique<thread>(&http_client::run, this);
  _request = gear::http_request();
  set_config_to_request();
}

void http_client::reset() {
  if (_thread) {
    _thread->join();
  }
}

void http_client::set_config_to_request() {
  _request.host(_config.host())
      .path(_config.base_path())
      .headers(_config.base_headers())
      .queries(_config.base_queries());
}

void http_client::http_get(const completion_handler& handler) {
  _request.method(http_method::GET);
  execute(handler);
}

void http_client::http_put(const completion_handler& handler) {
  _request.method(http_method::PUT);
  execute(handler);
}

void http_client::http_post(const completion_handler& handler) {
  _request.method(http_method::POST);
  execute(handler);
}

void http_client::http_delete(const completion_handler& handler) {
  _request.method(http_method::DELETE);
  execute(handler);
}

void http_client::http_patch(const completion_handler& handler) {
  _request.method(http_method::PATCH);
  execute(handler);
}

// http_request forward
string http_client::host() const { return _request.host(); }

http_client& http_client::host(const string& host) {
  _config.host(host);
  _request.host(host);
  return *this;
}

string http_client::path() const { return _request.path(); }

http_client& http_client::path(const string& path) {
  if (_config.base_path() == "")
    _request.path(path);
  else
    _request.path(_config.base_path() + path);
  return *this;
}

http_method http_client::method() const { return _request.method(); }

http_client& http_client::method(const http_method& method) {
  _request.method(method);

  return *this;
}

unordered_map<string, string> http_client::headers() const {
  return _request.headers();
}

http_client& http_client::headers(
    const unordered_map<string, string>& headers) {
  _request.headers(headers);
  return *this;
}

http_client& http_client::add_header(const string& key, const string& value) {
  _request.add_header(key, value);
  return *this;
}

string http_client::body() const { return _request.body(); }

http_client& http_client::body(const string& body) {
  _request.body(body);
  return *this;
}

vector<pair<string, string>> http_client::queries() const {
  return _request.queries();
}
http_client& http_client::queries(const vector<pair<string, string>>& queries) {
  _request.queries(queries);
  return *this;
}

http_client& http_client::add_query(const pair<string, string>& query) {
  _request.add_query(query);
  return *this;
}

http_client& http_client::add_query(const string& key, const string& value) {
  _request.add_query(key, value);
  return *this;
}

}  // namespace tw
