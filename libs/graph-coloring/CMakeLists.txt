CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(graph-coloring)

SET(CMAKE_CXX_STANDARD 17)

ADD_LIBRARY(graph-coloring STATIC
        Source/coloring_algorithm.cpp
        Source/dsatur.cpp
        Source/mcs.cpp
        Source/lmxrlf.cpp
        Source/tabucol.cpp
        Source/hybrid_dsatur.cpp
        Source/hybrid_lmxrlf.cpp
        )

set_target_properties(graph-coloring PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

include(GenerateExportHeader)

GENERATE_EXPORT_HEADER(graph-coloring
        BASE_NAME graph-coloring
        EXPORT_MACRO_NAME GRAPH_COLORING_EXPORTS
        EXPORT_FILE_NAME graph_coloring_exports.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif ()
