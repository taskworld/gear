#include <iostream>
#include <thread>

#include "HttpClient.hpp"
#include "StringUtils.hpp"

namespace gear {
class HttpClient::impl {
 public:
  impl(asio::ioContext& ioContext, asio::ssl::context& context,
       const HttpRequest request, completion_handler handler)
      : _socket(ioContext, context),
        _resolver(ioContext),
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
        bind(&impl::handleResolve, this, placeholders::_1, placeholders::_2));
  }

  HttpRequest getRequest() { return _request; }

  friend class HttpClient;

 private:
  static unique_ptr<impl> execute(asio::ioContext& ioContext,
                                  asio::ssl::context& context,
                                  const HttpRequest& request,
                                  const completion_handler& handler) {
    return make_unique<impl>(ioContext, context, request, handler);
  }

  void handleResolve(const error_code& error,
                      asio::ip::tcp::resolver::results_type endpoints) {
    if (!error) {
      asio::async_connect(_socket.lowest_layer(), endpoints,
                          bind(&impl::handleConnect, this, placeholders::_1));
    } else {
      handleError(error);
    }
  }

  void handleConnect(const error_code& error) {
    if (!error) {
      _socket.async_handshake(
          asio::ssl::stream_base::client,
          bind(&impl::handleHandshake, this, placeholders::_1));
    } else {
      handleError(error);
    }
  }

  void handleHandshake(const error_code& error) {
    if (!error) {
      auto request = writeRequest();
      asio::async_write(
          _socket, asio::buffer(request),
          bind(&impl::handleWrite, this, placeholders::_1, placeholders::_2));
    } else {
      handleError(error);
    }
  }

  void handleWrite(const error_code& error, size_t /*bytes_transferred*/) {
    if (!error) {
      asio::async_read_until(_socket, _streamResponse, "\r\n",
                             bind(&impl::handleReadCodeAndMessage, this,
                                  placeholders::_1, placeholders::_2));
    } else {
      handleError(error);
    }
  }

  void handleReadCodeAndMessage(const error_code& error,
                                    size_t /*bytes_transferred*/) {
    if (!error) {
      istream responseStream(&_streamResponse);
      string httpVersion;
      unsigned int statusCode;
      string statusMessage;
      responseStream >> httpVersion;
      responseStream >> statusCode;
      getline(responseStream, statusMessage);
      _response.code(statusCode).message(statusMessage);
      asio::async_read_until(_socket, _streamResponse, "\r\n\r\n",
                             bind(&impl::handleReadHeaders, this,
                                  placeholders::_1, placeholders::_2));
    } else {
      handleError(error);
    }
  }

  void handleReadHeaders(const error_code& error,
                           size_t /*bytes_transferred*/) {
    if (!error) {
      // Process the response headers.
      istream responseStream(&_streamResponse);
      string headerString;
      while (std::getline(responseStream, headerString) &&
             headerString != "\r") {
        auto header = gear_utils::split(headerString, ':');
        auto headerKey = header[0];
        auto headerValue = gear_utils::trim(header[0]);
        _response.addHeader(headerKey, headerValue);
      }
      //  check if stream_response get some part of body ,it's should write to
      //  body response
      if (_streamResponse.size() > 0) _responseStream << &_streamResponse;
      handleReadBody();
    } else {
      handleError(error);
    }
  }

  void handleReadBody() {
    asio::async_read(_socket, _streamResponse, asio::transfer_at_least(1),
                     bind(&impl::recursiveReadBody, this, placeholders::_1,
                          placeholders::_2));
  }

  void recursiveReadBody(const error_code& error,
                           size_t /*bytes_transferred*/) {
    if (!error) {
      _responseStream << &_streamResponse;
      handleReadBody();
    } else if (error == asio::error::eof || error.value() == 1) {
      _response.body(_responseStream.str());
      _handler(_request, _response);
    }
  }

  void handleError(const error_code& error) {
    _response.message(error.message());
    _response.code(error.value());
    _handler(_request, _response);
  }

  string writeRequest() {
    string verb;
    switch (_request.method()) {
      case HttpMethod::GET:
        verb = "GET";
        break;
      case HttpMethod::PUT:
        verb = "PUT";
        break;
      case HttpMethod::POST:
        verb = "POST";
        break;
      case HttpMethod::DELETE:
        verb = "DELETE";
        break;
      case HttpMethod::PATCH:
        verb = "PATCH";
        break;
      default:
        break;
    }
    string body = _request.body();
    ostringstream os;
    os << verb << " " << _request.path();
    if (_request.method() == HttpMethod::GET ||
        _request.method() == HttpMethod::DELETE) {
      setQueriesToStream(os);
    }
    os << " HTTP/1.1\r\n";
    if ((_request.method() == HttpMethod::POST ||
         _request.method() == HttpMethod::PUT ||
         _request.method() == HttpMethod::PATCH) &&
        _request.body().size() > 0) {
      _request.addHeader("Content-Length", to_string(body.length()));
      _request.addHeader("Content-Type", "application/json");
    } else {
      body = "";
    }
    setHeadersToStream(os);
    os << "\r\n";
    os << body;
    return os.str();
  }

  void setHeadersToStream(ostringstream& os) {
    for (const auto& header : _request.headers()) {
      os << header.first + ": " + header.second + "\r\n";
    }
  }

  void setQueriesToStream(ostringstream& os) {
    if (_request.queries().size() > 0) {
      os << "?";
      bool isFirst = true;
      for (const auto& query : _request.queries()) {
        if (isFirst) {
          isFirst = false;
        } else {
          os << "&";
        }
        os << gear_utils::encodeUrl(query.first) + "=" +
                  gear_utils::encodeUrl(query.second);
      }
    }
  }

  asio::ssl::stream<asio::ip::tcp::socket> _socket;
  asio::ip::tcp::resolver _resolver;
  gear::HttpRequest _request;
  gear::HttpResponse _response;
  ostringstream _responseStream;
  asio::streambuf _streamResponse;
  unique_ptr<thread> _thread;
  completion_handler _handler;
};

