#include "Hashmap.hpp"
#include "Worker.hpp"
#include "Master.hpp"
#include "HeartbeatMonitor.hpp"
#include "Vector.hpp"

#include <chrono>
#include <mutex>

HeartbeatMonitor::HeartbeatMonitor(Master* master, std::chrono::seconds expirationTime):
        master(master), expirationTime(expirationTime) {}

void HeartbeatMonitor::registerHeartbeat(WorkerId id) {
    std::scoped_lock<std::mutex> lock{m};
    if (!workers.contains(id)) {
        LOG_ERROR("Registering heartbeat for invalid worker id=%d", id);
        return;
    }

    workers.insert({id, getNow()});
}

void HeartbeatMonitor::addWorker(WorkerId id) {
    std::scoped_lock<std::mutex> lock{m};
    workers.insert({id, getNow()});
    cv.notify_one();
}

void HeartbeatMonitor::disconnectWorker(WorkerId id) {
    workers.erase(id);
}

void HeartbeatMonitor::checkWorkers() {
    LOG_TRACE("Checking workers now. Number of workers=%zu", workers.size());
    Vector<WorkerId> disconnectedWorkers;
    for (auto& [k, v]: workers) {
        Timepoint now = getNow();
        if (hasExpired(v, now)) {
            disconnectedWorkers.push_back(k);
        }
    }

    for (WorkerId id: disconnectedWorkers) {
        master->handleDisconnect(id);
    }
}

void HeartbeatMonitor::svc() {
    while (!shutdown) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5s);
        std::unique_lock<std::mutex> lock{m};
        cv.wait(lock, [this]{ return workers.size() != 0; });
        checkWorkers();
    }
}

void HeartbeatMonitor::activate() {
    monitorThread = std::thread{&HeartbeatMonitor::svc, this};
}

HeartbeatMonitor::~HeartbeatMonitor() {
    shutdown = true;
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
    shutdown = false;
}

