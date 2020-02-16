#include <thread>
#include <queue>

#include "Consumer.h"

class ConsumerManager {
  public:
    ConsumerManager(std::queue<std::string> *buffer, Semaphore *items, bool *empty_page_reached, unsigned int max_threads);
    int run();
  private:
    std::queue<std::string> *buffer;
    Semaphore *items;
    unsigned int max_threads = 20;
    bool *empty_page_reached;
    std::vector<std::thread> threads;
    std::vector<Consumer*> consumers;
    std::queue<int> tids;              // thread ids
};
