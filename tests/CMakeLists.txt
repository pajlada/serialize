cmake_minimum_required(VERSION 3.15...4.0)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY On) # For rapidjson

project(serialize-test)

# For MSVC: Prevent overriding the parent project's compiler/linker settings
# See https://github.com/google/googletest/blob/main/googletest/README.md#visual-studio-dynamic-vs-static-runtimes
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Ensure we don't install googletest
set(INSTALL_GTEST OFF CACHE INTERNAL "")

include(GoogleTest)
include(FetchContent)

option(PAJLADA_SERIALIZE_BUILD_COVERAGE "Build coverage" OFF)

FetchContent_Declare(
    RapidJSON
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../external/rapidjson
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS
)
set(RAPIDJSON_BUILD_EXAMPLES Off CACHE INTERNAL "")
set(RAPIDJSON_BUILD_TESTS Off CACHE INTERNAL "")

FetchContent_Declare(
    googletest
    URL ${CMAKE_CURRENT_LIST_DIR}/../external/googletest
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS NAMES GTest
)

FetchContent_MakeAvailable(RapidJSON googletest)

enable_testing()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/unsorted.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE Pajlada::Serialize)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)

if(TARGET rapidjson)
    message(STATUS "Linking to rapidjson target")
    target_link_libraries(${PROJECT_NAME} PRIVATE rapidjson)
elseif(DEFINED RapidJSON_SOURCE_DIR)
    message(STATUS "RapidJSON_SOURCE_DIR defined, assuming this is a submodule/source build. (${RapidJSON_SOURCE_DIR})")
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${RapidJSON_SOURCE_DIR}/include)
else()
    message(STATUS "No rapidjson target found, this is most likely a system install. Adding include directories (${RAPIDJSON_INCLUDE_DIRS}) instead")
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
endif()

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
