#include "ProducerManager.h"

ProducerManager::ProducerManager(std::queue<std::string> *buffer, Semaphore *items, bool *empty_page_reached, unsigned int max_threads) {
  this->buffer = buffer;
  this->items = items;
  this->empty_page_reached = empty_page_reached;
  this->max_threads = max_threads;
}

int ProducerManager::run(void) {
  // Create initial threads
  for (unsigned int i = 0; i < max_threads; i++) {
    producers.push_back(new Producer("https://negativeunderwear.com/products.json?page="+std::to_string(i+1), i+1, buffer, items, &pids, empty_page_reached));
    threads.push_back(std::thread(&Producer::perform, producers[i]));
    num_pages++;
  }

  // Keep respawning threads while we haven't yet found an empty page
  while(!*empty_page_reached) {
    if (!pids.empty()) {
      // Replace the thread corresponding to a finished page number
      int idx = (pids.front() - 1) % (max_threads - 1);
      std::cout << idx << std::endl;
      threads[idx].join();
      delete producers[idx];
      producers[idx] = new Producer("https://negativeunderwear.com/products.json?page="+std::to_string(num_pages+1), num_pages+1, buffer, items, &pids, empty_page_reached);
      threads[idx] = std::thread(&Producer::perform, producers[idx]);
      pids.pop();
      num_pages++;
    }
  }

  // Clean up all remaining threads
  while(!pids.empty()) {
    int idx = pids.front() - 1;
    threads[idx].join();
    delete producers[idx];
    pids.pop();
    num_pages++;
  }

  return num_pages;
}
