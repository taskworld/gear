#include <iostream>
#include <vector>

using namespace std;

namespace gear {
enum class http_method { GET, POST, PUT, DELETE, PATCH };
class http_request final {
 public:
  string host() const;
  http_request& host(const string& host);

  string path() const;
  http_request& path(const string& path);

  http_method method() const;
  http_request& method(const http_method& method);

  vector<pair<string, string>> headers() const;
  http_request& headers(const vector<pair<string, string>>& headers);
  http_request& add_header(const pair<string, string>& header);
  http_request& add_header(const string& key, const string& value);

  string body() const;
  http_request& body(const string& parameters);

  vector<pair<string, string>> queries() const;
  http_request& queries(const vector<pair<string, string>>& queries);
  http_request& add_query(const pair<string, string>& query);
  http_request& add_query(const string& key, const string& value);

 private:
  string _host;
  string _path;
  http_method _method;
  vector<pair<string, string>> _queries;
  vector<pair<string, string>> _headers = {{"Accept", "*/*"},
                                           {"Connection", "close"}};
  string _body;
};
}