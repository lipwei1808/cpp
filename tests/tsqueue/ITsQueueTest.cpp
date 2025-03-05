#include <gtest/gtest.h>
#include <thread>

#include "TsQueue.hpp"
#include "FineGrainedTsQueue.hpp"

template <typename T>
class ITsQueueTest: public testing::Test {
public:
    T queueImpl;
};

using IntQueueImpls = ::testing::Types<TsQueue<int>, FineGrainedTsQueue<int>>;
TYPED_TEST_SUITE(ITsQueueTest, IntQueueImpls);

TYPED_TEST(ITsQueueTest, Basic) {
    this->queueImpl.push(1);
    SharedPtr<int> res = this->queueImpl.waitAndPop();
    ASSERT_EQ(*res, 1);
    SharedPtr<int> res2 = this->queueImpl.tryPop();
    EXPECT_TRUE(res2.get() == nullptr);

    this->queueImpl.push(2);
    res = this->queueImpl.tryPop();
    EXPECT_TRUE(*res == 2);
}

TYPED_TEST(ITsQueueTest, SingleThread) {
    int times = 100000;
    for (int i = 1; i <= times * 4; i++) {
        this->queueImpl.push(i);
    }

    long long sum{};
    for (int i = 0; i < times; i++) {
        int val;
        ASSERT_TRUE(this->queueImpl.tryPop(val));
        sum += val;
    }
    long long expected = (times + 1LL) * (times / 2LL);
    ASSERT_EQ(sum, expected);

    long long sum1{};
    for (int i = 0; i < times; i++) {
        SharedPtr<int> res = this->queueImpl.tryPop();
        ASSERT_TRUE(res.get() != nullptr);
        sum1 += *res;
    }

    expected = (times + 1LL + times * 2LL) * (times / 2LL);
    ASSERT_EQ(sum1, expected);

    long long sum2{};
    for (int i = 0; i < times; i++) {
        int val;
        this->queueImpl.waitAndPop(val);
        sum2 += val;
    }
    expected = (times * 2LL + 1LL + times * 3LL) * (times / 2LL);
    ASSERT_EQ(sum2, expected);

    long long sum3{};
    for (int i = 0; i < times; i++) {
        SharedPtr<int> res = this->queueImpl.waitAndPop();
        ASSERT_TRUE(res.get() != nullptr);
        sum3 += *res;
    }
    expected = (times * 3LL + 1LL + times * 4LL) * (times / 2LL);
    ASSERT_EQ(sum3, expected);

    expected = (1LL + times * 4LL) * (times * 4LL / 2LL);
    ASSERT_EQ(sum1 + sum2 + sum3 + sum, expected);
}

template <typename Queue>
void multiThreadProducerConsumer(
        Queue& q,
        std::function<void(long long& sum,
                           int consumerAmt,
                           Queue& q)> consumerFn) {
    int producerAmt = 150000;
    int consumerAmt = 100000;
    auto producerFn = [&]() {
        for (int i = 1; i <= producerAmt; i++) {
            q.push(i);
        }
    };

    std::thread p1{producerFn};
    std::thread p2{producerFn};

    long long s1 = 0;
    long long s2 = 0;
    long long s3 = 0;
    std::thread c1{consumerFn, std::ref(s1), consumerAmt, std::ref(q)};
    std::thread c2{consumerFn, std::ref(s2), consumerAmt, std::ref(q)};
    std::thread c3{consumerFn, std::ref(s3), consumerAmt, std::ref(q)};

    p1.join();
    p2.join();
    c1.join();
    c2.join();
    c3.join();

    long long correctSum = (1LL + producerAmt) * (producerAmt / 2LL);
    ASSERT_EQ(s1 + s2 + s3, 2 * correctSum);

}

TYPED_TEST(ITsQueueTest, MultithreadWaitAndPopCopy) {
    auto consumerFn = [&](long long& sum,
                          int consumerAmt,
                          TypeParam& q) {
        for (int i = 0; i < consumerAmt; i++) {
            int val;
            this->queueImpl.waitAndPop(val);
            sum += val;
        }
    };
    multiThreadProducerConsumer<TypeParam>(this->queueImpl, consumerFn);
}


TYPED_TEST(ITsQueueTest, MultithreadWaitAndPopPointer) {
    auto consumerFn = [&](long long& sum,
                          int consumerAmt,
                          TypeParam& q) {
        for (int i = 0; i < consumerAmt; i++) {
            SharedPtr<int> val = this->queueImpl.waitAndPop();
            sum += *val;
        }
    };
    multiThreadProducerConsumer<TypeParam>(this->queueImpl, consumerFn);
}

TYPED_TEST(ITsQueueTest, MultithreadTryPopCopy) {
    auto consumerFn = [&](long long& sum,
                          int consumerAmt,
                          TypeParam& q) {
        for (int i = 0; i < consumerAmt; i++) {
            int val;
            while (!this->queueImpl.tryPop(val)) {}
            sum += val;
        }
    };
    multiThreadProducerConsumer<TypeParam>(this->queueImpl, consumerFn);
}

TYPED_TEST(ITsQueueTest, MultithreadTryPopPointer) {
    auto consumerFn = [&](long long& sum,
                          int consumerAmt,
                          TypeParam& q) {
        for (int i = 0; i < consumerAmt; i++) {
            while (true) {
                SharedPtr<int> res = this->queueImpl.tryPop();
                if (res.get() != nullptr) {
                    sum += *res;
                    break;
                }
            }
        }
    };
    multiThreadProducerConsumer<TypeParam>(this->queueImpl, consumerFn);
}

struct Foo {
    int x;
    long long y;
    char z;
};

template <typename T>
class ITsQueueCustomStructTest: public testing::Test {
public:
    T queueImpl;
};

using CustomStructQueueImpls = ::testing::Types<TsQueue<Foo>, FineGrainedTsQueue<Foo>>;
TYPED_TEST_SUITE(ITsQueueCustomStructTest, CustomStructQueueImpls);

TYPED_TEST(ITsQueueCustomStructTest, CustomStruct) {
    for (int i = 0; i < 10; i++) {
        char c = i + '0';
        Foo f{i, i, c};
        this->queueImpl.push(f);
    }
    for (int i = 0; i < 10; i++) {
        Foo res;
        this->queueImpl.tryPop(res);

        ASSERT_EQ(res.x, i);
        ASSERT_EQ(res.y, i);
        ASSERT_EQ(res.z, i + '0');
    }
}

