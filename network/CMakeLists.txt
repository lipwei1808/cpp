add_library(NetworkLib Network.cpp)

target_include_directories(NetworkLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NetworkLib PUBLIC LoggerLib StringLib)
