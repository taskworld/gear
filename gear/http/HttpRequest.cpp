#include "HttpRequest.hpp"
#include "StringUtils.hpp"

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

HttpRequest& HttpRequest::method(const std::string& method) {
  if(gear_utils::stringCompareIgnoreCase(method,"get")){
   _method = gear::HttpMethod::GET;
  }
  else if(gear_utils::stringCompareIgnoreCase(method,"post")) {
    _method = gear::HttpMethod::POST;
  }
  else if(gear_utils::stringCompareIgnoreCase(method,"put")) {
    _method = gear::HttpMethod::PUT;
  }
  else if(gear_utils::stringCompareIgnoreCase(method,"patch")) {
    _method = gear::HttpMethod::PATCH;
  }
  else if(gear_utils::stringCompareIgnoreCase(method,"delete")) {
    _method = gear::HttpMethod::DELETE;
  }
  return *this;
}

std::unordered_map<std::string, std::string> HttpRequest::headers() const { return _headers; }

HttpRequest& HttpRequest::headers(std::unordered_map<std::string, std::string> headers) {
  for (const auto& header : headers) {
    _headers[header.first] = header.second;
  }
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
  for (const auto& query : queries) {
    _queries.push_back(std::make_pair(query.first, query.second));
  }
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