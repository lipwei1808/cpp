#include "Vector.hpp"
#include "Logger.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

TEST(VectorTest, Constructor) {
    Vector<int> v1;
    ASSERT_EQ(v1.size(), 0);

    Vector<int> v2{0, 1, 2, 3, 4};
    Vector<int> v3 = {0, 1, 2, 3, 4};
    Vector<int> v4{{0, 1, 2, 3, 4}};
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(v2[i], i);
        ASSERT_EQ(v3[i], i);
        ASSERT_EQ(v4[i], i);
    }
    
    Vector<int> v5(18, 99);
    ASSERT_EQ(v5.size(), 18);
    for (int i = 0; i < v5.size(); i++) {
        ASSERT_EQ(v5[i], 99);
    }
}

TEST(VectorTest, Copy) {
    Vector<int> v({3, 5, 8});
    Vector<int> copy = v;
    Vector<int> copy1{v};
    for (int i = 0; i < v.size(); i++) {
        ASSERT_EQ(v[i], copy[i]);
        ASSERT_EQ(v[i], copy1[i]);
    }
}

TEST(VectorTest, Move) {
    Vector<int> v({3, 5, 8});
    Vector<int> move = std::move(v);
    ASSERT_EQ(v.data(), nullptr);
    ASSERT_EQ(move[0], 3);
    ASSERT_EQ(move[1], 5);
    ASSERT_EQ(move[2], 8);

    v = {3, 5, 8};
    ASSERT_NE(v.data(), nullptr);
    Vector<int> move1{std::move(v)};
    ASSERT_EQ(v.data(), nullptr);
    ASSERT_EQ(move1[0], 3);
    ASSERT_EQ(move1[1], 5);
    ASSERT_EQ(move1[2], 8);
}

TEST(VectorTest, Pushback) {
    Vector<int> v;
    v.push_back(1);
    ASSERT_EQ(v[0], 1);

    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    for (int i = 0; i < v.size(); i++) {
        ASSERT_EQ(v[i], i + 1);
    }
}

TEST(VectorTest, Popback) {
    Vector<char> v;
    size_t cnt = 10;
    for (int i = 0; i < cnt; i++) {
        v.push_back('a' + i);
    }
    
    size_t sz = v.size();
    for (int i = 0; i < cnt; i++) {
        ASSERT_EQ(v[v.size() - 1], 'a' + cnt - 1 - i);
        v.pop_back();
    }
}

TEST(VectorTest, ElementAccess) {
    Vector<char> v;
    EXPECT_NO_THROW(v[0]);
    EXPECT_THROW(v.at(0), std::out_of_range);
    EXPECT_THROW(v.at(1), std::out_of_range);

    size_t cnt = 10;
    for (int i = 0; i < cnt; i++) {
        v.push_back('a' + i);
    }

    for (int i = 0; i < cnt; i++) {
        ASSERT_EQ(v.at(i), 'a' + i);
    }

    char* ptr = v.data();
    for (int i = 0; i < v.size(); i++) {
        ASSERT_EQ(ptr[i], 'a' + i);
    }
}

TEST(VectorTest, Clear) {
    Vector<int> v;
    for (int i = 0; i < 10; i++) {
        v.push_back(19);
    }
    ASSERT_EQ(v.size(), 10);
    ASSERT_EQ(v.capacity(), 16);

    v.clear();   
    ASSERT_EQ(v.size(), 0);
    ASSERT_EQ(v.capacity(), 16);
}

TEST(VectorTest, EmplaceBack) {
    Vector<int> vec;
    int res = vec.emplace_back(19);
    ASSERT_EQ(res, 19);
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec[0], 19);

    res = vec.emplace_back(38);
    ASSERT_EQ(res, 38);
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0], 19);
    ASSERT_EQ(vec[1], 38);

    // Test resizing with emplace back
    for (int i = 0; i < 10; i++) {
        vec.emplace_back(38);
    }
    ASSERT_EQ(vec.size(), 12);
    ASSERT_EQ(vec.capacity(), 16);
}

