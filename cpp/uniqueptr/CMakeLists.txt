add_library(UniquePtrLib INTERFACE)

target_include_directories(UniquePtrLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UniquePtrLib INTERFACE LoggerLib)
