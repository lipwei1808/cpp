#pragma once

#include "Logger.hpp"

// TODO: Write tests
template <typename T>
class UniquePtr {
public:
    UniquePtr(): ptr(nullptr) {
        LOG_DEBUG("Default constructor: %p", this);
    }
    UniquePtr(T* ptr): ptr(ptr) {
        LOG_DEBUG("Parameter constructor: %p, ptr: %p", this, ptr);
    }
    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr(UniquePtr&& other): ptr{other.ptr} {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) {
        LOG_DEBUG("operator= ptr=%p other.ptr=%p", ptr, other.ptr);
        if (this != &other) {
            reset(other.ptr);
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() {
        return *ptr;
    }
    T* operator->() {
        return ptr;
    }

    T* release() {
        ptr = nullptr;
        return ptr;
    }

    void reset(T* newPtr) {
        delete ptr;
        ptr = newPtr;
    }

    void swap(UniquePtr<T>& other) noexcept {
        T* otherPtr = other.ptr;
        other.ptr = ptr;
        ptr = otherPtr;
    }

    T* get() {
        return ptr;
    }

    ~UniquePtr() {
        delete ptr;
    }

private:
    T* ptr = nullptr;;
};
