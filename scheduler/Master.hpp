#pragma once

#include <Hashmap.hpp>

class Master {
public:
    Master(const char* hostname, const char* port);
    bool init();
    bool listen();
    bool run();
    ~Master();

private:
    void handleWorker(int workerFd);
    void handleDisconnect(int workerFd);
    void handleNewConnection();
    bool sendHeartbeat(int workerFd);
    bool sendHandshakeResponse(int workerFd);
    int fd = 0;
    int kq = 0;
    const char* hostname;
    const char* port;
    Hashmap<int, int> workerFds;
};
