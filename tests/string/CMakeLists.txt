add_executable(StringTest StringTest.cpp)

target_link_libraries(StringTest PRIVATE StringLib gtest_main)

include(GoogleTest)
gtest_discover_tests(StringTest)
