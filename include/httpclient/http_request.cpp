#include "http_request.hpp"

namespace gear {

string http_request::host() const { return _host; }

http_request& http_request::host(const string& host) {
  _host = host;
  if (!_host.empty()) {
    add_header("Host", _host);
  }
  return *this;
}

string http_request::path() const { return _path; }

http_request& http_request::path(const string& path) {
  _path = path;
  return *this;
}

http_method http_request::method() const { return _method; }

http_request& http_request::method(const http_method& method) {
  _method = method;
  return *this;
}

unordered_map<string, string> http_request::headers() const { return _headers; }

http_request& http_request::headers(unordered_map<string, string> headers) {
  _headers = headers;
  return *this;
}

http_request& http_request::add_header(const string& key, const string& value) {
  _headers[key] =  value;
  return *this;
}

string http_request::body() const { return _body; }

http_request& http_request::body(const string& parameters) {
  _body = parameters;
  return *this;
}

vector<pair<string, string>> http_request::queries() const { return _queries; }

http_request& http_request::queries(vector<pair<string, string>> queries) {
  _queries = queries;
  return *this;
}

http_request& http_request::add_query(const pair<string, string>& query) {
  _queries.push_back(query);
  return *this;
}

http_request& http_request::add_query(const string& key, const string& value) {
  _queries.push_back(make_pair(key, value));
  return *this;
}
}