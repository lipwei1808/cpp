#pragma once

#include "Logger.hpp"

#include <condition_variable>
#include <queue>
#include <mutex>

template <typename T>
class TsQueue {
public:
    void push(const T& val) {
        std::unique_lock<std::mutex> lock(m);
        q.push(val);
        cv.notify_one();
    }

    bool consumeSync(T& item) {
        bool success = true;
        std::unique_lock<std::mutex> lock(m);
        counter++;
        LOG_DEBUG("Consume sync");
        cv.wait(lock, [this]() { return !empty() || shutdown; });
        if (shutdown) {
            success = false;
            counter--;
            return false;
        }

        item = std::move(q.front());
        q.pop();
        counter--;
        cv.notify_one();
        LOG_DEBUG("After consume. size=%zu", size());
        return true;
    }

    bool empty() const {
        return q.empty();
    }

    size_t size() const {
        return q.size();
    }
    
    void stop() {
        LOG_DEBUG("Stopping queue. sync counter=%u", counter);
        std::unique_lock<std::mutex> lock(m);
        shutdown = true;
        cv.notify_all();
        syncCv.wait(lock, [this]() { return counter == 0; });
        shutdown = false;
    }

    ~TsQueue() {
        stop();
    }
    
private:
    std::mutex m;
    std::queue<T> q;
    std::condition_variable cv;
    std::condition_variable syncCv;
    unsigned int counter = 0;
    bool shutdown = false;
};
