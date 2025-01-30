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
  std::queue<std::string> buffer_queue;
  std::mutex mtx;
  std::condition_variable condition_variable;

  std::atomic<bool> is_running = true;

  // Проверка вводимой строки (<= 60 символов, только цифры)
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
  ThreadManager() { client.establish(); };
  ~ThreadManager() { client.disconnectFromServer(); };

  // Работа первого потока
  void job1() {
    std::string str;

    while (is_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      std::cout << "Enter a string: " << std::endl;
      getline(std::cin, str);

      // выход из программы при вводе q
      if (str == "q") {
        is_running = false;
        condition_variable.notify_one(); // оповещение второго потока
        break;
      }

      if (is_valid(str)) {
        func_1(str);

        mtx.lock(); // заблокировать mutex при работе с общим буфером
        this->buffer_queue.push(str);
        mtx.unlock(); // разблокировать

        this->condition_variable.notify_one(); // оповещение второго потока
      } else {
        std::cout << "\n== The provided string \"" << str
                  << "\" is invalid. It should be 64 characters or "
                     "fewer in length, and all characters should be digits ==\n"
                  << std::endl;
      }
      str.clear();
    }
  }

  // работа второго потока
  void job2() {
    std::string str;
    client.connectToServer();

    while (is_running) {
      std::unique_lock<std::mutex> ul(mtx);
      // Ожидание оповещения от первого потока.
      // После оповещения второй поток будет обрабатывать строки из очереди пока
      // очередь не опустеет, или завершит поток при вводе q
      condition_variable.wait(
          ul, [this]() { return !is_running || !this->buffer_queue.empty(); });

      // завершение работы потока
      if (!is_running) {
        break;
      }

      // mutex автоматически заблокирован методов wait
      str = this->buffer_queue.front();
      this->buffer_queue.pop();
      ul.unlock(); // разбликировка mutex

      std::cout << "\n== Data received from first thread: \"" << str
                << "\" ==\n"
                << std::endl;

      int res = func_2(str);

      // Перед отправкой проверяем соединение
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