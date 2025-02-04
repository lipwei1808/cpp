add_subdirectory(string)
add_subdirectory(hashmap)
add_subdirectory(vector)
add_subdirectory(logger)
add_subdirectory(sharedptr)
add_subdirectory(uniqueptr)
add_subdirectory(tsqueue)

add_library(CppLib INTERFACE)
target_link_libraries(CppLib INTERFACE 
    StringLib 
    HashmapLib
    VectorLib
    LoggerLib
    SharedPtrLib
    UniquePtrLib
    TsQueueLib
)
