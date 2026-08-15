# Benchmarking (depends on Catch2)
option(
  FICTION_BENCHMARK
  "Build fiction benchmarks, which can evaluate the performance of certain code fragments"
  OFF)
if(FICTION_BENCHMARK)
  message(STATUS "Building fiction benchmarks")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
endif()

# Define libfiction_test as an interface target
add_library(libfiction_test INTERFACE)

# Apply compile definitions to libfiction_test that point to the test folder
target_compile_definitions(
  libfiction_test INTERFACE "TEST_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/\"")

include_directories(.)

# Pre-Compiled Header (PCH) carrier for the test suite. Every test executable
# compiles the same expensive header stack; building the PCH once here and
# reusing it keeps that work from being repeated in all of them.
if(FICTION_ENABLE_PCH)
  add_library(libfiction_test_pch STATIC
              ${CMAKE_CURRENT_SOURCE_DIR}/pch_test.cpp)
  target_link_libraries(
    libfiction_test_pch PRIVATE libfiction_test fiction::libfiction
                                Catch2::Catch2WithMain)
  target_compile_definitions(libfiction_test_pch
                             PRIVATE CATCH_CONFIG_NO_POSIX_SIGNALS)
  target_precompile_headers(libfiction_test_pch PRIVATE pch_test.hpp)
endif()

file(GLOB_RECURSE FILENAMES */*.cpp)
list(FILTER FILENAMES EXCLUDE REGEX "benchmark/.*$")

foreach(FILE IN LISTS FILENAMES)
  get_filename_component(NAME ${FILE} NAME_WE)
  set(TEST_NAME test_${NAME})
  add_executable(${TEST_NAME} ${FILE})

  # Link the test executable against libfiction and Catch2
  target_link_libraries(
    ${TEST_NAME} PRIVATE libfiction_test fiction::libfiction
                         Catch2::Catch2WithMain)

  # Make Catch2 ignore SIGTERMs sent to applications when timeouts are reached
  target_compile_definitions(${TEST_NAME} PRIVATE CATCH_CONFIG_NO_POSIX_SIGNALS)

  if(FICTION_ENABLE_PCH)
    target_precompile_headers(${TEST_NAME} REUSE_FROM libfiction_test_pch)
  endif()

  add_test(NAME ${NAME} COMMAND ${TEST_NAME}) # group tests by file
endforeach()
