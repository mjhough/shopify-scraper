#include "Worker.h"
#include "Semaphore.h"

class Consumer : protected Worker {
  public:
    // Constructor
    Consumer(std::queue<std::string> *buffer, Semaphore *items);

    // Functions
    void perform();
    void stop_thread();

  private:
    bool should_run = true;
};
