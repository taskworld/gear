#include "HttpResponse.hpp"

namespace gear {

std::string HttpResponse::message() const { return _message; }

HttpResponse& HttpResponse::message(const std::string& message) {
  _message = message;
  return *this;
}

int HttpResponse::code() const { return _code; }

HttpResponse& HttpResponse::code(const int code) {
  _code = code;
  return *this;
}

std::unordered_map<std::string, std::string> HttpResponse::headers() const { return _headers; }

HttpResponse& HttpResponse::headers(const std::unordered_map<std::string, std::string>& headers) {
  _headers = headers;
  return *this;
}

HttpResponse& HttpResponse::addHeader(const std::string& key, const std::string& value) {
  _headers[key] = value;
  return *this;
}

std::string HttpResponse::body() const { return _body; }

HttpResponse& HttpResponse::body(const std::string& body) {
  _body = body;
  return *this;
}
}
