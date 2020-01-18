#include <iostream>
#include <string>

#include "Producer.h"

int main(int argc, char *argv[]) {
  Producer p1("http://kith.com/products.json?page=1", 1);
  p1.perform();
  p1.printResults();

  return 0;
}
