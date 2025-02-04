#pragma once

#include "message.pb.h"

#include <chrono>
#include <thread>

using WorkerId = int;

class Worker {
public:
    Worker(const char* hostname, const char* port,
            std::chrono::seconds heartbeatInterval = std::chrono::seconds{1});
    Worker(Worker&& worker);
    bool connect();
    void run();
    void runHeartbeat();
    ~Worker();
    
private:
    bool sendHeartbeat();
    bool handshake();
    bool execute(Scheduler::TaskType task);
    bool executeTaskOne();
    bool executeTaskTwo();
    void stopHeartbeat();
    int fd = 0;
    const char* hostname = nullptr; 
    const char* port = nullptr;
    std::thread heartbeatThread;
    WorkerId id = -1;
    bool shutdownHeartbeat = false;
    std::chrono::seconds heartbeatInterval = std::chrono::seconds{1};
};

