#include <asio.hpp>
#include <asio/ssl.hpp>
#include <experimental/optional>
#include <thread>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpConfig.hpp"

using namespace std;
using namespace std::experimental;

using completion_handler =
    function<void(const gear::HttpRequest, const gear::HttpResponse)>;

namespace gear {

class HttpClient final {
 public:
  HttpClient(const string& uri);
  HttpClient(HttpConfig config);
  ~HttpClient();

  string host() const;
  HttpClient& host(const string& host);

  string path() const;
  HttpClient& path(const string& path);

  HttpMethod method() const;
  HttpClient& method(const HttpMethod& method);

  unordered_map<string, string> headers() const;
  HttpClient& headers(const unordered_map<string, string>& headers);
  HttpClient& addHeader(const string& key, const string& value);

  string body() const;
  HttpClient& body(const string& body);

  vector<pair<string, string>> queries() const;
  HttpClient& queries(const vector<pair<string, string>>& queries);
  HttpClient& addQuery(const pair<string, string>& query);
  HttpClient& addQuery(const string& key, const string& value);

  void httpGet(const completion_handler& handler);
  void httpPut(const completion_handler& handler);
  void httpPost(const completion_handler& handler);
  void httpDelete(const completion_handler& handler);
  void httpPatch(const completion_handler& handler);
  void execute(const HttpRequest& requestExecute,
               const completion_handler& handler);
  void execute(const completion_handler& handler);

 private:
  HttpClient();
  void reset();
  void run();
  void setConfigToRequest();

  asio::io_context _ioContext;
  asio::ssl::context _sslContext;
  HttpRequest _request;
  HttpConfig _config;
  class impl;
  unique_ptr<impl> _pimpl;

  unique_ptr<thread> _thread;
};
}  // namespace tw