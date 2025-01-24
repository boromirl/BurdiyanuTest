#include "my_lib.h"

void func_1(std::string &str){
    std::sort(str.begin(), str.end(), std::greater<char>());

    std::string temp;
    temp.reserve(str.length() + str.length() / 2);

    for(int i = 0; i < str.length(); i++){
        if(i % 2 == 1){ // из-за начала с 0 (изменить?)
            temp.append("KB");
        } else{
            temp.push_back(str[i]);
        }
    }

    str = temp;
}

int func_2(std::string str){return 0;}

bool func_3(std::string str){return 0;}