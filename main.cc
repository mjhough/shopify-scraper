#include <string>
#include <thread>

#include "Producer.h"
#include "Consumer.h"
#include "Semaphore.h"

int main(int argc, char *argv[]) {
  std::queue<std::string> *buffer = new std::queue<std::string>();
  Semaphore *items = new Semaphore(0);

  // Testing with just one producer and one consumer
  Consumer *c1 = new Consumer(buffer, items);
  std::thread cth1(&Consumer::perform, c1);

  Producer *p1 = new Producer("http://kith.com/products.json?page=1", 1, buffer, items);
  std::thread pth1(&Producer::perform, p1);

  pth1.join();
  cth1.join();

  delete c1;
  delete p1;

  return 0;
}
