#pragma once

template <typename T>
class UniquePtr {
public:
    UniquePtr(): ptr(nullptr) {}
    UniquePtr(T* ptr): ptr(ptr) {}
    UniquePtr(const UniquePtr& ptr) = delete;
    UniquePtr(UniquePtr&& ptr) = delete;
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
        if (ptr) {
            delete ptr;
        }
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
        if (ptr) {
            delete ptr;
        }
    }

private:
    T* ptr = nullptr;;
};
