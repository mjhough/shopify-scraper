#ifndef WORKER_H
#define WORKER_H

#include <iostream>
#include <string>
#include <queue>

#include "Semaphore.h"

class Worker {
  protected:
    // Each worker has pointers to the below:
    std::queue<std::string> *buffer;     // Buffer. Producer puts results into here for consumer to consume.
    Semaphore *items;                 // General semaphore that represents num spaces in the buffer that are full
    // Note: No need to keep track of spaces free for producer to wait on since we have an unlimited sized dynamic buffer
};

#endif
