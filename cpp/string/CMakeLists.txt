add_library(StringLib String.cpp)

target_link_libraries(StringLib LoggerLib)

target_include_directories(StringLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
