#include <gtest/gtest.h>

#include "TsQueue.hpp"

TEST(TsQueueTest, Constructor) {
    TsQueue<int> q;
}

TEST(TsQueueTest, Push) {
    TsQueue<int> q;
    ASSERT_EQ(q.size(), 0);
    q.push(1);
    q.push(2);
    ASSERT_EQ(q.size(), 2);
}

TEST(TsQueueTest, ConsumeSync) {
    TsQueue<int> q;
    for (int i = 0; i < 5; i++) {
        q.push(i);
    }

    for (int i = 0; i < 5; i++) {
        int res;
        q.consumeSync(res);
        ASSERT_EQ(res, i);
    }
}
