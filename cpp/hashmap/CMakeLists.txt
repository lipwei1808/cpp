add_library(HashmapLib INTERFACE)

target_include_directories(HashmapLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(HashmapLib INTERFACE LoggerLib)
