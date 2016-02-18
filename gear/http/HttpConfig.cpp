#include "HttpConfig.hpp"

namespace gear {

HttpConfig::HttpConfig(const std::string& initHost, const std::string& initBasePath,const std::pair<std::string, std::string>& initBaseHeader){
  host(initHost).basePath(initBasePath).addBaseHeader(initBaseHeader);
}

std::string HttpConfig::host() const { return _host; }

HttpConfig& HttpConfig::host(const std::string& host) {
  _host = host;
  if (!_host.empty()) {
    addBaseHeader("Host", _host);
  }
  return *this;
}

std::string HttpConfig::basePath() const { return _basePath; }

HttpConfig& HttpConfig::basePath(const std::string& basePath) {
  _basePath = basePath;
  return *this;
}

std::unordered_map<std::string, std::string> HttpConfig::baseHeaders() const {
  return _baseHeaders;
}

HttpConfig& HttpConfig::addBaseHeader(const std::pair<std::string, std::string>& baseHeader) {
  _baseHeaders[baseHeader.first] = baseHeader.second;
  return *this;
}

HttpConfig& HttpConfig::addBaseHeader(const std::string& key,
                                          const std::string& value) {
  _baseHeaders[key] = value;
  return *this;
}

std::vector<std::pair<std::string, std::string>> HttpConfig::baseQueries() const {
  return _baseQueries;
}
HttpConfig& HttpConfig::baseQueries(
    const std::vector<std::pair<std::string, std::string>>& queries) {
  _baseQueries = queries;
  return *this;
}

HttpConfig& HttpConfig::addBaseQuery(const std::pair<std::string, std::string>& query) {
  _baseQueries.push_back(query);
  return *this;
}

HttpConfig& HttpConfig::addBaseQuery(const std::string& key,
                                         const std::string& value) {
  _baseQueries.push_back(std::make_pair(key, value));
  return *this;
}
}