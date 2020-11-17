cmake_minimum_required(VERSION 3.8)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/.cmake"
    )

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(serialize)

include_directories(include)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
# set_property(TARGET serialize-test PROPERTY CXX_STANDARD 17)
# set_property(TARGET serialize-test PROPERTY CXX_STANDARD_REQUIRED ON)

add_library(PajladaSerialize INTERFACE)

target_include_directories(PajladaSerialize INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(PajladaSerialize INTERFACE RapidJSON::RapidJSON)

find_package(RapidJSON REQUIRED MODULE)

if (BUILD_TESTS)
    message("++ Tests enabled")

    add_subdirectory(external/googletest)

    enable_testing()
    include(GoogleTest)
    # find_package(GTest REQUIRED PATHS ${CMAKE_CURRENT_LIST_DIR}/external/googletest/googletest)

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    add_executable(serialize-test
        src/main.cpp
        )

    add_definitions(-DPAJLADA_TESTING)

    target_link_libraries(serialize-test PajladaSerialize gtest gtest_main)

    gtest_discover_tests(serialize-test)

    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.travis/cmake)

    if (BUILD_COVERAGE)
        message("++ Coverage enabled")
        include(CodeCoverage)

        setup_target_for_coverage(serialize-test-coverage serialize-test coverage)

        SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
        SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
    endif()
endif()
