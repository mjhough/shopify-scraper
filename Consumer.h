#include "Worker.h"
#include "Semaphore.h"

class Consumer : protected Worker {
  public:
    // Constructor
    Consumer(std::queue<std::string> *buffer, Semaphore *items, int tid, std::queue<int> *tids);

    // Functions
    void perform();

  private:
    int tid;
    std::queue<int> *tids;
};
