#include "http_response.hpp"

namespace gear {

string http_response::message() const { return _message; }

http_response& http_response::message(const string& message) {
  _message = message;
  return *this;
}

int http_response::code() const { return _code; }

http_response& http_response::code(const int code) {
  _code = code;
  return *this;
}

unordered_map<string, string> http_response::headers() const {
  return _headers;
}

http_response& http_response::headers(
    const unordered_map<string, string>& headers) {
  _headers = headers;
  return *this;
}

http_response& http_response::add_header(const string& key,
                                         const string& value) {
  _headers[key] = value;
  return *this;
}

string http_response::body() const { return _body; }

http_response& http_response::body(const string& body) {
  _body = body;
  return *this;
}
}