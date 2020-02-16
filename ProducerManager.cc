#include "ProducerManager.h"

ProducerManager::ProducerManager(std::queue<std::string> *buffer, Semaphore *items, bool *producers_done, unsigned int max_threads, std::string url) {
  this->buffer = buffer;
  this->items = items;
  this->max_threads = max_threads;
  this->producers_done = producers_done;
  this->url = url;
}

int ProducerManager::run(void) {
  bool empty_page_reached = false;

  // Create initial threads
  for (unsigned int i = 0; i < max_threads; i++) {
    producers.push_back(new Producer(url+"/products.json?page="+std::to_string(i+1), i+1, buffer, items, &pids, &empty_page_reached));
    threads.push_back(std::thread(&Producer::perform, producers[i]));
    num_pages++;
  }

  // Keep respawning threads while we haven't yet found an empty page
  while(!empty_page_reached) {
    if (!pids.empty()) {
      // Replace the thread corresponding to a finished page number
      int idx = (pids.front() - 1) % (max_threads - 1);
      threads[idx].join();
      delete producers[idx];
      producers[idx] = new Producer(url+"/products.json?page="+std::to_string(num_pages+1), num_pages+1, buffer, items, &pids, &empty_page_reached);
      threads[idx] = std::thread(&Producer::perform, producers[idx]);
      pids.pop();
      num_pages++;
    }
  }

  // Clean up all remaining threads
  while(!producers.empty()) {
    threads.back().join();
    threads.pop_back();
    delete producers.back();
    producers.pop_back();
    if (!pids.empty())
      pids.pop();
    num_pages++;
  }

  *producers_done = true;

  return num_pages;
}
