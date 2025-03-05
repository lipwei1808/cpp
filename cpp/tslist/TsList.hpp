#pragma once

#include "SharedPtr.hpp"
#include "UniquePtr.hpp"

#include <functional>
#include <mutex>

template <typename T>
class TsList {
public:
    TsList(): head{new Node} {}

    void pushFront(T val) {
        LOG_DEBUG("pushFront");
        SharedPtr<T> v{new T{val}};
        UniquePtr<Node> newHead{new Node{v}};
        std::unique_lock lk{head->m};
        UniquePtr<Node> oldHead = std::move(head->next);
        head->next = std::move(newHead);
        head->next->next = std::move(oldHead);
    }

    void forEach(std::function<void(T&)> fn) {
        LOG_DEBUG("forEach");
        std::unique_lock lk{head->m};
        Node* cur = head.get();
        while (cur->next.get() != nullptr) {
            LOG_DEBUG("Looping over Node* cur=%p", cur);
            Node* next = cur->next.get();
            std::unique_lock nextLk{next->m};
            lk.unlock();
            fn(*next->val);
            cur = next;
            lk = std::move(nextLk);
        }
    }

    void removeIf(std::function<bool(T&)> pred) {
        LOG_DEBUG("removeIf");
        std::unique_lock lk{head->m};
        Node* cur = head.get();
        while (cur->next.get() != nullptr) {
            LOG_DEBUG("Looping over Node* cur=%p", cur);
            Node* next = cur->next.get();
            std::unique_lock nextLk{next->m};
            bool res = pred(*next->val);
            if (res) {
                UniquePtr<Node> nextPtr = std::move(cur->next);
                cur->next = std::move(nextPtr->next);

                // We need to unlock mutex first before nextPtr
                // is destroyed as it is undefined behaviour
                // to destroy a locked mutex
                nextLk.unlock();
            } else {
                lk.unlock();
                cur = next;
                lk = std::move(nextLk);
            }
        }
    }

private:
    struct Node {
        SharedPtr<T> val;
        UniquePtr<Node> next;
        mutable std::mutex m;
    };

    UniquePtr<Node> head;
};

