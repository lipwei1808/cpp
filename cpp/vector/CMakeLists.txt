add_library(VectorLib INTERFACE)

target_include_directories(VectorLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(VectorLib INTERFACE LoggerLib)
