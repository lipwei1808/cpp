#pragma once

#include "Distributor.hpp"
#include "Hashmap.hpp"
#include "Worker.hpp"
#include "String.hpp"
#include "UniquePtr.hpp"

#include <barrier>

class HeartbeatMonitor;
class Master {
public:
    Master(const char* hostname, const char* port);
    bool init();
    bool listen();
    bool run();
    void stop();
    ~Master();

private:
    bool handle(int fd, const String& s);
    bool handleClient(int clientFd, const Scheduler::Message& msg);
    bool handleWorker(int workerFd, const Scheduler::Message& msg);
    void handleDisconnect(int fd);
    void handleDisconnectWorker(int workerFd);
    void handleNewConnection();
    bool handleHeartbeat(int workerFd);
    bool sendHandshakeResponse(int workerFd);
    bool handleTaskResponse(int workerFd, const std::string& data);
    int fd = 0;
    int kq = 0;
    bool shutdown = false;
    const char* hostname;
    const char* port;
    Hashmap<int, WorkerId> workerFds;
    Hashmap<int, int> clientFds;
    Distributor distributor;
    UniquePtr<HeartbeatMonitor> heartbeatMonitor;
    std::barrier<std::function<void()>> barrier{2, []{}};

    friend class HeartbeatMonitor;
};

