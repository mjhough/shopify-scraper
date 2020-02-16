#include <thread>
#include <queue>

#include "Consumer.h"

class ConsumerManager {
  public:
    ConsumerManager(std::queue<std::string> *buffer, Semaphore *items, bool *producers_done, unsigned int max_threads);
    int run();
  private:
    std::queue<std::string> *buffer;
    Semaphore *items;
    unsigned int max_threads = 20;
    bool *producers_done;
    std::vector<std::thread> threads;
    std::vector<Consumer*> consumers;
};
