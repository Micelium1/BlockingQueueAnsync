#include <iostream>
#include "BlockingQueue.h"
#include <future>
#include <vector>
#include <chrono>
void addNumbers(BlockingQueue<int>* queue)
{
    int pocket = 100;
    for (int i = 0; i < pocket; ++i)
    {
        queue->atomicEnque(i);
        std::this_thread::sleep_for(std::chrono::microseconds(rand()));
    }
}
void unNumber(BlockingQueue<int>* queue)
{
    while (true)
    {
        queue->atomicDeque();
        std::this_thread::sleep_for(std::chrono::microseconds(rand()));
    }
}
int main()
{
    srand(time(NULL));
    BlockingQueue<int> queue(5);
    std::vector<std::future<void>> inFuncs;
    std::vector<std::future<void>> outFuncs;
    for (int i = 0; i < 8; ++i)
    {
        inFuncs.push_back(std::async(std::launch::async, &addNumbers,&queue));
        
    }
    for (int i = 0; i < 4; ++i)
    {
        outFuncs.push_back(std::async(std::launch::async, &unNumber, &queue));

    }
}

