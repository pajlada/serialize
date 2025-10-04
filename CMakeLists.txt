cmake_minimum_required(VERSION 3.15...4.1.1)

list(APPEND CMAKE_MESSAGE_CONTEXT PajladaSerialize)
project(
    PajladaSerialize
    VERSION 0.1.0
    DESCRIPTION "pajlada serialize library"
    HOMEPAGE_URL https://github.com/pajlada/serialize
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

option(PAJLADA_SERIALIZE_BUILD_TESTS "Build tests" OFF)

FetchContent_Declare(
    RapidJSON
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS REQUIRED
)

FetchContent_MakeAvailable(RapidJSON)

add_library(PajladaSerialize INTERFACE)
add_library(Pajlada::Serialize ALIAS PajladaSerialize)

target_include_directories(PajladaSerialize INTERFACE include)
if(TARGET rapidjson)
    target_link_libraries(PajladaSerialize INTERFACE rapidjson)
else()
    message(DEBUG "No rapidjson target found, this is most likely a system install. Adding include directories (${RAPIDJSON_INCLUDE_DIRS}) instead")
    target_include_directories(PajladaSerialize SYSTEM INTERFACE ${RAPIDJSON_INCLUDE_DIRS})
endif()

if(PAJLADA_SERIALIZE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
