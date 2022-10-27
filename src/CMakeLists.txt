cmake_minimum_required(VERSION 3.15...3.23)

add_library(key_value_pair_processing_lib KeyValuePairFileProcessor.cpp)
add_executable(key_value_pair_processing main.cpp KeyValuePairFileProcessor.cpp)