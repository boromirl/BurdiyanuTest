#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

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

  // !!! default values???
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
    while (!is_connected) {
      int n = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
      if (n < 0) {
        perror("\n== ERROR connecting to server ==");
        std::cerr << "Retrying (try " << counter + 1 << ") in " << delay
                  << " seconds...\n"
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(delay));
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

  std::string recvFromServer() {
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

  int sendToServer(std::string message) {
    int n = send(sockfd, message.c_str(), message.length(), 0);
    return n;
  }

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

class ThreadManager {
private:
  TCPClient client;
  // !!! i think it would be better to make a queue. Is it?
  std::queue<std::string> buffer_queue;
  std::mutex mtx;
  std::condition_variable condition_variable;

  bool is_valid(std::string str) {
    if (str.length() > 64) {
      return false;
    }

    for (char ch : str) {
      if (!isdigit(ch)) {
        return false;
      }
    }

    return true;
  }

public:
  ThreadManager() {
    client.establish();
    // client.connectToServer("localhost", 5000);
  };
  ~ThreadManager() { client.disconnectFromServer(); };

  void job1() {
    std::string str;

    while (1) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      std::cout << "Enter a string: " << std::endl;
      getline(std::cin, str);

      if (is_valid(str)) {
        // std::cout << "== Entered string \"" << str
        //           << "\" is valid ==" << std::endl;
        func_1(str);
        // std::cout << "== String after func_1: \"" << str
        //           << "\" ==" << std::endl;

        // lock mutex when using shared resource
        mtx.lock();
        this->buffer_queue.push(str);
        mtx.unlock();
        this->condition_variable.notify_one();
      } else {
        std::cout << "== String \"" << str
                  << "\"is invalid. String should be 64 characters long or "
                     "less and all characters should be digits =="
                  << std::endl;
      }

      str.clear();
    }
  }

  void job2() {
    std::string str;
    client.connectToServer();

    while (1) {
      std::unique_lock<std::mutex> ul(mtx);
      condition_variable.wait(ul,
                              [this]() { return !this->buffer_queue.empty(); });
      // lock mutex when using shared resource
      str = this->buffer_queue.front();
      this->buffer_queue.pop();
      ul.unlock();

      std::cout << "\n== Data received from first thread: \"" << str
                << "\" ==\n"
                << std::endl;

      int res = func_2(str);

      // std::cout << "== Send \"" << res << "\" to program 2 ==" << std::endl;

      while (!client.checkConnection()) {
        std::cout << "\n== Not connected to the server. Attempt to reconnect "
                     "in 3 seconds ==\n"
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        client.reconnectToServer();
      }
      client.sendToServer(std::to_string(res));
    }
  }

  void run() {
    std::thread worker1(&ThreadManager::job1, this);
    std::thread worker2(&ThreadManager::job2, this);

    worker1.join();
    worker2.join();
  }
};

int main() {
  ThreadManager tm;
  tm.run();
  return 0;
}