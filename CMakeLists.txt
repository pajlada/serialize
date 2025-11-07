cmake_minimum_required(VERSION 3.15...4.1.1)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY On) # For rapidjson

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
    rapidjson
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/external/rapidjson
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS NAMES RapidJSON
)
set(RAPIDJSON_BUILD_EXAMPLES Off CACHE INTERNAL "")
set(RAPIDJSON_BUILD_TESTS Off CACHE INTERNAL "")

FetchContent_MakeAvailable(rapidjson)

add_library(PajladaSerialize INTERFACE)
add_library(Pajlada::Serialize ALIAS PajladaSerialize)

# Enable std::string overloads
target_compile_definitions(PajladaSerialize INTERFACE RAPIDJSON_HAS_STDSTRING=1)

target_include_directories(PajladaSerialize INTERFACE include)
if(TARGET rapidjson)
    message(DEBUG "Linking to rapidjson target")
    target_link_libraries(PajladaSerialize INTERFACE rapidjson)
elseif(DEFINED RapidJSON_SOURCE_DIR)
    message(DEBUG "RapidJSON_SOURCE_DIR defined, assuming this is a submodule/source build. (${RapidJSON_SOURCE_DIR})")
    target_include_directories(PajladaSerialize SYSTEM INTERFACE ${RapidJSON_SOURCE_DIR}/include)
else()
    message(DEBUG "No rapidjson target found, this is most likely a system install. Adding include directories (${RAPIDJSON_INCLUDE_DIRS}) instead")
    target_include_directories(PajladaSerialize SYSTEM INTERFACE ${RAPIDJSON_INCLUDE_DIRS})
endif()

if(PAJLADA_SERIALIZE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
