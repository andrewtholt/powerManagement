/*
 * Thread Safe C++ Q's
 */

#include <iostream>       // std::cin, std::cout
#include <queue>          // std::queue
#include <mutex>
#include <string>

class tQueue {
    private:
        std::mutex mtx;
        std::queue<std::string> msgQueue;

    public:
        void push(std::string msg);   // Add message to Q
        std::string pop();       // Get and remove message from Q
        std::string get();       // Get and leave message on Q
        int depth();        // Return number of messages on Q
        void discard();     // Remove from front of Q and discard.
};

