#pragma once

#include "Logger.hpp"

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <type_traits>
#include <utility>

template <typename K, typename V>
class Hashmap {
private:
    using value_type = std::pair<const K, V>;
    using Bucket = std::list<value_type>;

public:
    template <bool IsConst> 
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional<IsConst, const value_type*, value_type*>::type;
        using reference = std::conditional<IsConst, const value_type&, value_type&>::type;
        Iterator(Bucket::iterator ptr, Bucket* bucketPtr, size_t currentBucket, size_t numOfBuckets):
                mPtr(ptr), mBucketPtr(bucketPtr), currentBucket(currentBucket), numOfBuckets(numOfBuckets) {}
        reference operator*() const {
            return *mPtr;
        }
        pointer operator->() { return &(*mPtr); }

        Bucket::iterator getBucketIterator() {
            return mPtr;
        }

        Iterator& operator++() {
            next();
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp{*this};
            next();
            return tmp;
        }

        friend bool operator==(const Iterator&a, const Iterator& b) { 
            if (a.numOfBuckets == a.currentBucket && b.numOfBuckets == b.currentBucket) {
                return true;
            }

            if (a.numOfBuckets == a.currentBucket || b.numOfBuckets == b.currentBucket) {
                return false;
            }

            return a.mPtr == b.mPtr && a.currentBucket == b.currentBucket;
        }

        friend bool operator!=(const Iterator&a, const Iterator& b) { 
            return !(a == b);
        }

    private:
        void next() {
            mPtr++;
            if (mPtr != mBucketPtr[currentBucket].end()) {
                return;
            }
            
            currentBucket++;
            while(currentBucket < numOfBuckets && mBucketPtr[currentBucket].empty()) {
                currentBucket++;
            }
            
            if (currentBucket == numOfBuckets) {
                return;
            }
            mPtr = mBucketPtr[currentBucket].begin();
        }

        Bucket::iterator mPtr;
        Bucket* mBucketPtr;
        size_t currentBucket;
        size_t numOfBuckets;

    };
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    Hashmap() = default;
    Hashmap(std::initializer_list<value_type> lst) {
        LOG_TRACE("Initializer list constructor");
        rehash(lst.size());
        for (const value_type& item: lst) {
            insert(item);
        }
    }
    Hashmap(const Hashmap& other): maxLoadFactor(other.maxLoadFactor) {
        rehash(other.mBucketCount);
        for (const auto& el: other) {
            LOG_TRACE("start: %d, end: %d", el.first, el.second);
            insert(el);
        }
    }

    Hashmap(Hashmap&& other):  mBucketCount(other.mBucketCount),
        mSize(other.mSize), maxLoadFactor(other.maxLoadFactor) {
        mStore = other.mStore;
        other.mStore = nullptr;
    }

    V& at(const K& key) {
        size_t hashIdx = std::hash<K>()(key);
        size_t bucketIdx = hashIdx % mBucketCount;
        for (auto& item: mStore[bucketIdx]) {
            if (item.first == key) {
                return item.second;
            }
        }
        throw std::out_of_range("invalid key");
    } 

    std::pair<Iterator<false>, bool> insert(const value_type& value) {
        LOG_TRACE("insert");
        if (mBucketCount == 0) {
            rehash(1);
        }
        size_t hashIdx = std::hash<K>()(value.first);
        size_t bucketIdx = hashIdx % mBucketCount;

        // Check if key already exists and replace
        typename Bucket::iterator start = mStore[bucketIdx].begin();
        for (; start != mStore[bucketIdx].end(); start++) {
            if (start->first == value.first) {
                LOG_TRACE("inserting key already present, overriding...");
                start->second = value.second;
                break;
            }
        }

        if (start != mStore[bucketIdx].end()) {
            return { Iterator<false>{start, mStore, bucketIdx, mSize }, false };
        }

        if (mSize / mBucketCount >= maxLoadFactor) {
            rehash(mBucketCount * 2);
            bucketIdx = hashIdx % mBucketCount;
        }
        start = mStore[bucketIdx].insert(mStore[bucketIdx].end(), value);
        mSize++;

        return { Iterator<false>{start, mStore, bucketIdx, mSize }, true };
    }

    V& operator[](const K& key) {
        size_t hashIdx = std::hash<K>()(key);
        size_t bucketIdx = hashIdx % mBucketCount;
        for (auto& item: mStore[bucketIdx]) {
            if (item.first == key) {
                return item.second;
            }
        }
        auto res = insert({key, V{}});
        return res.first->second;
    }

    // TODO: unit tests
    iterator find(const K& key) {
        size_t hashIdx = std::hash<K>()(key);
        size_t bucketIdx = hashIdx % mBucketCount;
        for (auto start = mStore[bucketIdx].begin(); start != mStore[bucketIdx].end(); start++) {
            if (start->first == key) {
                return Iterator<false>{start, mStore, bucketIdx, mBucketCount};
            }
        }
        return end();
    }

    // TODO: unit tests
    bool contains(const K& key) {
        return find(key) != end();
    }

    // TODO: unit tests
    size_t erase(const K& key) {
        iterator it = find(key);
        if (it == end()) {
            return 0;
        }
        size_t hashIdx = std::hash<K>()(key);
        size_t bucketIdx = hashIdx % mBucketCount;
        mStore[bucketIdx].erase(it.getBucketIterator());
        mSize--;
        return 1;
    }

    size_t size() const {
        return mSize;
    }

    size_t bucket_count() const {
        return mBucketCount;
    }

    void rehash(size_t count) {
        LOG_TRACE("rehash to %zu", count);
        Bucket* newStore = new Bucket[count];
        for (size_t i = 0; i < mBucketCount; i++) {
            for (const value_type& val: mStore[i]) {
                size_t hashIdx = std::hash<K>()(val.first);
                size_t bucketIdx = hashIdx % count;
                newStore[bucketIdx].insert(newStore[bucketIdx].end(), std::move(val));
            }
        }
        delete[] mStore;
        mStore = newStore;
        mBucketCount = count;
    }

    const_iterator cbegin() const {
        for (size_t i = 0; i < mBucketCount; i++) {
            if (!mStore[i].empty()) {
                return Iterator<true>{mStore[i].begin(), mStore, i, mBucketCount};
            }
        }
        return cend();
    }
    
    const_iterator cend() const {
        return Iterator<true>{{}, mStore, mBucketCount, mBucketCount};
    }

    iterator begin() {
        for (size_t i = 0; i < mBucketCount; i++) {
            if (!mStore[i].empty()) {
                return Iterator<false>{mStore[i].begin(), mStore, i, mBucketCount};
            }
        }
        return end();
    }
    
    const_iterator begin() const {
        return cbegin();
    }

    const_iterator end() const {
        return cend();
    }

    iterator end() {
        return Iterator<false>{{}, mStore, mBucketCount, mBucketCount};
    }

    ~Hashmap() {
        if (mStore) {
            delete[] mStore;
        }
    }

private:
    Bucket* mStore = nullptr;
    size_t mBucketCount = 0;
    size_t mSize = 0;
    size_t maxLoadFactor = 1;
};

