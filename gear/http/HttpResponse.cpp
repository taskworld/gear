#include "HttpResponse.hpp"

namespace gear {

string HttpResponse::message() const { return _message; }

HttpResponse& HttpResponse::message(const string& message) {
  _message = message;
  return *this;
}

int HttpResponse::code() const { return _code; }

HttpResponse& HttpResponse::code(const int code) {
  _code = code;
  return *this;
}

unordered_map<string, string> HttpResponse::headers() const {
  return _headers;
}

HttpResponse& HttpResponse::headers(
    const unordered_map<string, string>& headers) {
  _headers = headers;
  return *this;
}

HttpResponse& HttpResponse::addHeader(const string& key,
                                         const string& value) {
  _headers[key] = value;
  return *this;
}

string HttpResponse::body() const { return _body; }

HttpResponse& HttpResponse::body(const string& body) {
  _body = body;
  return *this;
}
}