CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(alglib)

SET(CMAKE_CXX_STANDARD 17)

ADD_LIBRARY(alglib STATIC
        Source/alglibinternal.cpp
        Source/alglibmisc.cpp
        Source/ap.cpp
        Source/dataanalysis.cpp
        Source/diffequations.cpp
        Source/fasttransforms.cpp
        Source/integration.cpp
        Source/interpolation.cpp
        Source/linalg.cpp
        Source/optimization.cpp
        Source/solvers.cpp
        Source/specialfunctions.cpp
        Source/statistics.cpp
)

set_target_properties(alglib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

include(GenerateExportHeader)

GENERATE_EXPORT_HEADER(alglib
        BASE_NAME alglib
        EXPORT_MACRO_NAME ALGLIB_EXPORTS
        EXPORT_FILE_NAME alglib_exports.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif ()
