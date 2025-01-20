#pragma once

class Master {
public:
    Master(const char* hostname, const char* port);
    bool init();
    bool listen();
    bool run();
    ~Master();

private:
    void handleWorker(int workerFd);
    bool sendHeartbeat(int workerFd);
    int fd = 0;
    const char* hostname;
    const char* port;
};
