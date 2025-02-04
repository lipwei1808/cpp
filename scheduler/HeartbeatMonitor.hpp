#pragma once

#include "Hashmap.hpp"
#include "Logger.hpp"
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
    inline bool hasExpired(const Timepoint& time, const Timepoint& now);

    Master* master = nullptr;
    Hashmap<WorkerId, Timepoint> workers;
    std::chrono::seconds expirationTime;
    std::mutex m;
    std::condition_variable cv;
    std::thread monitorThread;
    bool shutdown = false;
};

inline bool HeartbeatMonitor::hasExpired(const Timepoint& time, const Timepoint& now) {
    std::chrono::duration x = now - time;
    LOG_TRACE("now - last heartbeat = %llds", x / std::chrono::seconds{1});
    return x > expirationTime;
}


inline HeartbeatMonitor::Timepoint HeartbeatMonitor::getNow() {
    return std::chrono::system_clock::now();
}
