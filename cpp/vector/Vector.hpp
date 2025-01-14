#pragma once

#include "Logger.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <format>

template <std::destructible T>
class Vector {
public:
    Vector(): ptr(nullptr), m_capacity(0), idx(0) {}

    Vector(size_t size, const T& element): ptr(nullptr), m_capacity(0), idx(0) {
        LOG_TRACE("Constructor with size and element %zu", size);
        resize(size);
        for (int i = 0; i < size; i++) {
            new (&ptr[idx++]) T(element);
        }
    }

    Vector(std::initializer_list<T> lst): ptr(nullptr), m_capacity(0), idx(0) {
        LOG_TRACE("Initializer list %zu", lst.size());
        resize(lst.size());
        for (const auto& i: lst) {
            new (&ptr[idx++]) T(i);
        }
    }

    Vector(const Vector& vec): ptr(nullptr), m_capacity(0), idx(0) {
        LOG_TRACE("Copy constructor");
        resize(vec.m_capacity);
        for (int i = 0; i < vec.idx; i++) {
            new (&ptr[idx++]) T(vec.ptr[i]);
        }
    }

    Vector(Vector&& vec): ptr(vec.ptr), m_capacity(vec.m_capacity), idx(vec.idx) {
        LOG_TRACE("Move constructor");
        vec.ptr = nullptr;
    }

    Vector& operator=(const Vector& vec) {
        LOG_TRACE("Copy assignment operator");
        Vector v{vec};
        swap(v);
        return *this;
    }

    Vector& operator=(Vector&& vec) {
        LOG_TRACE("Move assignment operator");
        Vector v{std::move(vec)};
        swap(v);
        return *this;
    }

    void push_back(const T& val) {
        LOG_TRACE("push_back");
        if (idx == m_capacity) {
            resize(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        new (&ptr[idx++]) T(val);
    }

    void pop_back() {
        LOG_TRACE("pop_back");
        if (idx > 0) {
            idx--;
        }
    }

    void clear() {
        for (int i = 0; i < idx; i++) {
            ptr[i].~T();
        }
        idx = 0;
    }

    template <typename... U>
    T& emplace_back(U&&... args) {
        if (idx == m_capacity) {
            resize(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        new (&ptr[idx++]) T(std::forward<U>(args)...);
        return ptr[idx - 1];
    }

    void resize(size_t count) {
        if (count == m_capacity) {
            LOG_TRACE("resize count = capacity");
            return;
        }

        if (m_capacity > count) {
            LOG_TRACE("resize count < capacity");
            idx = std::min(count, idx);
            return;
        }

        LOG_TRACE("resize count (%zu) > capacity (%zu)", count , m_capacity);
        T* newPtr = reinterpret_cast<T*>(::operator new(sizeof(T) * count));
        for (size_t i = 0; i < idx; i++) {
            new (&newPtr[i]) T(std::move(ptr[i]));
        }
        LOG_TRACE("Calling destructors for old memory");
        for (size_t i = 0; i < idx; i++) {
            ptr[i].~T();
        }

        LOG_TRACE("Freeing old memory");
        ::operator delete(ptr);
        ptr = newPtr;
        m_capacity = count;
    }
    
    size_t size() const {
        return idx;
    }

    size_t capacity() const {
        return m_capacity;
    }

    T* data() {
        return ptr;
    }

    T& at(size_t pos) {
        if (pos >= idx) {
            throw std::out_of_range(std::format("pos %d is out of range", pos));
        }
        return ptr[pos];
    }

    T& operator[](size_t pos) {
        if (pos >= idx) {
            pos = 0;
        }

        return ptr[pos];
    }

    void swap(Vector& vec) {
        std::swap(ptr, vec.ptr);
        std::swap(m_capacity, vec.m_capacity);
        std::swap(idx, vec.idx);
    }
    
    ~Vector() {
        if (ptr) {
            ::operator delete(ptr);
        }
    }

private:
    T* ptr = nullptr;
    size_t m_capacity;
    size_t idx;
};

