add_executable(SharedPtrTest SharedPtrTest.cpp)

target_link_libraries(SharedPtrTest PRIVATE SharedPtrLib LoggerLib gtest_main)

include(GoogleTest)
gtest_discover_tests(SharedPtrTest)
