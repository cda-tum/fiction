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

include(${PROJECT_SOURCE_DIR}/cmake/AddPyfictionPythonBinding.cmake)

# Builds every extension module of the wheel.
add_custom_target(pyfiction)

# One extension module per top-level namespace. A nested namespace, such as
# `sidb::simulation::engines`, is a submodule of its top-level module.
foreach(
  module
  utils
  layouts
  networks
  synthesis
  qca
  mol_qca
  inml
  sidb
  fcn
  verification
  physical_design)
  file(GLOB_RECURSE module_sources CONFIGURE_DEPENDS
       ${CMAKE_CURRENT_SOURCE_DIR}/${module}/*.cpp)
  add_pyfiction_python_binding(pyfiction-${module} ${module_sources}
                               MODULE_NAME ${module})
endforeach()

# Verify that every binding header compiles on its own. The header set hangs off
# one module, which carries the same include directories as all others.
file(GLOB_RECURSE FICTION_PYFICTION_HEADERS CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
target_sources(
  pyfiction-utils
  PRIVATE FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include FILES
          ${FICTION_PYFICTION_HEADERS})
set_property(TARGET pyfiction-utils PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)
