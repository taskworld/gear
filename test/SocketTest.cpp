
#include "Semaphore.cpp"
#include "socket/SocketClient.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(SocketConnection, ssl) {
  Semaphore s;

  gear::WebSocketEndpoint socket;
  string sendMessage = "Hello Socket SSL";
  string receivedMessage = "";

  socket
      .withTls([](websocketpp::connection_hdl) {
        return make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);
      })
      .onOpened([&sendMessage, &socket](client::connection_ptr) {
        cout << "Open" << endl;
        socket.send(sendMessage);
      })
      .onFailed([&s](client::connection_ptr con) {
        cout << "Fail" << endl;
        cout << "Local close " << con->get_local_close_reason() << endl;
        cout << "Remote close " << con->get_remote_close_reason() << endl;
        cout << con->get_ec() << " - " << con->get_ec().message() << endl;
        s.notify();
      })
      .onClosed([&s](client::connection_ptr) {
        cout << "Close" << endl;
        s.notify();
      })
      .onMessageReceived(
          [&receivedMessage, &s](websocketpp::frame::opcode::value value, string payload) {
            receivedMessage = payload;
            cout << " Message: " << payload << endl;
          })
      .connect("wss://echo.websocket.org");

  s.waitFor(chrono::seconds(15));
  ASSERT_EQ(sendMessage, receivedMessage);
}

TEST(Socket, AutoRetry) {
  Semaphore s;
  gear::WebSocketEndpoint socket;
  int retry = 0;

  socket
      .withTls([](websocketpp::connection_hdl) {
        return make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);
      })
      .onOpened([&retry, &s, &socket](client::connection_ptr con) {
        cout << "Open" << endl;
        if (retry > 0) {
          s.notify();
        } else {
          cout << "Force close socket channel. Prepare to Retry." << endl;
          socket.close(websocketpp::close::status::going_away, "Force close");
        }
      })
      .onRetry([&retry](int retryCount) {
        retry = retryCount;
        cout << "Start Retry #" << retryCount << " after wait for " << retryCount * 3 << " seconds."
             << endl;
      })
      .onFailed([&s](client::connection_ptr con) {
        cout << "Fail" << endl;
        cout << "Local close " << con->get_local_close_reason() << endl;
        cout << "Remote close " << con->get_remote_close_reason() << endl;
        cout << con->get_ec() << " - " << con->get_ec().message() << endl;
        s.notify();
      })
      .onClosed([&s](client::connection_ptr) {
        cout << "Close" << endl;
        s.notify();
      })
      .onMessageReceived([&s](websocketpp::frame::opcode::value value, string payload) {
        cout << " Message: " << payload << endl;
      })
      .connect("wss://echo.websocket.org");

  s.waitFor(chrono::seconds(15));
  ASSERT_TRUE(retry > 0);
}
