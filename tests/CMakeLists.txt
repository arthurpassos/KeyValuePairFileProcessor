cmake_minimum_required(VERSION 3.15...3.23)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(key_value_pair_processing_tests_fsm InlineEscapingKeyValuePairExtractorTests.cpp LazyEscapingKeyValuePairExtractor.cpp KeyValuePairExtractorTest.h KeyValuePairExtractorTest.cpp)

target_link_libraries(key_value_pair_processing_tests_fsm GTest::gtest_main key_value_pair_processing_lib nlohmann_json::nlohmann_json)

include(GoogleTest)
gtest_discover_tests(key_value_pair_processing_tests_fsm)