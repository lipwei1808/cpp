add_executable(HashmapBenchmark HashmapBenchmark.cpp)

target_link_libraries(HashmapBenchmark HashmapLib benchmark::benchmark)
