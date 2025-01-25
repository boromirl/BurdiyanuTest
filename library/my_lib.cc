#include "my_lib.h"

#include <iostream>

void func_1(std::string &str) {
  std::sort(str.rbegin(), str.rend());

  std::string temp;
  temp.reserve(str.length() + str.length() / 2);

  for (int i = 0; i < str.length(); i++) {
    if (i % 2 == 1) {  // из-за начала с 0
      temp.append("KB");
    } else {
      temp.push_back(str[i]);
    }
  }

  str = temp;
}

int func_2(std::string str) {
  int res = 0;
  for (auto ch : str) {
    if (ch >= '0' && ch <= '9') {
      res += ch - '0';
    }
  }
  return res;
}

bool func_3(std::string str) {
  if (str.length() > 2 && (str.length() % 32 == 0)) {
    return true;
  } else {
    return false;
  }
}