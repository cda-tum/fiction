cmake_minimum_required(VERSION 3.2)
project(fiction
        LANGUAGES CXX
        VERSION 0.3.2)

# C++17
set(CMAKE_CXX_STANDARD 17)

# Set debug and release build options
if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /w /O2")
else ()
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wno-mismatched-tags -Wno-gnu-anonymous-struct -Wno-nested-anon-types -g -fms-extensions")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -w -O3")
endif ()

# Set release mode to default
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif ()
# Set the possible values of build type for cmake-gui
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# Enable progress bars
if (NOT WIN32)
    option(ENABLE_PROGRESS_BARS "Enable animated progress bars in command line" ON)
    if (ENABLE_PROGRESS_BARS)
        add_definitions(-DPROGRESS_BARS)
    endif ()
endif()

# Enable Mugen
option(ENABLE_MUGEN "Enable the usage of Mugen, a Python3 library by Winston Haaswijk for FCN one-pass synthesis, and its dependencies" ON)
if (ENABLE_MUGEN)
    add_definitions(-DMUGEN)
endif ()

# Include header files
include_directories(src/algo/ src/io/ src/io/cmd/ src/tech/ src/topo/ src/util/)

# Find source files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)
# Add configuration file
configure_file(src/util/version_info.h.in util/version_info.h)
# Include configuration file
include_directories(${PROJECT_BINARY_DIR}/util/)

# Require Boost libraries
find_package(Boost COMPONENTS system filesystem)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
else ()
	message(STATUS "Please configure Boost_CUSTOM_INCLUDE_DIRS and Boost_CUSTOM_LIBRARY_DIRS manually.")
	set(Boost_CUSTOM_INCLUDE_DIRS "" CACHE PATH "Boost include path")
	set(Boost_CUSTOM_LIBRARY_DIRS "" CACHE PATH "Boost library path")
	include_directories(${Boost_CUSTOM_INCLUDE_DIRS})
	link_directories(${Boost_CUSTOM_LIBRARY_DIRS})
endif ()

# Add option to compile libraries with verbose output
option(BUILD_LIBS_VERBOSE "Output status and warnings during library build" OFF)
# Use /dev/null to silence libraries. Not elegant but efficient
if (NOT BUILD_LIBS_VERBOSE)
    set(LIB_OUTPUT_CHANNEL > /dev/null)
endif ()

# Custom install prefix for libraries
set(LIB_PREFIX ${CMAKE_SOURCE_DIR}/libs)

# Set up a directory for Z3 solver
set(Z3_DIR ${PROJECT_BINARY_DIR}/z3)

if (UNIX)
    # Require Python interpreter
    if (${CMAKE_VERSION} VERSION_LESS "3.12.0")
        find_package(PythonInterp)  # deprecated since CMake 3.12, but works far better
        set(Z3_PYTHON ${PYTHON_EXECUTABLE})
    else ()
        find_package(Python3 COMPONENTS Interpreter Development)
        set(Z3_PYTHON ${Python3_EXECUTABLE})
    endif ()

    # Add option to build Z3 as a static library
    option(BUILD_Z3_STATIC_LIB "Build Z3 solver as a static library to link against fiction" OFF)
    if (BUILD_Z3_STATIC_LIB)
        set(Z3_LIB_FLAG --staticlib)
        set(Z3_LINK_TARGET libz3.a)
    else ()
        if (APPLE)
            set(Z3_LINK_TARGET libz3.dylib)
        else ()
            set(Z3_LINK_TARGET libz3.so)
        endif ()
    endif ()

    # include and library paths
    set(Z3_INCLUDE_DIR ${Z3_DIR}/include)
    set(Z3_LIB_DIR ${Z3_DIR}/lib)

    # Build and locally install Z3
    add_custom_command(
            OUTPUT ${Z3_LIB_DIR}/${Z3_LINK_TARGET}
            PRE_BUILD
            COMMAND ${Z3_PYTHON} scripts/mk_make.py -s --prefix=${Z3_DIR} ${Z3_LIB_FLAG} ${LIB_OUTPUT_CHANNEL}
            COMMAND $(MAKE) -C build ${LIB_OUTPUT_CHANNEL}
            COMMAND $(MAKE) -C build install ${LIB_OUTPUT_CHANNEL}
            WORKING_DIRECTORY ${LIB_PREFIX}/z3/)

    # Make sure Z3's custom build commands are actually being executed
    add_custom_target(z3
            ALL
            DEPENDS ${Z3_LIB_DIR}/${Z3_LINK_TARGET})

elseif (WIN32)
    set(Z3_LINK_TARGET libz3.lib)
    set(Z3_INCLUDE_DIR "${LIB_PREFIX}/z3/src/api" CACHE PATH "Path to Z3's custom include directory")
    set(Z3_LIB_DIR     "${LIB_PREFIX}/z3/build" CACHE PATH "Path to Z3's custom library directory")
    include_directories(${Z3_INCLUDE_DIR}/c++)
endif ()

# Include Z3
include_directories(${Z3_INCLUDE_DIR}/)

# Include cppitertools
include_directories(${LIB_PREFIX}/cppitertools/)

# Include alice
add_subdirectory(${LIB_PREFIX}/alice/)

# Include mockturtle
add_subdirectory(${LIB_PREFIX}/mockturtle/)

# Build glucose-syrup-4.1-parallel if Mugen is enabled
if (ENABLE_MUGEN)
    add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/glucose-syrup
            PRE_BUILD
            COMMAND make ${LIB_OUTPUT_CHANNEL}
            COMMAND mv glucose-syrup ${PROJECT_BINARY_DIR}/glucose-syrup ${LIB_OUTPUT_CHANNEL}
            COMMAND make clean ${LIB_OUTPUT_CHANNEL}
            WORKING_DIRECTORY ${LIB_PREFIX}/mugen/glucose-syrup-4.1/parallel/)

    # Make sure glucose's custom build commands are actually being executed
    add_custom_target(glucose_syrup
            ALL
            DEPENDS ${PROJECT_BINARY_DIR}/glucose-syrup)
            
    # include_directories(${Python3_INCLUDE_DIRS})
endif ()

# Build executable
add_executable(fiction ${SOURCES})
if (UNIX)
    add_dependencies(fiction z3)
endif ()

# Link against Boost, Z3, alice, lorina, and pybind11
target_link_libraries(fiction PRIVATE ${Boost_FILESYSTEM_LIBRARIES} ${Boost_SYSTEM_LIBRARIES}
                                      ${Z3_LIB_DIR}/${Z3_LINK_TARGET} alice mockturtle pybind11::embed ${Python3_LIBRARIES})
