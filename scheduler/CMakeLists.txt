add_subdirectory(proto)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_library(Scheduler STATIC ${SOURCES} ${HEADERS})

target_include_directories(Scheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Scheduler PUBLIC CppLib MyProto NetworkLib)
