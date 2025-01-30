#include <iostream>

#include "../include/mylib.h"
#include "tcp_server.h"

int main() {
  TCPServer server(5000);
  server.establish();
  server.connectToClient();

  // Основной цикл. Прослушивает сокет на сообщения от клиента
  while (1) {
    std::string str;
    if (!server.recvFromClient(str)) {
      std::cout << "== ERROR. No connection ==" << std::endl;
      server.disconnectFromClient(); // переподключение к клиенту
      server.connectToClient();
    } else if (str == "ping") { // клиент проверяет соединение
      server.sendToClient("pong");
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