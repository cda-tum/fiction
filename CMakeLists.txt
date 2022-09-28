# Large parts of this build system are based on Jason Turner's C++ Starter Project (https://github.com/lefticus/cpp_starter_project)

cmake_minimum_required(VERSION 3.15)
project(fiction
        LANGUAGES CXX
        VERSION 0.4.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/StandardProjectSettings.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PreventInSourceBuilds.cmake)

# Set C++ standard; at least C++17 is required
set(FICTION_CXX_STANDARD "17" CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD ${FICTION_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Interface library to set project options
add_library(project_options INTERFACE)

# Option to enable time tracing with clang
if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    if (ENABLE_BUILD_WITH_TIME_TRACE)
        target_compile_options(project_options INTERFACE -ftime-trace)
    endif ()
endif ()

# Use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# Enable cache system
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Cache.cmake)

# Standard compiler warnings
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Sanitizer options if supported by compiler
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Allow for static analysis options
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

# Enable progress bars
if (NOT WIN32)
    option(FICTION_PROGRESS_BARS "Enable animated progress bars in command line" ON)
    if (FICTION_PROGRESS_BARS)
        target_compile_definitions(project_options INTERFACE PROGRESS_BARS)
    endif ()
endif ()

# Include header files
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)

# CLI
option(FICTION_CLI "Build fiction CLI" ON)
if (FICTION_CLI)
    message(STATUS "Building fiction CLI")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cli)
endif ()

# Experiments
option(FICTION_EXPERIMENTS "Build fiction experiments" OFF)
if (FICTION_EXPERIMENTS)
    message(STATUS "Building fiction experiments")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/experiments)
endif ()

# Testing
option(FICTION_TEST "Build fiction tests" OFF)
if (FICTION_TEST)
    enable_testing()
    message(STATUS "Building fiction tests")
    add_subdirectory(test)
endif ()

# Include libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs)
