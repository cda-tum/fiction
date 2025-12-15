# Include a system directory (which suppresses its warnings).
function(target_include_system_directories target)
  set(multiValueArgs INTERFACE PUBLIC PRIVATE)
  cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

  foreach(scope IN ITEMS INTERFACE PUBLIC PRIVATE)
    foreach(lib_include_dirs IN LISTS ARG_${scope})
      if(NOT MSVC)
        # system includes do not work in MSVC awaiting
        # https://gitlab.kitware.com/cmake/cmake/-/issues/18272# awaiting
        # https://gitlab.kitware.com/cmake/cmake/-/issues/17904
        set(_SYSTEM SYSTEM)
      endif()
      if(${scope} STREQUAL "INTERFACE" OR ${scope} STREQUAL "PUBLIC")
        target_include_directories(
          ${target} ${_SYSTEM} ${scope}
          "$<BUILD_INTERFACE:${lib_include_dirs}>"
          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
      else()
        target_include_directories(${target} ${_SYSTEM} ${scope}
                                   ${lib_include_dirs})
      endif()
    endforeach()
  endforeach()

endfunction()

# Include the directories of a library target as system directories (which
# suppresses their warnings).
function(target_include_system_library target scope lib)
  # Check if lib contains a generator expression for BUILD_INTERFACE
  string(REGEX MATCH "\\$<BUILD_INTERFACE:(.+)>" match "${lib}")
  if(CMAKE_MATCH_1)
    set(check_target ${CMAKE_MATCH_1})
  else()
    set(check_target ${lib})
  endif()

  # check if this is a target
  if(TARGET ${check_target})
    get_target_property(lib_include_dirs ${check_target} INTERFACE_INCLUDE_DIRECTORIES)
    if(lib_include_dirs)
      target_include_system_directories(${target} ${scope} ${lib_include_dirs})
    else()
      message(
        TRACE
        "${check_target} library does not have the INTERFACE_INCLUDE_DIRECTORIES property."
      )
    endif()
  endif()
endfunction()

# Link a library target as a system library (which suppresses its warnings).
function(target_link_system_library target scope lib)
  # Include the directories in the library
  target_include_system_library(${target} ${scope} ${lib})

  # Link the library
  target_link_libraries(${target} ${scope} ${lib})
endfunction()

# Link multiple library targets as system libraries (which suppresses their
# warnings).
function(target_link_system_libraries target)
  set(multiValueArgs INTERFACE PUBLIC PRIVATE)
  cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

  foreach(scope IN ITEMS INTERFACE PUBLIC PRIVATE)
    foreach(lib IN LISTS ARG_${scope})
      target_link_system_library(${target} ${scope} ${lib})
    endforeach()
  endforeach()
endfunction()
