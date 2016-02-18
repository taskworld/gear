#include "HttpRequest.hpp"

namespace gear {

string HttpRequest::host() const { return _host; }

HttpRequest& HttpRequest::host(const string& host) {
  _host = host;
  if (!_host.empty()) {
    addHeader("Host", _host);
  }
  return *this;
}

string HttpRequest::path() const { return _path; }

HttpRequest& HttpRequest::path(const string& path) {
  _path = path;
  return *this;
}

HttpMethod HttpRequest::method() const { return _method; }

HttpRequest& HttpRequest::method(const HttpMethod& method) {
  _method = method;
  return *this;
}

unordered_map<string, string> HttpRequest::headers() const { return _headers; }

HttpRequest& HttpRequest::headers(unordered_map<string, string> headers) {
  _headers = headers;
  return *this;
}

HttpRequest& HttpRequest::addHeader(const string& key, const string& value) {
  _headers[key] =  value;
  return *this;
}

string HttpRequest::body() const { return _body; }

HttpRequest& HttpRequest::body(const string& parameters) {
  _body = parameters;
  return *this;
}

vector<pair<string, string>> HttpRequest::queries() const { return _queries; }

HttpRequest& HttpRequest::queries(vector<pair<string, string>> queries) {
  _queries = queries;
  return *this;
}

HttpRequest& HttpRequest::addQuery(const pair<string, string>& query) {
  _queries.push_back(query);
  return *this;
}

HttpRequest& HttpRequest::addQuery(const string& key, const string& value) {
  _queries.push_back(make_pair(key, value));
  return *this;
}
}