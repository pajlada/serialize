cmake_minimum_required(VERSION 3.25...4.1.1)

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

include(GNUInstallDirs)

option(PAJLADA_SERIALIZE_BUILD_TESTS "Build tests" OFF)
option(PAJLADA_SERIALIZE_INSTALL "Install PajladaSerialize" ${PROJECT_IS_TOP_LEVEL})

add_library(PajladaSerialize INTERFACE)
add_library(Pajlada::Serialize ALIAS PajladaSerialize)

set_target_properties(PajladaSerialize PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME Serialize
)

add_subdirectory(include)

# Enable std::string overloads
target_compile_definitions(PajladaSerialize INTERFACE RAPIDJSON_HAS_STDSTRING=1)

if(PAJLADA_SERIALIZE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(PAJLADA_SERIALIZE_INSTALL)
    include(CMakePackageConfigHelpers)

    # Install header files
    install(TARGETS PajladaSerialize
        EXPORT SerializeTargets
        FILE_SET headers
    )

    # Export(?) & install the Targets.cmake file
    install(EXPORT SerializeTargets
        FILE PajladaSerializeTargets.cmake
        NAMESPACE "Pajlada::"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    # Write the ConfigVersion.cmake and Config.cmake files and install them into lib/cmake/ProjectName/
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSerializeConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PajladaSerializeConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSerializeConfig.cmake
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/PajladaSerialize"
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSerializeConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/PajladaSerializeConfig.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
endif()

if(NOT PROJECT_IS_TOP_LEVEL)
    return(PROPAGATE
        PajladaSerialize_VERSION
        PajladaSerialize_VERSION_MAJOR
        PajladaSerialize_VERSION_MINOR
        PajladaSerialize_VERSION_PATCH
        PajladaSerialize_VERSION_TWEAK
    )
endif()
