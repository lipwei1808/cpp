add_executable(TsListTest TsListTest.cpp)

target_link_libraries(TsListTest PRIVATE TsListLib LoggerLib gtest_main)

include(GoogleTest)
gtest_discover_tests(TsListTest)
