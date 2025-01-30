#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "../include/mylib.h"
#include "tcp_client.h"

class ThreadManager {
private:
  TCPClient client;
  // !!! i think it would be better to make a queue. Is it?
  std::queue<std::string> buffer_queue;
  std::mutex mtx;
  std::condition_variable condition_variable;

  std::atomic<bool> is_running = true;

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

    while (is_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      std::cout << "Enter a string: " << std::endl;
      getline(std::cin, str);

      if (str == "q") {
        is_running = false;
        condition_variable.notify_one();
        break;
      }

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
        std::cout << "\n== The provided string \"" << str
                  << "\" is invalid. It should be 64 characters or "
                     "fewer in length, and all characters should be digits ==\n"
                  << std::endl;
      }
      str.clear();
    }
  }

  void job2() {
    std::string str;
    client.connectToServer();

    while (is_running) {
      std::unique_lock<std::mutex> ul(mtx);
      condition_variable.wait(
          ul, [this]() { return !is_running || !this->buffer_queue.empty(); });

      if (!is_running) {
        break;
      }

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