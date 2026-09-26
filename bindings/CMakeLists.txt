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

# The headers shared by all modules. The verified header set compiles each
# header on its own, which also gives it an entry in the compilation database
# that Clang-Tidy reads. The generated docstring header has no include guard and
# is only included through `documentation.hpp`.
add_library(pyfiction_headers INTERFACE)
target_include_directories(pyfiction_headers
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(pyfiction_headers SYSTEM INTERFACE ${NB_DIR}/include)
target_link_libraries(pyfiction_headers INTERFACE libfiction Python::Module)
file(GLOB_RECURSE FICTION_PYFICTION_HEADERS CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
list(FILTER FICTION_PYFICTION_HEADERS EXCLUDE REGEX
     "pybind11_mkdoc_docstrings\\.hpp$")
target_sources(
  pyfiction_headers
  INTERFACE FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
            FILES ${FICTION_PYFICTION_HEADERS})
set_property(TARGET pyfiction_headers PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

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
