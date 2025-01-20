add_executable(MasterTest MasterTest.cpp)

target_link_libraries(MasterTest CppLib Scheduler gtest_main ${Protobuf_LIBRARIES})

include(GoogleTest)
gtest_discover_tests(MasterTest)
