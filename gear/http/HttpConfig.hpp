#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

namespace gear {
class HttpConfig final {
 public:
  HttpConfig() = default;
  HttpConfig(const std::string& host);
  HttpConfig(const std::string& host, const std::string& basePath);
  HttpConfig(const std::string& host, const std::string& basePath,const std::pair<std::string, std::string>& baseHeader);

  std::string host() const;
  HttpConfig& host(const std::string& host);

  std::string basePath() const;
  HttpConfig& basePath(const std::string& basePath);

  std::unordered_map<std::string, std::string>baseHeaders() const;
  HttpConfig& baseHeaders(const std::unordered_map<std::string, std::string>& baseHeaders);
  HttpConfig& addBaseHeader(const std::pair<std::string, std::string>& baseHeader);
  HttpConfig& addBaseHeader(const std::string& key, const std::string& value);

  std::vector<std::pair<std::string, std::string>> baseQueries() const;
  HttpConfig& baseQueries(const std::vector<std::pair<std::string, std::string>>& baseQueries);
  HttpConfig& addBaseQuery(const std::pair<std::string, std::string>& base_query);
  HttpConfig& addBaseQuery(const std::string& key, const std::string& value);

 private:
  std::string _host;
  std::string _basePath;
  std::vector<std::pair<std::string, std::string>> _baseQueries;
  std::unordered_map<std::string, std::string> _baseHeaders = {{"Accept", "*/*"},
                                           {"Connection", "close"}};
};
}
