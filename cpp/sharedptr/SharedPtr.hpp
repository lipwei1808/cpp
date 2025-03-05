#pragma once

#include <atomic>
#include "Logger.hpp"

template <typename T>
class SharedPtr {
public:
    SharedPtr(): ptr(nullptr), refCount(new std::atomic<unsigned int>(1)) {
        LOG_DEBUG("Default Constructor (%p). refCount: %p", this, refCount);
    }
    SharedPtr(T* ptr): ptr(ptr), refCount(new std::atomic<unsigned int>(1)) {
        LOG_DEBUG("Parameter pointer Constructor (%p): %p, %p", this, ptr, refCount);
    }
    SharedPtr(const SharedPtr& other): ptr(other.ptr), refCount(other.refCount) {
        LOG_DEBUG("Copy Constructor (%p) other ptr: %p, other refCount: %p", this, other.ptr, other.refCount);
        refCount->fetch_add(1, std::memory_order::relaxed);
    }
    SharedPtr(SharedPtr&& other): ptr(other.ptr), refCount(other.refCount) {
        LOG_DEBUG("Move Constructor (%p) other ptr: %p, other refCount: %p", this, other.ptr, other.refCount);
        other.ptr = nullptr;
        other.refCount = nullptr;
    }

    SharedPtr<T>& operator=(const SharedPtr& other) {
        LOG_DEBUG("Copy assignment operator");
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount; 
            if (refCount) {
                refCount->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    SharedPtr<T>& operator=(SharedPtr&& other) {
        LOG_DEBUG("Move assignment operator");
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount; 
            other.ptr = nullptr;
            other.refCount = nullptr;
        }
        return *this;
    }

    unsigned int get_count() const {
        return refCount->load(std::memory_order::acquire);
    }

    T* get() const { return ptr; }
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }

    ~SharedPtr() {
        release();
    }

private:
    void release() {
        if (!refCount) return;
        unsigned int oldCount = refCount->fetch_sub(1, std::memory_order_acq_rel);
        if (oldCount == 1) {
            delete ptr;
            delete refCount;
        }
    }
    T* ptr = nullptr;
    std::atomic<unsigned int>* refCount = nullptr;
};
