#include <asio.hpp>
#include <asio/ssl.hpp>
#include <experimental/optional>
#include <thread>

#include "http_request.hpp"
#include "http_response.hpp"

using namespace std;
using namespace std::experimental;

using completion_handler =
    function<void(const gear::http_request&, const gear::http_response&)>;

namespace gear {

class http_client final {
 public:
  http_client();
  http_client(const string& uri);
  ~http_client();

  string host() const;
  http_client& host(const string& host);

  string path() const;
  http_client& path(const string& path);

  http_method method() const;
  http_client& method(const http_method& method);

  vector<pair<string, string>> headers() const;
  http_client& headers(const vector<pair<string, string>>& headers);
  http_client& add_header(const pair<string, string>& header);
  http_client& add_header(const string& key, const string& value);

  string body() const;
  http_client& body(const string& body);

  vector<pair<string, string>> queries() const;
  http_client& queries(const vector<pair<string, string>>& queries);
  http_client& add_query(const pair<string, string>& query);
  http_client& add_query(const string& key, const string& value);

  void http_get(const completion_handler& handler);
  void http_put(const completion_handler& handler);
  void http_post(const completion_handler& handler);
  void http_delete(const completion_handler& handler);
  void http_patch(const completion_handler& handler);
  void execute(const http_request& request_execute,
               const completion_handler& handler);
  void execute(const completion_handler& handler);

 private:
  void reset();
  void run();

  asio::io_context _io_context;
  asio::ssl::context _ssl_context;
  http_request _request;
  class impl;
  unique_ptr<impl> _pimpl;

  unique_ptr<thread> _thread;
};
}  // namespace tw