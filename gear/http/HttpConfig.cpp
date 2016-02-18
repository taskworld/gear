#include "HttpConfig.hpp"

namespace gear {

HttpConfig::HttpConfig(){

}

HttpConfig::HttpConfig(const string& initHost, const string& initBasePath,const pair<string, string>& initBaseHeader){
  host(initHost).basePath(initBasePath).addBaseHeader(initBaseHeader);
}

string HttpConfig::host() const { return _host; }

HttpConfig& HttpConfig::host(const string& host) {
  _host = host;
  if (!_host.empty()) {
    addBaseHeader("Host", _host);
  }
  return *this;
}

string HttpConfig::basePath() const { return _basePath; }

HttpConfig& HttpConfig::basePath(const string& basePath) {
  _basePath = basePath;
  return *this;
}

unordered_map<string, string> HttpConfig::baseHeaders() const {
  return _baseHeaders;
}

HttpConfig& HttpConfig::addBaseHeader(const pair<string, string>& baseHeader) {
  _baseHeaders[baseHeader.first] = baseHeader.second;
  return *this;
}

HttpConfig& HttpConfig::addBaseHeader(const string& key,
                                          const string& value) {
  _baseHeaders[key] = value;
  return *this;
}

vector<pair<string, string>> HttpConfig::baseQueries() const {
  return _baseQueries;
}
HttpConfig& HttpConfig::baseQueries(
    const vector<pair<string, string>>& queries) {
  _baseQueries = queries;
  return *this;
}

HttpConfig& HttpConfig::addBaseQuery(const pair<string, string>& query) {
  _baseQueries.push_back(query);
  return *this;
}

HttpConfig& HttpConfig::addBaseQuery(const string& key,
                                         const string& value) {
  _baseQueries.push_back(make_pair(key, value));
  return *this;
}
}