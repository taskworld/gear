#include "http_config.hpp"

namespace gear {

http_config::http_config(){

}

http_config::http_config(const string& host, const string& base_path,const pair<string, string>& base_header){
  host(host);
  base_path(base_path);
  add_base_header(base_header);
}

string http_config::host() const { return _host; }

http_config& http_config::host(const string& host) {
  _host = host;
  if (!_host.empty()) {
    add_base_header("Host", _host);
  }
  return *this;
}

string http_config::base_path() const { return _base_path; }

http_config& http_config::base_path(const string& base_path) {
  _base_path = base_path;
  return *this;
}

unordered_map<string, string> http_config::base_headers() const {
  return _base_headers;
}

http_config& http_config::add_base_header(const pair<string, string>& base_header) {
  _base_headers[base_header.first] = base_header.second;
  return *this;
}

http_config& http_config::add_base_header(const string& key,
                                          const string& value) {
  _base_headers[key] = value;
  return *this;
}

vector<pair<string, string>> http_config::base_queries() const {
  return _base_queries;
}
http_config& http_config::base_queries(
    const vector<pair<string, string>>& queries) {
  _base_queries = queries;
  return *this;
}

http_config& http_config::add_base_query(const pair<string, string>& query) {
  _base_queries.push_back(query);
  return *this;
}

http_config& http_config::add_base_query(const string& key,
                                         const string& value) {
  _base_queries.push_back(make_pair(key, value));
  return *this;
}
}