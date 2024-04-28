include(cmake/SystemLink.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(FetchContent)

macro(fiction_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(fiction_setup_options)
  option(FICTION_ENABLE_HARDENING "Enable hardening" OFF)
  option(FICTION_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    FICTION_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies" ON
    FICTION_ENABLE_HARDENING OFF)

  option(FICTION_ENABLE_IPO "Enable IPO/LTO" OFF)
  option(FICTION_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
  option(FICTION_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
  option(FICTION_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
  option(FICTION_ENABLE_PCH "Enable precompiled headers" OFF)
  option(FICTION_ENABLE_CACHE "Enable ccache" ON)

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      FICTION_ENABLE_IPO
      FICTION_WARNINGS_AS_ERRORS
      FICTION_ENABLE_SANITIZER_ADDRESS
      FICTION_ENABLE_SANITIZER_LEAK
      FICTION_ENABLE_SANITIZER_UNDEFINED
      FICTION_ENABLE_SANITIZER_THREAD
      FICTION_ENABLE_SANITIZER_MEMORY
      FICTION_ENABLE_UNITY_BUILD
      FICTION_ENABLE_COVERAGE
      FICTION_ENABLE_PCH
      FICTION_ENABLE_CACHE)
  endif()

endmacro()

macro(fiction_global_options)
  if(FICTION_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    fiction_enable_ipo()
  endif()

  fiction_supports_sanitizers()

  if(FICTION_ENABLE_HARDENING AND FICTION_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR FICTION_ENABLE_SANITIZER_UNDEFINED
       OR FICTION_ENABLE_SANITIZER_ADDRESS
       OR FICTION_ENABLE_SANITIZER_THREAD
       OR FICTION_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    fiction_enable_hardening(fiction_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(fiction_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(fiction_warnings INTERFACE)
  add_library(fiction_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  fiction_set_project_warnings(fiction_warnings ${FICTION_WARNINGS_AS_ERRORS}
                               "" "" "" "")

  include(cmake/Sanitizers.cmake)
  fiction_enable_sanitizers(
    fiction_options ${FICTION_ENABLE_SANITIZER_ADDRESS}
    ${FICTION_ENABLE_SANITIZER_LEAK} ${FICTION_ENABLE_SANITIZER_UNDEFINED}
    ${FICTION_ENABLE_SANITIZER_THREAD} ${FICTION_ENABLE_SANITIZER_MEMORY})

  set_target_properties(fiction_options
                        PROPERTIES UNITY_BUILD ${FICTION_ENABLE_UNITY_BUILD})

  if(FICTION_ENABLE_PCH)
    target_precompile_headers(fiction_options INTERFACE <vector> <string>
                              <utility>)
  endif()

  if(FICTION_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    fiction_enable_cache()
  endif()

  if(FICTION_ENABLE_COVERAGE)
    include(cmake/Coverage.cmake)
    fiction_enable_coverage(fiction_options)
  endif()

  if(FICTION_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(fiction_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(FICTION_ENABLE_HARDENING AND NOT FICTION_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR FICTION_ENABLE_SANITIZER_UNDEFINED
       OR FICTION_ENABLE_SANITIZER_ADDRESS
       OR FICTION_ENABLE_SANITIZER_THREAD
       OR FICTION_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    fiction_enable_hardening(fiction_options OFF
                             ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
