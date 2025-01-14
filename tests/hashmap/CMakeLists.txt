add_executable(HashmapTest HashmapTest.cpp)

target_link_libraries(HashmapTest PRIVATE HashmapLib gtest_main)

include(GoogleTest)
gtest_discover_tests(HashmapTest)
