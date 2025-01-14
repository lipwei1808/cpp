add_executable(StringBenchmark StringBenchmark.cpp)

target_link_libraries(StringBenchmark StringLib benchmark::benchmark)

