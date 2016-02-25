
#pragma once

#include <experimental/optional>
#include <thread>

#include "HttpConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

using completion_handler = std::function<void(const gear::HttpRequest, const gear::HttpResponse)>;

namespace asio {
class io_context;

namespace ssl {
class context;
}
}

namespace gear {

class HttpClient final {
 public:
  HttpClient(const std::string& uri);
  HttpClient(HttpConfig config);
  ~HttpClient();

  std::string host() const;
  HttpClient& host(const std::string& host);

  std::string path() const;
  HttpClient& path(const std::string& path);

  HttpMethod method() const;
  HttpClient& method(const HttpMethod& method);
  HttpClient& method(const std::string& method);

  std::unordered_map<std::string, std::string> headers() const;
  HttpClient& headers(const std::unordered_map<std::string, std::string>& headers);
  HttpClient& addHeader(const std::string& key, const std::string& value);

  std::string body() const;
  HttpClient& body(const std::string& body);

  std::vector<std::pair<std::string, std::string>> queries() const;
  HttpClient& queries(const std::vector<std::pair<std::string, std::string>>& queries);
  HttpClient& addQuery(const std::pair<std::string, std::string>& query);
  HttpClient& addQuery(const std::string& key, const std::string& value);

  void httpGet(const completion_handler& handler);
  void httpPut(const completion_handler& handler);
  void httpPost(const completion_handler& handler);
  void httpDelete(const completion_handler& handler);
  void httpPatch(const completion_handler& handler);
  void execute(const HttpRequest& requestExecute, const completion_handler& handler);
  void execute(const completion_handler& handler);

 private:
  HttpClient();
  void reset();
  void run();
  void applyConfig();

  std::unique_ptr<asio::ssl::context> _sslContext;
  std::unique_ptr<asio::io_context> _ioContext;
  HttpRequest _request;
  HttpConfig _config;
  class impl;
  std::unique_ptr<impl> _pimpl;

  std::unique_ptr<std::thread> _thread;
};
}  // namespace tw
