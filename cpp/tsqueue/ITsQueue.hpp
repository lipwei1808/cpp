#pragma once

#include "SharedPtr.hpp"

template <typename T>
class ITsQueue {
public:
    virtual void push(T val) = 0;
    virtual bool waitAndPop(T& val) = 0;
    virtual SharedPtr<T> waitAndPop() = 0;
    virtual SharedPtr<T> tryPop() = 0;
    virtual bool tryPop(T& val) = 0;
    virtual ~ITsQueue() = default;
};
