#include "string_utils.hpp"

namespace gear_utils {

string trim(const string &s) {
  auto is_space = [](int c) { return isspace(c); };
  auto front = find_if_not(begin(s), end(s), is_space);
  auto back = find_if_not(rbegin(s), rend(s), is_space).base();
  return (back <= front ? string() : string(front, back));
}

vector<string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

string decode_url(const string &url) {
  ostringstream result_stream;
  for (unsigned int i = 0; i < url.length(); i++) {
    if (url[i] == '%') {
      int dec1, dec2;
      if (-1 != (dec1 = HEX2DEC[(int)url[i + 1]]) &&
          -1 != (dec2 = HEX2DEC[(int)url[i + 2]])) {
        result_stream << ((char)((dec1 << 4) + dec2));
        i += 2;
      }
    } else {
      result_stream << url[i];
    }
  }
  return result_stream.str();
}

string encode_url(const string &url) {
  ostringstream result_stream;
  for (const char &c : url) {
    if (SAFE[(unsigned int)c]) {
      result_stream << c;
    } else {
      result_stream << '%' << DEC2HEX[c >> 4] << DEC2HEX[c & 0x0F];
    }
  }
  return result_stream.str();
}
}
