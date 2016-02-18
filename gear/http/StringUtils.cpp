#include "stringUtils.hpp"

namespace gear_utils {

std::string trim(const std::string &s) {
  auto isSpace = [](int c) { return isspace(c); };
  auto front = find_if_not(begin(s), end(s), isSpace);
  auto back = find_if_not(rbegin(s), rend(s), isSpace).base();
  return (back <= front ? std::string() : std::string(front, back));
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::string decodeUrl(const std::string &url) {
  std::ostringstream resultStream;
  for (unsigned int i = 0; i < url.length(); i++) {
    if (url[i] == '%') {
      int dec1, dec2;
      if (-1 != (dec1 = HEX2DEC[(int)url[i + 1]]) &&
          -1 != (dec2 = HEX2DEC[(int)url[i + 2]])) {
        resultStream << ((char)((dec1 << 4) + dec2));
        i += 2;
      }
    } else {
      resultStream << url[i];
    }
  }
  return resultStream.str();
}

std::string encodeUrl(const std::string &url) {
  std::ostringstream resultStream;
  for (const char &c : url) {
    if (SAFE[(unsigned int)c]) {
      resultStream << c;
    } else {
      resultStream << '%' << DEC2HEX[c >> 4] << DEC2HEX[c & 0x0F];
    }
  }
  return resultStream.str();
}
}
