add_library(TsQueueLib INTERFACE)

target_include_directories(TsQueueLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TsQueueLib INTERFACE LoggerLib SharedPtrLib UniquePtrLib)
