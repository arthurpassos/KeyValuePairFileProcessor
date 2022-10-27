cmake_minimum_required(VERSION 3.23)
project(key_value_pair_processing)

set(CMAKE_CXX_STANDARD 23)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)