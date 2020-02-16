#include <string>
#include <thread>

#include "ProducerManager.h"
#include "ConsumerManager.h"
#include "Semaphore.h"

int main(int argc, char *argv[]) {
  std::queue<std::string> *buffer = new std::queue<std::string>();
  Semaphore *items = new Semaphore(0);
  bool producers_done;

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <BASE_URL>" << std::endl;
    delete buffer;
    delete items;
    return -1;
  }

  // Get the future on this so we can see the number of pages
  // Pass in the max num threads here
  ProducerManager pm(buffer, items, &producers_done, 50, argv[1]);
  std::thread pm_thread(&ProducerManager::run, &pm);

  ConsumerManager cm(buffer, items, &producers_done, 20);
  std::thread cm_thread(&ConsumerManager::run, &cm);

  pm_thread.join();
  cm_thread.join();

  delete buffer;
  delete items;

  return 0;
}
