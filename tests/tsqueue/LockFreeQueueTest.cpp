#include <gtest/gtest.h>

#include "LockFreeQueue.hpp"

TEST(LockFreeQueueTest, Basic) {
    LockFreeQueue q;
    q.push(1);
}
