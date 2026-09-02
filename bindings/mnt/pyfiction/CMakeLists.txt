if(APPLE)
  set(BASEPOINT @loader_path)
else()
  set(BASEPOINT $ORIGIN)
endif()
list(APPEND CMAKE_INSTALL_RPATH ${BASEPOINT}
     ${BASEPOINT}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# nanobind is resolved as an installed Python build dependency (not
# FetchContent'd); locate it via the interpreter that scikit-build-core is
# building against.
set(Python_FIND_VIRTUALENV
    FIRST
    CACHE STRING "Give precedence to virtualenvs when searching for Python")
set(Python_FIND_FRAMEWORK
    LAST
    CACHE STRING "Prefer Brew/Conda to Apple framework Python")
set(Python_ARTIFACTS_INTERACTIVE
    ON
    CACHE BOOL
          "Prevent multiple searches for Python and instead cache the results.")
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module
                                        ${SKBUILD_SABI_COMPONENT})
execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  RESULT_VARIABLE nanobind_exec_result
  ERROR_VARIABLE nanobind_exec_err
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE nanobind_ROOT)
if(NOT nanobind_exec_result EQUAL 0)
  message(
    FATAL_ERROR
      "Failed to query nanobind CMake directory via '${Python_EXECUTABLE} -m nanobind --cmake_dir': ${nanobind_exec_err}"
  )
endif()
find_package(nanobind CONFIG REQUIRED PATHS "${nanobind_ROOT}" NO_DEFAULT_PATH)

# Split mode: the extension links no nanobind library code and resolves it at
# import time from the `nanobind-backend` wheel, so one abi3 binary covers
# Python 3.10 up. It implies STABLE_ABI. Free-threaded interpreters need the
# `abi3t` of Python 3.15 (PEP 803); below that, nanobind stops the configure
# with a message naming the version.
nanobind_add_module(
  pyfiction
  BACKEND_MODULE
  nanobind_backend
  FREE_THREADED
  LTO
  NB_SUPPRESS_WARNINGS
  pyfiction.cpp)
target_link_libraries(pyfiction PRIVATE libfiction)
target_include_directories(pyfiction
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET pyfiction PROPERTY POSITION_INDEPENDENT_CODE ON)

# Keep the embedded static libraries' symbols local. `fiction_options` adds
# `-fvisibility=hidden` to everything that links `libfiction`, and nanobind adds
# `CXX_VISIBILITY_PRESET hidden` on the module's own objects; the vendored
# archives -- `tinyxml2`, `graph-coloring`, ALGLIB -- consume neither and keep
# default visibility, so the extension re-exports them. Ported from
# marcelwa/aigverse#490.
if(APPLE)
  target_link_options(pyfiction PRIVATE
                      "LINKER:-exported_symbol,_PyInit_pyfiction")
elseif(UNIX)
  target_link_options(pyfiction PRIVATE "LINKER:--exclude-libs,ALL")

  # `--gc-sections` has little to collect without these: nanobind's own size
  # tuning stops at `-Os`, and neither the bindings nor the vendored archives
  # emit per-function sections. A linked build inherited both from
  # `nanobind-static`; split mode links no nanobind target.
  target_compile_options(
    pyfiction
    PRIVATE
      "$<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:-ffunction-sections;-fdata-sections>"
  )
  target_link_options(
    pyfiction
    PRIVATE
    "$<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:LINKER:--gc-sections>"
  )
endif()

# Collect pyfiction headers under bindings/mnt/pyfiction/
file(
  GLOB_RECURSE FICTION_PYFICTION_HEADERS CONFIGURE_DEPENDS
  RELATIVE ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/
  ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/include/*.hpp)

# Collect pyfiction sources under bindings/mnt/pyfiction/src
file(GLOB_RECURSE FICTION_PYFICTION_SOURCES CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/src/*.cpp)

# Register header file set for IDE integration and installation metadata
# Multiple BASE_DIRS so CMake knows how to layout the installed tree
#
# Using a FILE_SET ensures proper exposure in CMake package exports later if
# export() logic is added.
#
# (GLOB with CONFIGURE_DEPENDS keeps IDE view in sync when adding headers.)
target_sources(
  pyfiction
  PRIVATE ${FICTION_PYFICTION_SOURCES}
  PRIVATE FILE_SET HEADERS BASE_DIRS
          ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/ FILES
          ${FICTION_PYFICTION_HEADERS})
# Ensure header verification is enabled for this target
set_property(TARGET pyfiction PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Enforce project-wide C++ standard feature requirement (redundant but explicit)
target_compile_features(pyfiction PRIVATE cxx_std_${CMAKE_CXX_STANDARD})

# Install directive for scikit-build-core
install(
  TARGETS pyfiction
  DESTINATION .
  COMPONENT fiction_Python)
