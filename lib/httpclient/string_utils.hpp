#pragma once

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace gear_utils {
inline string trim(const string &s) {
  auto is_space = [](int c) { return isspace(c); };
  auto front = find_if_not(begin(s), end(s), is_space);
  auto back = find_if_not(rbegin(s), rend(s), is_space).base();
  return (back <= front ? string() : string(front, back));
}

inline std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

// string url encode and decode
const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
const int HEX2DEC[256] = {
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* 1 */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* 2 */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* 3 */ 0,  1,  2,  3,  4,  5,  6,  7,
    8,          9,  -1, -1, -1, -1, -1, -1,

    /* 4 */ -1, 10, 11, 12, 13, 14, 15, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* 5 */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* 6 */ -1, 10, 11, 12, 13, 14, 15, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* 7 */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,

    /* 8 */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* 9 */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* A */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* B */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,

    /* C */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* D */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* E */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1,
    /* F */ -1, -1, -1, -1, -1, -1, -1, -1,
    -1,         -1, -1, -1, -1, -1, -1, -1};

// Only alphanum is safe.
const bool SAFE[256] = {
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

    /* 4 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    /* 6 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

    /* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* C */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* D */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* E */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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
