#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

namespace gear {
enum class HttpMethod { GET, POST, PUT, DELETE, PATCH };
class HttpRequest final {
 public:
  string host() const;
  HttpRequest& host(const string& host);

  string path() const;
  HttpRequest& path(const string& path);

  HttpMethod method() const;
  HttpRequest& method(const HttpMethod& method);

  unordered_map<string, string> headers() const;
  HttpRequest& headers(unordered_map<string, string> headers);
  HttpRequest& addHeader(const string& key, const string& value);

  string body() const;
  HttpRequest& body(const string& parameters);

  vector<pair<string, string>> queries() const;
  HttpRequest& queries(vector<pair<string, string>> queries);
  HttpRequest& addQuery(const pair<string, string>& query);
  HttpRequest& addQuery(const string& key, const string& value);

 private:
  string _host;
  string _path;
  HttpMethod _method;
  vector<pair<string, string>> _queries;
  unordered_map<string, string> _headers;
  string _body;
};
}