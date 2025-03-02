add_library(SharedPtrLib INTERFACE)

target_include_directories(SharedPtrLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

#target_compile_options(SharedPtrLib INTERFACE -fsanitize=thread)
#target_link_options(SharedPtrLib INTERFACE -fsanitize=thread)

target_link_libraries(SharedPtrLib INTERFACE LoggerLib)

