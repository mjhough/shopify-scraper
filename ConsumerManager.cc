#include "ConsumerManager.h"

ConsumerManager::ConsumerManager(std::queue<std::string> *buffer, Semaphore *items, bool *producers_done, unsigned int max_threads) {
  this->buffer = buffer;
  this->items = items;
  this->producers_done = producers_done;
  this->max_threads = max_threads;
}

int ConsumerManager::run(void) {
  for (unsigned int i = 0; i < max_threads; i++) {
    consumers.push_back(new Consumer(buffer, items));
    threads.push_back(std::thread(&Consumer::perform, consumers[i]));
  }

  while (!*producers_done || !buffer->empty()) {
    // Do nothing and wait for producers to be done and the buffer to be empty
  }

  // Clean up all remaining threads
  while (!consumers.empty()) {
    consumers.back()->stop_thread();
    threads.back().join();
    threads.pop_back();
    delete consumers.back();
    consumers.pop_back();
  }

  return 0;
}
