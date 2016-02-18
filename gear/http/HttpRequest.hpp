#include <iostream>
#include <unordered_map>
#include <vector>

namespace gear {
enum class HttpMethod { GET, POST, PUT, DELETE, PATCH };
class HttpRequest final {
 public:
  std::string host() const;
  HttpRequest& host(const std::string& host);

  std::string path() const;
  HttpRequest& path(const std::string& path);

  HttpMethod method() const;
  HttpRequest& method(const HttpMethod& method);

  std::unordered_map<std::string, std::string> headers() const;
  HttpRequest& headers(std::unordered_map<std::string, std::string> headers);
  HttpRequest& addHeader(const std::string& key, const std::string& value);

  std::string body() const;
  HttpRequest& body(const std::string& parameters);

  std::vector<std::pair<std::string, std::string>> queries() const;
  HttpRequest& queries(std::vector<std::pair<std::string, std::string>> queries);
  HttpRequest& addQuery(const std::pair<std::string, std::string>& query);
  HttpRequest& addQuery(const std::string& key, const std::string& value);

 private:
  std::string _host;
  std::string _path;
  HttpMethod _method;
  std::vector<std::pair<std::string, std::string>> _queries;
  std::unordered_map<std::string, std::string> _headers;
  std::string _body;
};
}