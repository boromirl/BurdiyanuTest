#include "mylib.h"

#include <iostream>

void func_1(std::string &str) {
  std::sort(str.rbegin(), str.rend());

  std::string temp;
  temp.reserve(str.length() + str.length() / 2);

  for (int i = 0; i < str.length(); i++) {
    if (i % 2 == 1) { // из-за начала с 0
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

/*
  Неоднозначная функция. В описании задания сказано, что функция должна
  анализировать длину строки. Функция должна проверять, что длина больше 2 и
  кратна 32. Но по заданию в этой строке находится число (которое к тому же
  сумма цифр из строки с максимальным размером 64).
  Поэтому я сделал проверку самого числа на кратность 32.
  Возможно я что-то неправильно понял.
*/
bool func_3(std::string str) {
  if (str.length() > 2 && (stoi(str) % 32 == 0)) {
    return true;
  } else {
    return false;
  }
}