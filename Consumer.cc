#include "Consumer.h"

Consumer::Consumer(std::queue<std::string> *buffer, Semaphore *items) {
  this->buffer = buffer;
  this->items = items;
}

void Consumer::perform(void) {
  items->wait();
  std::cout << buffer->front() << std::endl;
  buffer->pop();
}
