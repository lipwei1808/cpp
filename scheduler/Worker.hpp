#pragma once

#include <thread>

class Worker {
public:
    Worker(const char* hostname, const char* port);
    Worker(Worker&& worker);
    bool connect();
    void runHeartbeat();
    ~Worker();
    
private:
    bool sendHeartbeat();

    int fd = 0;
    const char* hostname = nullptr; 
    const char* port = nullptr;
    std::thread heartbeatThread;
};

