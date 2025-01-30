#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "../include/mylib.h"

class TCPServer {
private:
  int port;
  int serverfd, clientfd;

  struct sockaddr_in serv_addr, cli_addr;

public:
  TCPServer(int port) : port(port), serverfd(-1), clientfd(-1) {}

  ~TCPServer() {
    close(serverfd);
    close(clientfd);
  }

  bool establish() {
    int n = 0; // for error handling
    // create a TCP socket
    // !!!!!!!!!!! add error for failed opening a socket?
    this->serverfd = socket(AF_INET, SOCK_STREAM, 0);
    /* семейство AF_UNIX подошло бы лучше для взаимодействия программ на одной
    машине. Однако, так как это тестовое задание, я решил использовать AF_INET и
    работать с программами через интернет */
    if (serverfd < 0) {
      perror("ERROR opening socket");
      return false;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr =
        INADDR_ANY; // !!!! how does it generate an address?
    serv_addr.sin_port = htons(this->port);
    n = bind(serverfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
      perror("ERROR on binding");
      return false;
    }

    // !!! I'm not sure if server should start to listen here or in run
    n = listen(serverfd, 5); // !!!! maybe just 1? I only need one connection?
    if (n < 0) {
      perror("ERROR on listen");
      return false;
    }

    return true;
  }

  bool connectToClient() {
    socklen_t clilen = sizeof(cli_addr);
    this->clientfd = accept(serverfd, (struct sockaddr *)&cli_addr, &clilen);
    if (clientfd < 0) {
      perror("ERROR on access");
      return false;
    }
    return true;
  }

  bool readFromClient(std::string &str) {
    char buffer[255];
    int n = recv(clientfd, buffer, 255, 0); // !!! 255 or more?
    if (n < 0) {
      perror("ERROR reading from socket");
      return false;
    }
    if (n == 0) { // client disconnected
      return false;
    }
    str = std::string(buffer, n);
    return true;
  }

  void writeToClient(const std::string &message) {
    int n = send(clientfd, message.c_str(), message.length(), 0);
    if (n < 0) {
      perror("ERROR writing to socket");
    }
  }

  void disconnectFromClient() { close(clientfd); }

  // !!! void shutdown()? i don't need it, but i may make it
};

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
        std::cout << "== Successfully got the right message: \"" << str
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