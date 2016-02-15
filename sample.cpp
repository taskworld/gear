#include "http_client.hpp"

using namespace std;
using namespace gear;

int main() {
  http_client client("httpbin.org");
  string status;
  client.path("/").http_get(
      [&](const http_request request, const http_response response) {
        status = (response.code() == 200) ? "OK" : "FAIL";
        cout << "Status: " << status << endl;
        cout << response.body() << endl;
      });
  return 0;
}
