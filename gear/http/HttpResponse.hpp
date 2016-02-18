#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

namespace gear {
class HttpResponse final {
 public:
  int code() const;
  HttpResponse& code(const int code);
  // TODO :overload operator
  string message() const;
  HttpResponse& message(const string& message);

  unordered_map<string, string> headers() const;
  HttpResponse& headers(const unordered_map<string, string>& headers);
  HttpResponse& addHeader(const string& key, const string& value);

  string body() const;
  HttpResponse& body(const string& body);

 private:
  int _code{-1};
  string _message;
  unordered_map<string, string> _headers;
  string _body;
};
}