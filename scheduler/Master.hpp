#pragma once

#include "Distributor.hpp"
#include "Hashmap.hpp"
#include "Worker.hpp"

class HeartbeatMonitor;
class Master {
public:
    Master(const char* hostname, const char* port);
    bool init();
    bool listen();
    bool run();
    ~Master();

private:
    bool handleClient(int clientFd);
    void handleWorker(int workerFd);
    void handleDisconnect(int workerFd);
    void handleNewConnection();
    bool handleHeartbeat(int workerFd);
    bool sendHandshakeResponse(int workerFd);
    bool handleTaskResponse(int workerFd, const std::string& data);
    int fd = 0;
    int kq = 0;
    const char* hostname;
    const char* port;
    Hashmap<int, WorkerId> workerFds;
    Hashmap<int, int> clientFds;
    Distributor distributor;
    std::unique_ptr<HeartbeatMonitor> heartbeatMonitor;
    friend class HeartbeatMonitor;
};

