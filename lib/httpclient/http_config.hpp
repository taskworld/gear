#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

namespace gear {
class http_config final {
 public:
  string host() const;
  http_config& host(const string& host);

  string base_path() const;
  http_config& base_path(const string& base_path);

  unordered_map<string, string>base_headers() const;
  http_config& base_headers(const unordered_map<string, string>& base_headers);
  http_config& add_base_header(const string& key, const string& value);

  vector<pair<string, string>> base_queries() const;
  http_config& base_queries(const vector<pair<string, string>>& base_queries);
  http_config& add_base_query(const pair<string, string>& base_query);
  http_config& add_base_query(const string& key, const string& value);

 private:
  string _host;
  string _base_path;
  vector<pair<string, string>> _base_queries;
  unordered_map<string, string> _base_headers = {{"Accept", "*/*"},
                                           {"Connection", "close"}};
};
}
