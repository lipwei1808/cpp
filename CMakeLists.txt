cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0126 NEW)

project(cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_compile_definitions(NDEBUG)

# Option to set the logging level. Default is ERROR_LEVEL.
option(LOG_LEVEL_OPTION "Set the log level (0=NO_LOG, 1=ERROR_LEVEL, 2=INFO_LEVEL, 3=TRACE_LEVEL)" 1)

# Define the LOG_LEVEL based on the option
if(LOG_LEVEL_OPTION EQUAL 0)
    message(STATUS "Disabling LOG")
    add_compile_definitions(LOG_LEVEL=0x0)
elseif(LOG_LEVEL_OPTION EQUAL 1)
    message(STATUS "Using default Log Level ERROR")
    add_compile_definitions(LOG_LEVEL=0x01)
elseif(LOG_LEVEL_OPTION EQUAL 2)
    message(STATUS "Using default Log Level INFO")
    add_compile_definitions(LOG_LEVEL=0x02)
elseif(LOG_LEVEL_OPTION EQUAL 3)
    message(STATUS "Using default Log Level TRACE")
    add_compile_definitions(LOG_LEVEL=0x03)
else()
    message(STATUS "Using default Log Level INFO")
endif()

# Google benchmark
#set(benchmark_DIR "~/opt/lib/benchmark/cmake/benchmark")
#list(APPEND CMAKE_PREFIX_PATH "~/opt/lib/benchmark")
list(APPEND CMAKE_PREFIX_PATH "/Users/tanlipwei/opt")
#list(APPEND CMAKE_PREFIX_PATH "/Users/tanlipwei/opt/protobuf/")
find_package(benchmark REQUIRED)
if (benchmark_FOUND)
    message(STATUS "Google benchmark found")
else()
    message(STATUS "Google benchmark not found")
endif()

# Google test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(cpp)
add_subdirectory(scheduler)
add_subdirectory(benchmark)
add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)

