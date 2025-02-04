#pragma once

#include "Hashmap.hpp"
#include "Worker.hpp"

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <mutex>

class Master;
class HeartbeatMonitor {
    using Timepoint = std::chrono::time_point<std::chrono::system_clock>;
public:
    HeartbeatMonitor(Master* master, std::chrono::seconds expirationTime);
    void registerHeartbeat(WorkerId id);
    void addWorker(WorkerId id);
    void svc();
    void stop();
    void activate();
    void disconnectWorker(WorkerId id);
    ~HeartbeatMonitor();

private:
    void checkWorkers();
    Timepoint getNow();
    inline bool hasExpired(std::chrono::seconds time);

    Master* master = nullptr;
    Hashmap<WorkerId, Timepoint> workers;
    std::chrono::seconds expirationTime;
    std::mutex m;
    std::condition_variable cv;
    std::thread monitorThread;
    bool shutdown = false;
};

inline bool HeartbeatMonitor::hasExpired(std::chrono::seconds time) {
    return time > expirationTime;
}


inline HeartbeatMonitor::Timepoint HeartbeatMonitor::getNow() {
    return std::chrono::system_clock::now();
}
