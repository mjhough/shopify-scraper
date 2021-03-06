#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>

class Semaphore {
  public:
    // Constructor
    Semaphore(int count = 0);

    // Functions
    void post();
    void wait();
    bool try_wait();

  private:
    // Variables
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

#endif
