#pragma once

#include <atomic>
#include "Logger.hpp"

// TODO: Is not thread safe currently
template <typename T>
class SharedPtr {
public:
    SharedPtr(T* ptr): ptr(ptr), refCount(new std::atomic<unsigned int>(1)) {
        LOG_INFO("Default Constructor: %p, %p", ptr, refCount);
    }
    SharedPtr(const SharedPtr& other): ptr(other.ptr), refCount(other.refCount) {
        LOG_DEBUG("Copy Constructor other ptr: %p, other refCount: %p", other.ptr, other.refCount);
        refCount->fetch_add(1, std::memory_order::relaxed);
    }
    SharedPtr(SharedPtr&& other): refCount(other.refCount) {
        LOG_DEBUG("Move Constructor other ptr: %p, other refCount: %p", other.ptr, other.refCount);
        ptr = other.ptr;
        other.ptr = nullptr;
    }


    unsigned int get_count() const {
        return refCount->load(std::memory_order::acquire);
    }

    T* get() const { return ptr; }
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }

    ~SharedPtr() {
        unsigned int oldCount = refCount->fetch_sub(1, std::memory_order_acq_rel);
        if (oldCount == 1) {
            if (ptr) 
                delete ptr;
            delete refCount;
        }
    }

private:
    T* ptr = nullptr;
    std::atomic<unsigned int>* refCount = nullptr;
};
