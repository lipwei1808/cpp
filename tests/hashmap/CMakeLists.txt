add_executable(HashmapTest HashmapTest.cpp)

target_link_libraries(HashmapTest PRIVATE HashmapLib LoggerLib gtest_main)

include(GoogleTest)
gtest_discover_tests(HashmapTest)
