#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

  // создание сокета. Заполнение необходимых данных о сервере
  bool establish() {
    int n = 0; // for error handling
    this->serverfd = socket(AF_INET, SOCK_STREAM, 0);
    /* семейство AF_UNIX подошло бы лучше для взаимодействия программ на одной
    машине. Однако, так как это тестовое задание, я решил использовать AF_INET и
    работать с программами через интернет */

    if (serverfd < 0) {
      perror("ERROR opening socket");
      return false;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->port);
    n = bind(serverfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
      perror("ERROR on binding");
      return false;
    }

    n = listen(serverfd, 5); // не обязательно 5, можно и меньше
    if (n < 0) {
      perror("ERROR on listen");
      return false;
    }

    return true;
  }

  // Прослушивание и принятие соединения от клиента
  bool connectToClient() {
    socklen_t clilen = sizeof(cli_addr);
    std::cout << "== Listening... ==" << std::endl;
    this->clientfd = accept(serverfd, (struct sockaddr *)&cli_addr, &clilen);
    if (clientfd < 0) {
      perror("ERROR on access");
      return false;
    }

    std::cout << "== Connection accepted == " << std::endl;
    return true;
  }

  // Получение данных от клиента
  bool recvFromClient(std::string &str) {
    char buffer[255];
    int n = recv(clientfd, buffer, 255, 0);
    if (n < 0) {
      perror("ERROR reading from socket");
      return false;
    }
    if (n == 0) { // клиент отключился
      return false;
    }
    str = std::string(buffer, n);
    return true;
  }

  // Отправка данных клиенту (используется только для подтверждения наличия
  // подключения)
  void sendToClient(const std::string &message) {
    int n = send(clientfd, message.c_str(), message.length(), 0);
    if (n < 0) {
      perror("ERROR writing to socket");
    }
  }

  void disconnectFromClient() { close(clientfd); }
};