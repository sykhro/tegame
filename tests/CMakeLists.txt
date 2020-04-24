add_executable(bitfield_test bitfield_t.cpp)
target_include_directories(bitfield_test PRIVATE . ../src/)
target_link_libraries(bitfield_test PRIVATE catch)

add_test(bitfield_test bitfield_test)
