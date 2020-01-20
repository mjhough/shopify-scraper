#include <iostream>
#include <string>
#include <queue>

class Worker {
  protected:
    static std::queue<std::string> buffer;     // Buffer. Producer puts results into here for consumer to consume.
    // General semaphore needed:
    // - items (init to 0 and represent how many spaces in the buffer are full)
    // Note: No need to keep track of spaces free for producer to wait on since we have an unlimited sized dynamic buffer
};
