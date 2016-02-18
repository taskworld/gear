
#include "socket/socket.hpp"
#include "Semaphore.cpp"
#include <gtest/gtest.h>

using namespace std;

TEST(socket, test_ssl) {
  Semaphore s;

  gear::websocket_endpoint socket;
  socket
      .with_tls([this](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<asio::ssl::context>(
            asio::ssl::context::tlsv12_client);
      })
      .on_open([&socket](client::connection_ptr) {
        cout << "Open" << endl;
        socket.send("Hello world!");
      })
      .on_fail([&s](client::connection_ptr con) {
        cout << "Fail" << endl;
        cout << "Local close " << con->get_local_close_reason() << endl;
        cout << "Remote close " << con->get_remote_close_reason() << endl;
        cout << con->get_ec() << " - " << con->get_ec().message() << endl;
        s.notify();
      })
      .on_close([&s](client::connection_ptr) {
        cout << "Close" << endl;
        s.notify();
      })
      .on_message(
          [&s](websocketpp::frame::opcode::value value, std::string payload) {
            cout << " Message: " << payload << endl;
          })
      .connect("wss://echo.websocket.org");

  s.waitFor(chrono::seconds(10));
}
