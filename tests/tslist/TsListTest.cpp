#include <gtest/gtest.h>

#include "TsList.hpp"

TEST(TsListTest, ForEachPrint) {
    TsList<int> q;

    for (int i = 0; i < 5; i++) {
        q.pushFront(i);
    }

    int i = 4;
    auto fn = [&i](int& val) {
        ASSERT_EQ(i, val);
        i--;
    };

    LOG_INFO("Start of foreach");
    q.forEach(fn);
    EXPECT_EQ(i, -1);
}

TEST(TsListTest, RemoveIf) {
    TsList<int> q;

    for (int i = 0; i < 10; i++) {
        q.pushFront(i);
    }

    q.removeIf([](int& val) {
        return val % 2 == 0;
    });

    int i = 9;
    q.forEach([&i](int& val) {
        EXPECT_EQ(val, i);
        i -= 2;
    });

    EXPECT_EQ(i, -1);
}
