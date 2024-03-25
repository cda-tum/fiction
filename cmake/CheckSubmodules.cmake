# check whether the submodule ``modulename`` is correctly cloned in the
# ``/libs`` directory.
macro(check_if_present modulename)
  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/libs/${modulename}/CMakeLists.txt")
    message(
      FATAL_ERROR
        "Submodule `${PROJECT_SOURCE_DIR}/libs/${modulename}` not cloned properly. Please run `git submodule update --init --recursive` from the main project directory to fix this issue."
    )
  endif()
endmacro()