TEST(VectorTest, EmplaceBackStruct) {
    struct Foo {
        int x;
        std::string y;
        Foo() {
            LOG_TRACE("Foo Default Constructor");
        }
        Foo(int x, std::string y): x(x), y(y) {
            LOG_TRACE("Foo Parameter Constructor %d, %s", x, y.c_str());
        }
        Foo(const Foo& foo): x(foo.x), y(foo.y) {
            LOG_TRACE("Foo Copy Constructor %d, %s", foo.x, foo.y.c_str());
        }
        Foo(Foo&& foo): x(std::move(foo.x)), y(std::move(foo.y)) {
            LOG_TRACE("Foo Move Constructor %d, %s", x, y.c_str());
        }
    };
    //Vector<Foo> v;
    std::vector<Foo> v;
    Foo& res = v.emplace_back(3, "hello");
    ASSERT_EQ(v.size(), 1);
    ASSERT_EQ(v[0].x, 3);
    ASSERT_EQ(v[0].y, "hello");
    ASSERT_EQ(&v[0], &res);
    ASSERT_EQ(v[0].x, res.x);
    ASSERT_EQ(v[0].y, res.y);

    v.emplace_back(6, "helloWorld");
    ASSERT_EQ(v.size(), 2);
    ASSERT_EQ(v[1].x, 6);
    ASSERT_EQ(v[1].y, "helloWorld");

    Foo f{3, "hell"};
    v.emplace_back(std::move(f));
    ASSERT_EQ(v.size(), 3);
    ASSERT_EQ(v[2].x, 3);
    ASSERT_EQ(v[2].y, "hell");
    EXPECT_TRUE(f.y.empty()); // Not guaranteed but we will take it

    Foo g{4, "hey"};
    v.emplace_back(g);
    ASSERT_EQ(v.size(), 4);
    ASSERT_EQ(v[3].x, 4);
    ASSERT_EQ(v[3].y, "hey");
    EXPECT_FALSE(g.y.empty()); 

    v.emplace_back(Foo{5, "he"});
    ASSERT_EQ(v.size(), 5);
    ASSERT_EQ(v[4].x, 5);
    ASSERT_EQ(v[4].y, "he");

    // Test that if move not available, it shall copy
    struct Bar {
        std::string x;
        Bar() = default;
        Bar(std::string x): x(std::move(x)) {}
        Bar(const Bar& b): x(b.x) {}
    };
    Vector<Bar> b;
    Bar b1{"hi"};
    b.emplace_back(std::move(b1));
    ASSERT_EQ(b.size(), 1);
    ASSERT_EQ(b[0].x, "hi");
    EXPECT_FALSE(b1.x.empty());
}

TEST(VectorTest, SizeAndCapacity) {
    Vector<int> v;
    ASSERT_EQ(v.size(), 0);
    ASSERT_EQ(v.capacity(), 0);

    v.push_back(1);
    ASSERT_EQ(v.size(), 1);
    ASSERT_EQ(v.capacity(), 1);

    v.push_back(1);
    ASSERT_EQ(v.size(), 2);
    ASSERT_EQ(v.capacity(), 2);

    v.push_back(1);
    ASSERT_EQ(v.size(), 3);
    ASSERT_EQ(v.capacity(), 4);

    v.push_back(1);
    ASSERT_EQ(v.size(), 4);
    ASSERT_EQ(v.capacity(), 4);
}

TEST(VectorTest, Resize) {
    Vector<int> v;
    for (int i = 0; i < 5; i++) {
        v.push_back(1);
    }
    ASSERT_EQ(v.size(), 5);
    ASSERT_EQ(v.capacity(), 8);

    v.resize(8);
    ASSERT_EQ(v.size(), 5);
    ASSERT_EQ(v.capacity(), 8);

    v.resize(4);
    ASSERT_EQ(v.size(), 4);
    ASSERT_EQ(v.capacity(), 8);

    v.resize(40);
    ASSERT_EQ(v.size(), 4);
    ASSERT_EQ(v.capacity(), 40);
    
    for (int i = 0; i < 40; i++) {
        v.push_back(10);
    }
    ASSERT_EQ(v.size(), 44);
    ASSERT_EQ(v.capacity(), 80);
}

TEST(VectorTest, Structs) {
    struct Foo {
        int a;
        std::unique_ptr<int> ptr;
        Foo(int a, std::unique_ptr<int> ptr): a(a), ptr(std::move(ptr)) {
            LOG_TRACE("Foo constructor");
        }
        Foo(const Foo& foo): a(foo.a), ptr(std::make_unique<int>(*foo.ptr)) {
            LOG_TRACE("Foo copy constructor");
        }
        Foo& operator=(const Foo& foo) {
            Foo f(foo);
            return *this;
        }
    };

    Vector<Foo> v;
    v.push_back(Foo{3, std::make_unique<int>(3)});
    ASSERT_EQ(v.size(), 1);
    Foo f1 = v[0];
    for (int i = 0; i < 5; i++) {
        LOG_TRACE("Loop %d", i);
        v.push_back(Foo{3, std::make_unique<int>(3)});
    }

    ASSERT_EQ(v.size(), 6);
    ASSERT_EQ(v.capacity(), 8);
}

