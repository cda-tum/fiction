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
  option(FICTION_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  option(FICTION_ENABLE_IPO "Enable IPO/LTO" OFF)
  option(FICTION_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
  option(FICTION_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
  option(FICTION_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
  option(FICTION_ENABLE_PCH "Enable precompiled headers" OFF)
  option(FICTION_ENABLE_CACHE "Enable ccache" ON)
  option(FICTION_LIGHTWEIGHT_DEBUG_BUILDS
         "Reduce memory consumption of Debug builds" OFF)
  option(FICTION_ENABLE_TIME_TRACE
         "Emit Clang -ftime-trace compilation profiles" OFF)

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      FICTION_ENABLE_IPO
      FICTION_WARNINGS_AS_ERRORS
      FICTION_ENABLE_SANITIZER_ADDRESS
      FICTION_ENABLE_SANITIZER_LEAK
      FICTION_ENABLE_SANITIZER_UNDEFINED
      FICTION_ENABLE_SANITIZER_THREAD
      FICTION_ENABLE_SANITIZER_MEMORY
      FICTION_ENABLE_COVERAGE
      FICTION_ENABLE_PCH
      FICTION_ENABLE_CACHE
      FICTION_LIGHTWEIGHT_DEBUG_BUILDS
      FICTION_ENABLE_TIME_TRACE)
  endif()

endmacro()

macro(fiction_global_options)
  if(FICTION_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    fiction_enable_ipo()
  endif()

  fiction_supports_sanitizers()
endmacro()

macro(fiction_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(fiction_warnings INTERFACE)
  add_library(fiction_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  fiction_set_project_warnings(fiction_warnings ${FICTION_WARNINGS_AS_ERRORS}
                               "" "" "")

  target_compile_options(
    fiction_options
    INTERFACE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
      -fvisibility=hidden
      -fvisibility-inlines-hidden
      >)

  include(cmake/Sanitizers.cmake)
  fiction_enable_sanitizers(
    fiction_options ${FICTION_ENABLE_SANITIZER_ADDRESS}
    ${FICTION_ENABLE_SANITIZER_LEAK} ${FICTION_ENABLE_SANITIZER_UNDEFINED}
    ${FICTION_ENABLE_SANITIZER_THREAD} ${FICTION_ENABLE_SANITIZER_MEMORY})

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

  # Emit one JSON profile next to every object file, recording where the
  # compiler spent its time. Aggregate them with ClangBuildAnalyzer; see
  # docs/getting_started.rst.
  if(FICTION_ENABLE_TIME_TRACE)
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
      target_compile_options(fiction_options INTERFACE -ftime-trace)
    else()
      message(
        WARNING
          "-ftime-trace is a Clang feature; FICTION_ENABLE_TIME_TRACE has no effect for ${CMAKE_CXX_COMPILER_ID}."
      )
    endif()
  endif()

  # This applies a memory optimization for Debug builds which may be used to
  # conform to memory limitations
  if(FICTION_LIGHTWEIGHT_DEBUG_BUILDS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      # /Z7 also makes Debug compilations cacheable; ccache refuses /Zi. The
      # format has to be set, not just the flag, or CMake appends /Zi after it.
      set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
      string(REGEX REPLACE "/Zi" "" CMAKE_CXX_FLAGS_DEBUG
                           "${CMAKE_CXX_FLAGS_DEBUG}")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7 /Ob0")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID
                                                   MATCHES ".*Clang")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g1 -fno-inline")
    else()
      message(
        WARNING
          "Lightweight Debug builds are not supported for this compiler (${CMAKE_CXX_COMPILER_ID})."
      )
    endif()
  endif()

endmacro()
