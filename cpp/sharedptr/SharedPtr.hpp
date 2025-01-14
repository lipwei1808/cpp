#pragma once

template <typename T>
class SharedPtr {
public:
    SharedPtr(): ptr(nullptr), refCount(new unsigned int(0)) {}
    SharedPtr(T* ptr): ptr(ptr) {
        if (ptr) {
            refCount = new unsigned int(1);
        }
    }
    SharedPtr(const SharedPtr& other): ptr(other.ptr), refCount(other.refCount) {
        if (ptr) {
            (*refCount)++;
        }
    }
    SharedPtr(SharedPtr&& other) {
        ptr = other.ptr;
        refCount = other.refCount;
        other.ptr = nullptr;
        other.refCount = nullptr;
    }

    T& operator*() {
        return *ptr;
    }

    T* operator->() {
        return ptr;
    }

    unsigned int get_count() const {
        return *refCount;
    }

    T* get() const {
        return ptr;
    }

    ~SharedPtr() {
        cleanup();
    }

private:
    void cleanup() {
        if (*refCount == 1) {
            if (ptr) {
                delete ptr;
            }
            delete refCount;
        } else {
            (*refCount)--;
        }
    }

    T* ptr = nullptr;
    unsigned int* refCount = nullptr;
};
