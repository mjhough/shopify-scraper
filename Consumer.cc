#include "Consumer.h"

Consumer::Consumer(std::queue<std::string> *buffer, Semaphore *items) {
  this->buffer = buffer;
  this->items = items;
}

void Consumer::perform(void) {
  // Thread will keep running until cancelled
  while(should_run) {
    if (items->try_wait()) {
      std::cout << buffer->front() << std::endl;
      buffer->pop();
    }
  }
}

void Consumer::stop_thread(void) {
  should_run = false;
}
