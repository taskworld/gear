#include "HttpRequest.hpp"

namespace gear {

std::string HttpRequest::host() const { return _host; }

HttpRequest& HttpRequest::host(const std::string& host) {
  _host = host;
  if (!_host.empty()) {
    addHeader("Host", _host);
  }
  return *this;
}

std::string HttpRequest::path() const { return _path; }

HttpRequest& HttpRequest::path(const std::string& path) {
  _path = path;
  return *this;
}

HttpMethod HttpRequest::method() const { return _method; }

HttpRequest& HttpRequest::method(const HttpMethod& method) {
  _method = method;
  return *this;
}

std::unordered_map<std::string, std::string> HttpRequest::headers() const { return _headers; }

HttpRequest& HttpRequest::headers(std::unordered_map<std::string, std::string> headers) {
  _headers = headers;
  return *this;
}

HttpRequest& HttpRequest::addHeader(const std::string& key, const std::string& value) {
  _headers[key] =  value;
  return *this;
}

std::string HttpRequest::body() const { return _body; }

HttpRequest& HttpRequest::body(const std::string& parameters) {
  _body = parameters;
  return *this;
}

std::vector<std::pair<std::string, std::string>> HttpRequest::queries() const { return _queries; }

HttpRequest& HttpRequest::queries(std::vector<std::pair<std::string, std::string>> queries) {
  _queries = queries;
  return *this;
}

HttpRequest& HttpRequest::addQuery(const std::pair<std::string, std::string>& query) {
  _queries.push_back(query);
  return *this;
}

HttpRequest& HttpRequest::addQuery(const std::string& key, const std::string& value) {
  _queries.push_back(std::make_pair(key, value));
  return *this;
}
}