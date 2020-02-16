#include "Consumer.h"

Consumer::Consumer(std::queue<std::string> *buffer, Semaphore *items, int tid, std::queue<int> *tids) {
  this->buffer = buffer;
  this->items = items;
  this->tid = tid;
  this->tids = tids;
}

void Consumer::perform(void) {
  items->wait();
  std::cout << buffer->front() << std::endl;
  // std::string front = buffer->front();
  buffer->pop();

  // Push finished thread id onto queue
  tids->push(tid);
}
