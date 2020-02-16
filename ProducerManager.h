#include <thread>
#include <queue>

#include "Producer.h"

class ProducerManager {
  public:
    ProducerManager(std::queue<std::string> *buffer, Semaphore *items, bool *empty_page_reached, unsigned int max_threads);
    int run();
  private:
    std::queue<std::string> *buffer;
    Semaphore *items;
    bool *empty_page_reached;    // A producer thread will set this if they reach an empty page
    unsigned int max_threads = 50;
    unsigned int num_pages = 0;
    std::vector<std::thread> threads;
    std::vector<Producer*> producers;
    std::queue<int> pids;               // Store the page numbers of the finished threads
};
