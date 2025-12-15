# Large parts of this build system are based on Jason Turner's C++ Starter
# Project (https://github.com/lefticus/cpp_starter_project) and CMake Template
# (https://github.com/cpp-best-practices/cmake_template)

cmake_minimum_required(VERSION 3.23...4.2)

# Only set the CMAKE_CXX_STANDARD if it is not set by someone else
if(NOT DEFINED CMAKE_CXX_STANDARD)
  # Set C++ standard; at least C++17 is required
  set(CMAKE_CXX_STANDARD 17)
endif()

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example when
# compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the project name and version
project(
  fiction
  VERSION 0.6.12
  DESCRIPTION
    "An open-source design automation framework for Field-coupled Nanotechnologies"
  HOMEPAGE_URL "https://github.com/cda-tum/fiction"
  LANGUAGES CXX C)

include(cmake/PreventInSourceBuilds.cmake)
include(cmake/ProjectOptions.cmake)
include(cmake/Utilities.cmake)
include(cmake/SystemLink.cmake)

fiction_setup_options()
fiction_global_options()
fiction_local_options()

set(GIT_SHA
    "Unknown"
    CACHE STRING "SHA this build was generated from")
string(SUBSTRING "${GIT_SHA}" 0 8 GIT_SHORT_SHA)

target_compile_features(fiction_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

# Alias for the options target
add_library(fiction::fiction_options ALIAS fiction_options)
# Alias for the warnings target
add_library(fiction::fiction_warnings ALIAS fiction_warnings)

# Include header files
add_subdirectory(include)

# Add vendors (formerly libs)
add_subdirectory(vendors)

# Enable progress bars
if(NOT WIN32)
  option(FICTION_PROGRESS_BARS "Enable animated progress bars in command line"
         ON)
  if(FICTION_PROGRESS_BARS)
    target_compile_definitions(fiction_options INTERFACE PROGRESS_BARS)
  endif()
endif()

# CLI
option(FICTION_CLI "Build fiction CLI" ON)
if(FICTION_CLI)
  message(STATUS "Building fiction CLI")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cli)
endif()

# Experiments
option(FICTION_EXPERIMENTS "Build fiction experiments" OFF)
if(FICTION_EXPERIMENTS)
  message(STATUS "Building fiction experiments")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/experiments)
endif()

# Python Bindings
option(FICTION_PYTHON_BINDINGS "Build fiction Python bindings" OFF)
if(FICTION_PYTHON_BINDINGS)
  message(STATUS "Building fiction Python bindings")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bindings/)
endif()

# Testing
option(FICTION_TEST "Build fiction tests" OFF)
if(FICTION_TEST)
  enable_testing()
  message(STATUS "Building fiction tests")
  add_subdirectory(test)
endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger
# environment so that it behaves well with MSVC's debugger, and we can run the
# target from visual studio
if(MSVC)
  get_all_installable_targets(all_targets)
  set_target_properties(
    ${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT
                              "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

# set the startup project for the "play" button in MSVC
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT fiction)

# Package Configuration Generation
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    fictionConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/fictionConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/fictionConfig.cmake
    INSTALL_DESTINATION lib/cmake/fiction
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/fictionConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/fictionConfigVersion.cmake
    DESTINATION lib/cmake/fiction
)

if(CMAKE_SKIP_INSTALL_RULES)
  return()
endif()
