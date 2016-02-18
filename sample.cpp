#include "http/HttpClient.hpp"

using namespace std;
using namespace gear;

int main() {
  HttpClient client("httpbin.org");
  string status;
  client.path("/").httpGet(
      [&](const HttpRequest request, const HttpResponse response) {
        status = (response.code() == 200) ? "OK" : "FAIL";
        cout << "Status: " << status << endl;
        cout << response.body() << endl;
      });
  return 0;
}
