#include <string>
#include <thread>

#include "ProducerManager.h"
#include "ConsumerManager.h"
#include "Semaphore.h"

int main(int argc, char *argv[]) {
  std::queue<std::string> *buffer = new std::queue<std::string>();
  Semaphore *items = new Semaphore(0);
  bool empty_page_reached;

  // Get the future on this so we can see the number of pages
  // Pass in the max num threads here
  ProducerManager pm(buffer, items, &empty_page_reached, 10);
  std::thread pm_thread(&ProducerManager::run, &pm);

  ConsumerManager cm(buffer, items, &empty_page_reached, 5);
  std::thread cm_thread(&ConsumerManager::run, &cm);

  pm_thread.join();
  cm_thread.join();

  return 0;
}
