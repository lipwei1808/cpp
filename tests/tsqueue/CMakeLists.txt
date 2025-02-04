add_executable(TsQueueTest TsQueueTest.cpp)

target_link_libraries(TsQueueTest PRIVATE TsQueueLib LoggerLib gtest_main)

include(GoogleTest)
gtest_discover_tests(TsQueueTest)
