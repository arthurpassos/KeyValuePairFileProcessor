cmake_minimum_required(VERSION 3.15...3.23)

if (KVP_EXTRACTOR_ENABLE_TESTING)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
    FetchContent_MakeAvailable(json)

    enable_testing()

    add_executable(KeyValuePairExtractorTests KeyValuePairExtractorTest.cpp)

    target_link_libraries(KeyValuePairExtractorTests GTest::gtest_main KeyValuePairExtractorLib nlohmann_json::nlohmann_json)

    include(GoogleTest)
    gtest_discover_tests(KeyValuePairExtractorTests)
endif ()
