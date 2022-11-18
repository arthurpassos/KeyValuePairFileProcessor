cmake_minimum_required(VERSION 3.15...3.23)
project(KeyValuePairExtractor)

set(CMAKE_CXX_STANDARD 23)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)