#include <iostream>

#include "../include/mylib.h"
#include "tcp_server.h"

int main() {
  TCPServer server(5000);
  server.establish();
  server.connectToClient();

  while (1) {
    std::string str;
    if (!server.readFromClient(str)) {
      server.disconnectFromClient();
      server.connectToClient();
    } else if (str == "ping") {
      server.writeToClient("pong");
    } else {
      bool res = func_3(str);
      if (res) {
        std::cout << "== SUCCESS. Received correct message: \"" << str
                  << "\" ==" << std::endl;
      } else {
        std::cout << "== ERROR. Message: \"" << str
                  << "\" is incorrect ==" << std::endl;
      }
    }
  }

  server.disconnectFromClient();
  return 0;
}