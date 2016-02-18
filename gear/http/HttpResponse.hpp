#include <iostream>
#include <unordered_map>
#include <vector>

namespace gear {
class HttpResponse final {
 public:
  int code() const;
  HttpResponse& code(const int code);
  // TODO :overload operator
  std::string message() const;
  HttpResponse& message(const std::string& message);

  std::unordered_map<std::string, std::string> headers() const;
  HttpResponse& headers(const std::unordered_map<std::string, std::string>& headers);
  HttpResponse& addHeader(const std::string& key, const std::string& value);

  std::string body() const;
  HttpResponse& body(const std::string& body);

 private:
  int _code{-1};
  std::string _message;
  std::unordered_map<std::string, std::string> _headers;
  std::string _body;
};
}