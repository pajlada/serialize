cmake_minimum_required(VERSION 3.15...4.0)

project(serialize-test)

# For MSVC: Prevent overriding the parent project's compiler/linker settings
# See https://github.com/google/googletest/blob/main/googletest/README.md#visual-studio-dynamic-vs-static-runtimes
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)
include(FetchContent)

option(PAJLADA_SERIALIZE_BUILD_COVERAGE "Build coverage" OFF)

FetchContent_Declare(
    googletest
    URL ${CMAKE_CURRENT_LIST_DIR}/../external/googletest
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS NAMES GTest
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/unsorted.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE Pajlada::Serialize)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)

gtest_discover_tests(${PROJECT_NAME})

if(PAJLADA_SERIALIZE_BUILD_COVERAGE)
    list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_LIST_DIR}/cmake"
    )
    include(CodeCoverage)

    append_coverage_compiler_flags()

    setup_target_for_coverage_gcovr_html(
        NAME coverage
        EXECUTABLE ${PROJECT_NAME}
        BASE_DIRECTORY "../"
        EXCLUDE "external/*" "${CMAKE_SOURCE_DIR}/tests/src/*"
        )
endif()

# # Enable strict compiler settings
# if (MSVC)
#     # Someone adds /W3 before we add /W4.
#     # This makes sure, only /W4 is specified.
#     string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# 
#     target_compile_options(${PROJECT_NAME} PUBLIC
#         /W4
#         /WX # treat warnings as errors
#     )
# else()
#     target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Werror)
# endif()
