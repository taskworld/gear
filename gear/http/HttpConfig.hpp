#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

namespace gear {
class HttpConfig final {
 public:
  HttpConfig();
  HttpConfig(const string& host, const string& basePath,const pair<string, string>& baseHeader);

  string host() const;
  HttpConfig& host(const string& host);

  string basePath() const;
  HttpConfig& basePath(const string& basePath);

  unordered_map<string, string>baseHeaders() const;
  HttpConfig& baseHeaders(const unordered_map<string, string>& baseHeaders);
  HttpConfig& addBaseHeader(const pair<string, string>& baseHeader);
  HttpConfig& addBaseHeader(const string& key, const string& value);

  vector<pair<string, string>> baseQueries() const;
  HttpConfig& baseQueries(const vector<pair<string, string>>& baseQueries);
  HttpConfig& addBaseQuery(const pair<string, string>& base_query);
  HttpConfig& addBaseQuery(const string& key, const string& value);

 private:
  string _host;
  string _basePath;
  vector<pair<string, string>> _baseQueries;
  unordered_map<string, string> _baseHeaders = {{"Accept", "*/*"},
                                           {"Connection", "close"}};
};
}
