#include <gtest/gtest.h>
#include <http/HttpClient.hpp>

#include "Semaphore.cpp"

TEST(asio_http, set_request_method_by_string) {
  gear::HttpRequest request;
  request.method("Get");
  EXPECT_EQ(request.method(), gear::HttpMethod::GET);
   request.method("Post");
  EXPECT_EQ(request.method(), gear::HttpMethod::POST);
   request.method("Put");
  EXPECT_EQ(request.method(), gear::HttpMethod::PUT);
   request.method("Delete");
  EXPECT_EQ(request.method(), gear::HttpMethod::DELETE);
   request.method("Patch");
  EXPECT_EQ(request.method(), gear::HttpMethod::PATCH);
}

TEST(asio_http, get_then_cancel) {
  Semaphore s;
  std::string status = "NONE";
  gear::HttpClient client("httpbin.org");

  client.path("/delay/5")
      .httpGet([&](const gear::HttpRequest, const gear::HttpResponse response) {
        status = (response.code() == 200) ? "OK" : "FAIL";
       s.notify();
      });
  client.cancel();
  s.waitFor(chrono::seconds(15));
  EXPECT_EQ(status, "NONE");
}

TEST(asio_http, get_unknown_host) {
  Semaphore s;
  std::string status;
  gear::HttpClient client("klj90u90klklkffkfjf");

  client.path("/v1/123156789")
      .httpGet([&](const gear::HttpRequest, const gear::HttpResponse response) {
        status = (response.code() == 1) ? "OK" : "FAIL";
        s.notify();
      });
  s.waitFor(chrono::seconds(15));
  EXPECT_EQ(status, "OK");
}

TEST(asio_http, config_set_value_to_request) {
  Semaphore s;
  std::string status;
  std::string payload1, payload2;

  auto config = gear::HttpConfig().host("httpbin.org").basePath("/headers");
  gear::HttpClient client(config);

  EXPECT_EQ(client.host(), config.host());
  EXPECT_EQ(client.path(), config.basePath());

  client.addHeader("header1", "value1")
      .httpGet([&](const gear::HttpRequest, const gear::HttpResponse response) {
        payload1 = response.body();
        status = (response.code() == 200) ? "OK" : "FAIL";
        s.notify();
      });

  s.waitFor(chrono::seconds(15));
  EXPECT_EQ(status, "OK");

  EXPECT_EQ(client.host(), config.host());
  EXPECT_EQ(client.path(), config.basePath());

  status = "";
  client.addHeader("header1", "value1")
      .httpGet([&](const gear::HttpRequest, const gear::HttpResponse response) {
        payload2 = response.body();
        status = (response.code() == 200) ? "OK" : "FAIL";
        s.notify();
      });

  s.waitFor(chrono::seconds(15));
  EXPECT_EQ(status, "OK");
  EXPECT_EQ(payload1, payload2);
}

TEST(asio_http, get) {
  Semaphore s;
  std::string status;

  gear::HttpClient client("httpbin.org");
  client.path("/headers")
      .httpGet([&](const gear::HttpRequest, const gear::HttpResponse response) {
        status = (response.code() == 200) ? "OK" : "FAIL";
        s.notify();
      });

  s.waitFor(chrono::seconds(15));

  EXPECT_EQ(status, "OK");
}

TEST(asio_http, put) {
  Semaphore s;
  std::string status;

  gear::HttpClient client("httpbin.org");
  client.path("/put").httpPut([&](const gear::HttpRequest, const gear::HttpResponse response) {
    status = (response.code() == 200) ? "OK" : "FAIL";
    s.notify();
  });
  s.waitFor(chrono::seconds(15));

  EXPECT_EQ(status, "OK");
}

TEST(asio_http, post) {
  Semaphore s;
  std::string status;

  gear::HttpClient client("httpbin.org");
  client.path("/post").httpPost([&](const gear::HttpRequest, const gear::HttpResponse response) {
    status = (response.code() == 200) ? "OK" : "FAIL";
    s.notify();
  });
  s.waitFor(chrono::seconds(15));

  EXPECT_EQ(status, "OK");
}

TEST(asio_http, patch) {
  Semaphore s;
  std::string status;

  gear::HttpClient client("httpbin.org");
  client.path("/patch")
      .httpPatch([&](const gear::HttpRequest, const gear::HttpResponse response) {
        status = (response.code() == 200) ? "OK" : "FAIL";
        s.notify();
      });
  s.waitFor(chrono::seconds(15));

  EXPECT_EQ(status, "OK");
}

TEST(asio_http, delete) {
  Semaphore s;
  std::string status;

  gear::HttpClient client("httpbin.org");
  client.path("/delete")
      .httpDelete([&](const gear::HttpRequest, const gear::HttpResponse response) {
        status = (response.code() == 200) ? "OK" : "FAIL";
        s.notify();
      });
  s.waitFor(chrono::seconds(15));

  EXPECT_EQ(status, "OK");
}

TEST(asio_http, get_with_header) {
  Semaphore s;
  std::string status;

  gear::HttpClient client("api.github.com");
  gear::HttpRequest requestExecute;
  requestExecute.path("/repos/kittinunf/Fuel/contributors")
      .method(gear::HttpMethod::GET)
      .addHeader("User-Agent", "Fuel");
  client.execute(requestExecute,
                 [&](const gear::HttpRequest, const gear::HttpResponse response) {
                   status = (response.code() == 200) ? "OK" : "FAIL";
                   s.notify();
                 });
  s.waitFor(chrono::seconds(15));

  EXPECT_EQ(status, "OK");
}

TEST(asio_http, get_with_query) {
  Semaphore s;
  std::string status = "Unknown";
  gear::HttpClient client("httpbin.org");
  gear::HttpRequest request1;
  request1.path("/get").method(gear::HttpMethod::GET).addQuery("show_env", "1");
  client.execute(request1, [&](const gear::HttpRequest, const gear::HttpResponse response) {
    status = (response.code() == 200) ? "OK" : "FAIL";
    s.notify();
  });
  s.waitFor(chrono::seconds(30));
  EXPECT_EQ(status, "OK");
}
