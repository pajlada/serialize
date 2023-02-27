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

    # For MSVC: Prevent overriding the parent project's compiler/linker settings
    # See https://github.com/google/googletest/blob/main/googletest/README.md#visual-studio-dynamic-vs-static-runtimes
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

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

    # Enable strict compiler settings
    if (MSVC)
        # Someone adds /W3 before we add /W4.
        # This makes sure, only /W4 is specified.
        string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

        target_compile_options(serialize-test PUBLIC
            /W4
            /WX # treat warnings as errors
        )
    else()
        target_compile_options(serialize-test PUBLIC -Wall -Werror)
    endif()
endif()
