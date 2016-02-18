
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
      .withTls([this](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);
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

  socket
      .withTls([this](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);
      })
      .onOpened([&socket](client::connection_ptr con) {
        cout << "Open" << endl;
        cout << "Force close socket channel. Prepare to retry." << endl;
        socket.close(websocketpp::close::status::going_away, "Force close");
      })
      .onRetryStarted([](int retry_attempt) {
        cout << "Start Retry #" << retry_attempt << " after wait for " << retry_attempt * 3
             << " seconds." << endl;
      })
      .onRetryOpened([&s](client::connection_ptr con) {
        cout << "Retry successful" << endl;
        s.notify();
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
}