HttpClient::HttpClient() : _sslContext(asio::ssl::context::sslv23) {
  _sslContext.set_default_verify_paths();
}

HttpClient::HttpClient(const string& uri) : HttpClient() {
  _config.host(uri);
  setConfigToRequest();
}

HttpClient::HttpClient(HttpConfig config) : HttpClient() {
  _config = config;
  setConfigToRequest();
}

HttpClient::~HttpClient() {
  if (_thread) _thread->join();
}

void HttpClient::run() {
  _ioContext.reset();
  _ioContext.run();
}

void HttpClient::execute(const HttpRequest& requestExecute,
                          const completion_handler& handler) {
  _request = requestExecute;
  if (_request.host().empty()) {
    _request.host(_config.host());
  }
  if (!_config.basePath().empty()) {
    _request.path(_config.basePath() + _request.path());
  }

  for (auto header : _config.baseHeaders()) {
    _request.addHeader(header.first, header.second);
  }
  for (auto query : _config.baseQueries()) {
    _request.addQuery(query.first, query.second);
  }
  execute(handler);
}

void HttpClient::execute(const completion_handler& handler) {
  reset();
  _pimpl = impl::execute(_ioContext, _sslContext, _request, handler);
  _thread = make_unique<thread>(&HttpClient::run, this);
  _request = gear::HttpRequest();
  setConfigToRequest();
}

void HttpClient::reset() {
  if (_thread) {
    _thread->join();
  }
}

void HttpClient::setConfigToRequest() {
  _request.host(_config.host())
      .path(_config.basePath())
      .headers(_config.baseHeaders())
      .queries(_config.baseQueries());
}

void HttpClient::httpGet(const completion_handler& handler) {
  _request.method(HttpMethod::GET);
  execute(handler);
}

void HttpClient::httpPut(const completion_handler& handler) {
  _request.method(HttpMethod::PUT);
  execute(handler);
}

void HttpClient::httpPost(const completion_handler& handler) {
  _request.method(HttpMethod::POST);
  execute(handler);
}

void HttpClient::httpDelete(const completion_handler& handler) {
  _request.method(HttpMethod::DELETE);
  execute(handler);
}

void HttpClient::httpPatch(const completion_handler& handler) {
  _request.method(HttpMethod::PATCH);
  execute(handler);
}

// HttpRequest forward
string HttpClient::host() const { return _request.host(); }

HttpClient& HttpClient::host(const string& host) {
  _config.host(host);
  _request.host(host);
  return *this;
}

string HttpClient::path() const { return _request.path(); }

HttpClient& HttpClient::path(const string& path) {
  if (_config.basePath().empty())
    _request.path(path);
  else
    _request.path(_config.basePath() + path);
  return *this;
}

HttpMethod HttpClient::method() const { return _request.method(); }

HttpClient& HttpClient::method(const HttpMethod& method) {
  _request.method(method);

  return *this;
}

unordered_map<string, string> HttpClient::headers() const {
  return _request.headers();
}

HttpClient& HttpClient::headers(
    const unordered_map<string, string>& headers) {
  _request.headers(headers);
  return *this;
}

HttpClient& HttpClient::addHeader(const string& key, const string& value) {
  _request.addHeader(key, value);
  return *this;
}

string HttpClient::body() const { return _request.body(); }

HttpClient& HttpClient::body(const string& body) {
  _request.body(body);
  return *this;
}

vector<pair<string, string>> HttpClient::queries() const {
  return _request.queries();
}
HttpClient& HttpClient::queries(const vector<pair<string, string>>& queries) {
  _request.queries(queries);
  return *this;
}

HttpClient& HttpClient::addQuery(const pair<string, string>& query) {
  _request.addQuery(query);
  return *this;
}

HttpClient& HttpClient::addQuery(const string& key, const string& value) {
  _request.addQuery(key, value);
  return *this;
}

}  // namespace tw
