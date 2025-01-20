find_package(Protobuf REQUIRED)

set(PROTO_FILES message.proto)

add_library(MyProto STATIC ${PROTO_FILES} )
target_include_directories(MyProto INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(MyProto PUBLIC protobuf::libprotobuf)

protobuf_generate(TARGET MyProto LANGUAGE cpp)
