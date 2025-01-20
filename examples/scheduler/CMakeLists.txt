add_executable(Master main.cpp)
add_executable(Worker worker.cpp)

target_link_libraries(Master Scheduler CppLib MyProto)
target_link_libraries(Worker Scheduler CppLib MyProto)
