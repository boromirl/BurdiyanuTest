#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "../include/mylib.h"

class TCPClient {
private:
  int sockfd;

public:
  TCPClient() : sockfd(-1) {}

  ~TCPClient() { close(sockfd); }

  bool establish() { // !!! rename ???
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      perror("ERROR opening socket");
      return false;
    }

    return true;
  }

  bool connectToServer(std::string hostname, int port) {
    struct hostent *server;
    struct sockaddr_in serv_addr;

    server = gethostbyname(hostname.c_str());
    if (server == NULL) {
      perror("ERROR, no such host");
      return false;
    }
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memmove((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(port);

    int n = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
      perror("ERROR connecting to server");
      return false;
    }

    return true;
  }

  std::string readFromServer() {
    char buffer[1024];

    int n = recv(sockfd, buffer, sizeof(buffer), 0);
    if (n < 0) {
      perror("ERROR reading from socket");
    }
    if (n == 0) {
      return "";
    }
    return std::string(buffer, n);
  }

  bool writeToServer(std::string message) {
    int n = send(sockfd, message.c_str(), message.length(), 0);
    if (n < 0) {
      perror("ERROR writing to socket");
      return false;
    }
    return true;
  }

  void disconnectFromServer() { close(sockfd); }
};

int main() { return 0; }