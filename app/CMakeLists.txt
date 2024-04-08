cmake_minimum_required(VERSION 3.15...3.23)

if (BUILD_APP)

    include(FetchContent)
    FetchContent_Declare(
            argparse
            GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    )
    FetchContent_MakeAvailable(argparse)

    add_executable(key_value_pair_extractor main.cpp)
    target_link_libraries(key_value_pair_extractor KeyValuePairExtractorLib argparse)
endif ()
