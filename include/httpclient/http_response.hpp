#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

namespace gear {
class http_response final {
 public:
  int code() const;
  http_response& code(const int code);
  // TODO :overload operator
  string message() const;
  http_response& message(const string& message);

  unordered_map<string, string> headers() const;
  http_response& headers(const unordered_map<string, string>& headers);
  http_response& add_header(const string& key, const string& value);

  string body() const;
  http_response& body(const string& body);

 private:
  int _code{-1};
  string _message;
  unordered_map<string, string> _headers;
  string _body;
};
}