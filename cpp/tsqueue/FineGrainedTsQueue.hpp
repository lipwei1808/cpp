#pragma once

#include "ITsQueue.hpp"
#include "SharedPtr.hpp"
#include "UniquePtr.hpp"
#include "Logger.hpp"

#include <condition_variable>
#include <mutex>

template <typename T>
class FineGrainedTsQueue: public ITsQueue<T> {
    struct Node;
public:
    FineGrainedTsQueue(): head{new Node{}}, tail{head.get()} {
        LOG_DEBUG("Default Constructor: head=%p, head=%p, tail=%p", &head, head.get(), tail);
    }

    // Pop from head
    SharedPtr<T> tryPop() override {
        LOG_DEBUG("tryPop, return shared ptr");
        std::lock_guard lk{headMutex};
        if (head.get() == getTail()) {
            return {};
        }
        UniquePtr<Node> oldHead = popHead();
        return oldHead->val;
    }

    bool tryPop(T& val) override {
        std::lock_guard lk{headMutex};
        if (head.get() == getTail()) {
            return {};
        }
        
        // Need to ensure copying is done before popping
        // in case of exceptions when copying and data 
        // is lost
        val = std::move(*head->val);
        popHead();
        return true;
    }

    SharedPtr<T> waitAndPop() override {
        LOG_DEBUG("waitAndPop, return shared ptr");
        std::unique_lock lk{headMutex};
        cv.wait(lk, [this]() { return head.get() != getTail(); });
        
        UniquePtr<Node> oldHead = popHead();
        return oldHead->val;
    }

    bool waitAndPop(T& val) override {
        std::unique_lock lk{headMutex};
        cv.wait(lk, [this]() { return head.get() != getTail(); });
        val = std::move(*head->val);
        popHead();
        return true;
    }

    // Push to tail
    void push(T val) override {
        LOG_DEBUG("push");
        SharedPtr<T> valPtr{new T{std::move(val)}};
        UniquePtr<Node> newDummy{new Node{}};
        {
            std::lock_guard lk{tailMutex};
            tail->val = valPtr;
            tail->next = std::move(newDummy);
            tail = tail->next.get();
        }
        cv.notify_one();
    }

    Node* getTail() {
        std::lock_guard lk{tailMutex};
        return tail;
    }

    bool empty() {
        std::lock_guard lk{headMutex};
        return head.get() == getTail();
    }

private:
    UniquePtr<Node> popHead() {
        UniquePtr<Node> oldHead = std::move(head);
        head = std::move(oldHead->next);
        return oldHead;
    }

    struct Node {
        SharedPtr<T> val;
        UniquePtr<Node> next;
    };

    UniquePtr<Node> head;
    Node* tail;

    std::mutex headMutex;
    std::mutex tailMutex;
    std::condition_variable cv;
};
