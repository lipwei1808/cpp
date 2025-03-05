add_library(TsListLib INTERFACE)

target_include_directories(TsListLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TsListLib INTERFACE LoggerLib UniquePtrLib SharedPtrLib)
