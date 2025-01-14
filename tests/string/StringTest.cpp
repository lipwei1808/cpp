#include <gtest/gtest.h>

#include "String.hpp"

#include <algorithm>
#include <string>

TEST(StringTest, Constructor) {
    const char arr[] = "Hello World";
    std::string expected{arr};
    
    // Default constructor
    String s0;
    ASSERT_TRUE(std::equal(s0.c_str(), s0.c_str() + s0.size(), ""));
    
    // Param constructor 
    String s1{arr};
    ASSERT_TRUE(std::equal(s1.c_str(), s1.c_str() + s1.size(), expected.c_str()));

    String s1_2{nullptr};
    ASSERT_TRUE(s1_2.c_str() == nullptr);
    ASSERT_TRUE(s1_2.size() == 0);

    // Param constructor with len
    String s2{arr, 5};
    ASSERT_TRUE(std::equal(expected.c_str(), expected.c_str() + 5, s2.c_str()));

    String s2_2{nullptr, 3};
    ASSERT_TRUE(s2_2.c_str() == nullptr);
    ASSERT_TRUE(s2_2.size() == 0);

    // Copy constructor
    String s3{s1};
    ASSERT_TRUE(std::equal(s3.c_str(), s3.c_str() + s3.size(), expected.c_str()));

    // Move constructor
    String s4{std::move(s1)};
    ASSERT_TRUE(std::equal(s4.c_str(), s4.c_str() + s4.size(), expected.c_str()));
}

TEST(StringTest, AssignmentOperators) {
    const char arr[] = "Hello World";
    std::string expected{arr};
    String s1{arr};

    String s2;
    s2 = s1;
    ASSERT_TRUE(std::equal(s2.c_str(), s2.c_str() + s2.size(), expected.c_str()));
    s2 = s2;
    ASSERT_TRUE(std::equal(s2.c_str(), s2.c_str() + s2.size(), expected.c_str()));

    String s3;
    s3 = std::move(s1);
    s3 = std::move(s3);
    ASSERT_TRUE(std::equal(s3.c_str(), s3.c_str() + s3.size(), expected.c_str()));

}

TEST(StringTest, Append) {
    const char arr[] = "Hello world";
    
    // Append
    String s1{arr};
    s1.append("");
    ASSERT_TRUE(s1 == arr);

    s1.append("Sx");
    ASSERT_TRUE(s1 == "Hello worldSx");
    
    String s2{", new man!"};
    s1.append(s2);
    ASSERT_TRUE(s1 == "Hello worldSx, new man!");

    s2.append(s2);
    ASSERT_TRUE(s2 == ", new man!, new man!");

    s2 = "reset";
    String s3;
    s2.append(s3);
    ASSERT_TRUE(s2 == "reset");

    // Operator+=
    s1 = arr;
    s1 += "";
    ASSERT_TRUE(s1 == arr);

    s1 += "Sx";
    ASSERT_TRUE(s1 == "Hello worldSx");
    
    s2 = ", new man!";
    s1 += s2;
    ASSERT_TRUE(s1 == "Hello worldSx, new man!");

    s2 += s2;
    ASSERT_TRUE(s2 == ", new man!, new man!");

    s2 = "reset";
    s3 = "";
    s2 += s3;
    ASSERT_TRUE(s2 == "reset");
}

TEST(StringTest, Equality) {
    String s1;
    String s2;
    ASSERT_EQ(s1, s2);

    s1.append("hey");
    ASSERT_NE(s1, s2);

    String s3{"hey"};
    ASSERT_EQ(s1, s3);

    s2 += "hey";
    ASSERT_EQ(s1, s2);

    s1.clear();
    s2.clear(); 
    ASSERT_EQ(s1, s2);

    // Test non sso strings
    const char largeString[200] = {0};
    String s4{largeString};
    String s5{largeString};
    ASSERT_EQ(s4, s5);
}

TEST(StringTest, SubscriptOperator) {
    String s1{"Hello world"};
    ASSERT_EQ(s1[0], 'H');
    ASSERT_EQ(s1[4], 'o');
    ASSERT_EQ(s1[5], ' ');
    ASSERT_EQ(s1[11], 'H'); // Should be UD but i return 0th index

    s1.append("hey");
    ASSERT_EQ(s1[11], 'h');
}

TEST(StringTest, Iterator) {
    const char arr[] = "hello world";
    String s{arr};
    ASSERT_STREQ(s.c_str(), arr);

    int count = 0;
    for (auto& i: s) {
        ASSERT_EQ(i, arr[count++]);
    }
    
    std::reverse(s.begin(), s.end());
    for (auto& i: s) {
        ASSERT_EQ(i, arr[--count]);
    }

    s.clear();
    ASSERT_EQ(s.begin(), s.end());
}
