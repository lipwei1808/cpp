add_library(SharedPtrLib INTERFACE)

target_include_directories(SharedPtrLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SharedPtrLib INTERFACE LoggerLib)
