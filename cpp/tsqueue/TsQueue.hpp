#pragma once

#include "ITsQueue.hpp"
#include "Logger.hpp"
#include "SharedPtr.hpp"

#include <condition_variable>
#include <queue>
#include <mutex>

template <typename T>
class TsQueue: public ITsQueue<T> {
public:
    void push(T val) override {
        std::unique_lock<std::mutex> lock(m);
        q.push(std::move(val));
        cv.notify_one();
    }

    bool waitAndPop(T& item) override {
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

    SharedPtr<T> waitAndPop() override {
        bool success = true;
        std::unique_lock<std::mutex> lock(m);
        counter++;
        cv.wait(lock, [this]() { return !empty() || shutdown; });
        if (shutdown) {
            success = false;
            counter--;
            return {};
        }

        SharedPtr<T> res{new T(std::move(q.front()))};
        q.pop();
        counter--;
        cv.notify_one();
        LOG_DEBUG("After consume. size=%zu", size());
        return res;
       
    } 

    bool tryPop(T& val) override {
        std::unique_lock lock(m);
        if (empty()) {
            return false;
        }

        val = std::move(q.front());
        q.pop();
        return true;
    }

    SharedPtr<T> tryPop() override {
        std::unique_lock lock(m);
        if (empty()) {
            return {};
        }

        SharedPtr<T> res{new T{std::move(q.front())}};
        q.pop();
        return res;
    };

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
