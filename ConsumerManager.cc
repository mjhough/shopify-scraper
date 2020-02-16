#include "ConsumerManager.h"

ConsumerManager::ConsumerManager(std::queue<std::string> *buffer, Semaphore *items, bool *empty_page_reached, unsigned int max_threads) {
  this->buffer = buffer;
  this->items = items;
  this->empty_page_reached = empty_page_reached;
  this->max_threads = max_threads;
}

int ConsumerManager::run(void) {
  for (unsigned int i = 0; i < max_threads; i++) {
    consumers.push_back(new Consumer(buffer, items, i, &tids));
    threads.push_back(std::thread(&Consumer::perform, consumers[i]));
  }

  // Keep respawning threads while we haven't yet found an empty page
  while(!*empty_page_reached) {
    if (!tids.empty()) {
      // Replace the thread corresponding to the thread id
      int idx = tids.front();
      threads[idx].join();
      delete consumers[idx];
      consumers[idx] = new Consumer(buffer, items, idx, &tids);
      threads[idx] = std::thread(&Consumer::perform, consumers[idx]);
      tids.pop();
    }
  }

  // Clean up all remaining threads
  while(!tids.empty()) {
    int idx = tids.front();
    threads[idx].join();
    delete consumers[idx];
    tids.pop();
  }

  return 0;
}
