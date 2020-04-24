cmake_minimum_required(VERSION 3.5)

project(tegame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/catch EXCLUDE_FROM_ALL)
add_subdirectory(external/fmt EXCLUDE_FROM_ALL)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
