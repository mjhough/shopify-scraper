#include "Semaphore.h"

Semaphore::Semaphore(int count) {
  this->count = count;
}

void Semaphore::post() {
  std::unique_lock<std::mutex> lock(mtx);
  count++;
  cv.notify_one();
}

void Semaphore::wait() {
  std::unique_lock<std::mutex> lock(mtx);
  while(count == 0) {
    cv.wait(lock);
  }
  count--;
}

bool Semaphore::try_wait() {
  std::unique_lock<std::mutex> lock(mtx);
  if (count) {
    count--;
    return true;
  }
  return false;
}
