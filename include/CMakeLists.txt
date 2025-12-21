target_sources(PajladaSerialize INTERFACE
    FILE_SET headers TYPE HEADERS FILES
    pajlada/serialize.hpp
    pajlada/serialize/common.hpp
    pajlada/serialize/deserialize.hpp
    pajlada/serialize/serialize.hpp
)

target_include_directories(PajladaSerialize INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
