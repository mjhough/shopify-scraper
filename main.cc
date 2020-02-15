#include <iostream>
#include <string>

#include "Producer.h"
#include "Semaphore.h"

int main(int argc, char *argv[]) {
  std::queue<std::string> *buffer = new std::queue<std::string>();
  Semaphore *items = new Semaphore(0);

  Producer p1("http://kith.com/products.json?page=1", 1, buffer, items);
  p1.perform();

  return 0;
}
