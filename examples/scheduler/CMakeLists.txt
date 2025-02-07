add_executable(Master main.cpp)
add_executable(Worker worker.cpp)
add_executable(Client client.cpp)

target_link_libraries(Master Scheduler CppLib MyProto)
target_link_libraries(Worker Scheduler CppLib MyProto NetworkLib)
target_link_libraries(Client NetworkLib)
