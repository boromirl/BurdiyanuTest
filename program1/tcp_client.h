#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

class TCPClient {
private:
  int sockfd;

public:
  TCPClient() : sockfd(-1) {}

  ~TCPClient() { close(sockfd); }

  // Создание сокета
  bool establish() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      perror("ERROR opening socket");
      return false;
    }

    return true;
  }

  // Подключение к серверу по имени и порту. Delay - задержка между попытками
  // подключения (в секундах)
  bool connectToServer(std::string hostname = "localhost", int port = 5000,
                       int delay = 3) {
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

    bool is_connected = false;
    int counter = 0;
    // Попытки подключения к серверу
    while (!is_connected) {
      int n = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
      if (n < 0) {
        perror("\n== ERROR connecting to server ==");
        std::cerr << "Retrying (try " << counter + 1 << ") in " << delay
                  << " seconds...\n"
                  << std::endl;
        sleep(delay);
        counter++;
      } else {
        is_connected = true;
        std::cout << "\n== Successfully connected to: " << hostname << ":"
                  << port << " ==\n"
                  << std::endl;
      }
    }

    return true;
  }

  // Переподключение к серверу с обновлением сокета
  bool reconnectToServer() {
    bool res;
    close(sockfd);
    res = establish();
    if (!res) {
      return false;
    }
    res = connectToServer();
    return res;
  }

  // Получение строки от сервера
  std::string recvFromServer() {
    char buffer[1024];

    int n = recv(sockfd, buffer, sizeof(buffer), 0);
    if (n < 0) {
      perror("ERROR reading from socket");
    }
    if (n == 0) { // успешно прочитано нулевое сообщение = сервер отключился
      return "";
    }
    return std::string(buffer, n);
  }

  // Отправка строки серверу
  int sendToServer(std::string message) {
    int n = send(sockfd, message.c_str(), message.length(), 0);
    return n;
  }

  // Проверка подключения путем отправки сообщения "ping" и ожидания ответа от
  // сервера
  bool checkConnection() {
    sendToServer("ping");
    std::string buffer = recvFromServer();
    if (buffer == "pong")
      return true;
    else {
      return false;
    }
  }

  void disconnectFromServer() { close(sockfd); }
};