#include <gtest/gtest.h>
#include <stdexcept>

#include "Hashmap.hpp"
#include "Logger.hpp"

TEST(HashmapTest, Constructor) {
    Hashmap<int, int> map;
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.bucket_count(), 0);

    Hashmap<int, int> map2{{3, 5}, {4, 6}};
    ASSERT_EQ(map2.size(), 2);
    ASSERT_EQ(map2.bucket_count(), 2);

    Hashmap<int, int> map3{{3, 5}, {3, 6}};
    ASSERT_EQ(map3.size(), 1);
    ASSERT_EQ(map3.bucket_count(), 2);
}

TEST(HashmapTest, CopyConstructor) {
    Hashmap<int, int> map{{3, 5}, {3, 6}};

    Hashmap<int, int> map2{map};
    ASSERT_EQ(map.size(), map2.size());
    ASSERT_EQ(map.bucket_count(), map2.bucket_count());
    for (const auto& el: map2) {
        ASSERT_EQ(map2.at(el.first), el.second);
    }
}

TEST(HashmapTest, MoveConstructor) {
    Hashmap<int, std::string> map{{1, "bye"}, {2, "hi"}};
    std::string longString{"long string here i am here and there everywher"};
    map.insert({3, longString});

    Hashmap<int, std::string> map2{std::move(map)};
    ASSERT_EQ(map2.size(), 3);
    ASSERT_EQ(map2.bucket_count(), 4);
    ASSERT_EQ(map2.at(1), "bye");
    LOG_TRACE("hi");
    ASSERT_EQ(map2.at(2), "hi");
    LOG_TRACE("hi2");
    ASSERT_EQ(map2.at(3), longString);
}

TEST(HashmapTest, Insert) {
    Hashmap<int, int> map;
    map.insert({3, 5});
    ASSERT_EQ(map.size(), 1);
    ASSERT_EQ(map.bucket_count(), 1);
    ASSERT_EQ(map.at(3), 5);

    for (int i = 0; i < 3; i++) {
        map.insert({i, i});
    }
    ASSERT_EQ(map.bucket_count(), 4);

    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(map.at(i), i);
    }

    for (int i = 0; i < 3; i++) {
        map.insert({i, i + 1});
    }

    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(map.at(i), i + 1);
    }

    auto res = map.insert({100, 1});
    ASSERT_EQ(map.size(), 5);
    ASSERT_EQ(map.bucket_count(), 8);
    ASSERT_EQ(res.second, true);
    ASSERT_EQ(res.first->first, 100);
    ASSERT_EQ(res.first->second, 1);
    ASSERT_EQ(map.at(100), 1);

    res = map.insert({100, 2});
    ASSERT_EQ(map.size(), 5);
    ASSERT_EQ(res.second, false);
    ASSERT_EQ(res.first->first, 100);
    ASSERT_EQ(res.first->second, 2);
    ASSERT_EQ(map.at(100), 2);
}

TEST(HashmapTest, Access) {
    Hashmap<int, std::string> map{{0, "hey"}, {1, "string"}};
    ASSERT_EQ(map[0], "hey");
    ASSERT_EQ(map[1], "string");
    ASSERT_EQ(map.at(0), "hey");
    ASSERT_EQ(map.at(1), "string");

    ASSERT_EQ(map[2], "");
    EXPECT_THROW(map.at(3), std::out_of_range);
}

TEST(HashmapTest, Rehash) {
    Hashmap<int, int> map;
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.bucket_count(), 0);

    map.rehash(0);
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.bucket_count(), 0);

    map.insert({3, 3});
    ASSERT_EQ(map.size(), 1);
    ASSERT_EQ(map.bucket_count(), 1);

    map.insert({3, 3});
    ASSERT_EQ(map.size(), 1);
    ASSERT_EQ(map.bucket_count(), 1);

    map.insert({4, 4});
    ASSERT_EQ(map.size(), 2);
    ASSERT_EQ(map.bucket_count(), 2);

    map.insert({5, 5});
    ASSERT_EQ(map.size(), 3);
    ASSERT_EQ(map.bucket_count(), 4);

    map.insert({6, 6});
    ASSERT_EQ(map.size(), 4);
    ASSERT_EQ(map.bucket_count(), 4);
     
    map.insert({7, 7});
    ASSERT_EQ(map.size(), 5);
    ASSERT_EQ(map.bucket_count(), 8);

    for (int i = 3; i <= 7; i++) {
        ASSERT_EQ(map.at(i), i);
    }
}

TEST(HashmapTest, Iterator) {
    Hashmap<int, int> map;
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(map.insert({i, i}).second);
    }

    int i = 0;
    for (const auto& el: map) {
        ASSERT_EQ(el.second, i++);
    }

    Hashmap<int, int>::iterator start = map.begin();
    for (int i = 0; i < 5; i++) {
        start++;
    }
    ASSERT_EQ(start, map.end());
}
