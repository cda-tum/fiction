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
# reusing it keeps that work from being repeated in all of them. ClangCL can
# omit exception copy constructors from PCH metadata, corrupting the heap when
# async workers propagate exceptions. Keep PCH off for this ABI until
# https://github.com/llvm/llvm-project/issues/53486 is fixed.
if(FICTION_ENABLE_PCH AND NOT (MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  add_library(libfiction_test_pch STATIC
              ${CMAKE_CURRENT_SOURCE_DIR}/pch_test.cpp)
  target_link_libraries(
    libfiction_test_pch PRIVATE libfiction_test fiction::libfiction
                                Catch2::Catch2WithMain)
  target_compile_definitions(libfiction_test_pch
                             PRIVATE CATCH_CONFIG_NO_POSIX_SIGNALS)
  target_precompile_headers(libfiction_test_pch PRIVATE pch_test.hpp)
endif()

file(
  GLOB_RECURSE FILENAMES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  */*.cpp)
list(FILTER FILENAMES EXCLUDE REGEX "^benchmark/")
# The installed consumer has its own CMake project, driven by
# test/package/run.py.
list(FILTER FILENAMES EXCLUDE REGEX "^package/")

# These tests include utils/allocation_failure.hpp and own global new/delete. A
# second allocator replacement conflicts with their fault injection.
set(ALLOCATION_FAILURE_TESTS
    test_layout test_clocking_state test_gate_level_layout test_routing_utils
    test_determine_clocking test_cell_writer_allocation_failure)
get_target_property(ALLOCATION_FAILURE_LIBRARIES libfiction
                    INTERFACE_LINK_LIBRARIES)
list(FILTER ALLOCATION_FAILURE_LIBRARIES EXCLUDE REGEX "jemalloc")

# Test files carry the test_ prefix in their own name, so the target and the
# CTest name are both just the base name -- nothing is synthesized here.
foreach(FILE IN LISTS FILENAMES)
  get_filename_component(TEST_NAME ${FILE} NAME_WE)
  add_executable(${TEST_NAME} ${FILE})

  target_link_libraries(${TEST_NAME} PRIVATE libfiction_test)

  if(TEST_NAME IN_LIST ALLOCATION_FAILURE_TESTS)
    # Preserve all header dependencies except jemalloc's allocation operators.
    # Copy the property so imported dependencies keep their directory scopes.
    set_property(
      TARGET ${TEST_NAME}
      APPEND
      PROPERTY LINK_LIBRARIES "${ALLOCATION_FAILURE_LIBRARIES}")
    target_include_directories(
      ${TEST_NAME}
      PRIVATE $<TARGET_PROPERTY:libfiction,INTERFACE_INCLUDE_DIRECTORIES>)
    target_include_directories(
      ${TEST_NAME} SYSTEM
      PRIVATE $<TARGET_PROPERTY:libfiction,INTERFACE_SYSTEM_INCLUDE_DIRECTORIES>
    )
    target_compile_options(
      ${TEST_NAME}
      PRIVATE $<TARGET_PROPERTY:libfiction,INTERFACE_COMPILE_OPTIONS>)
    target_compile_definitions(
      ${TEST_NAME}
      PRIVATE $<TARGET_PROPERTY:libfiction,INTERFACE_COMPILE_DEFINITIONS>)
  else()
    target_link_libraries(${TEST_NAME} PRIVATE fiction::libfiction)
  endif()

  target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain)

  # Make Catch2 ignore SIGTERMs sent to applications when timeouts are reached
  target_compile_definitions(${TEST_NAME} PRIVATE CATCH_CONFIG_NO_POSIX_SIGNALS)

  # The shared PCH carrier inherits libfiction's allocator dependencies.
  if(TARGET libfiction_test_pch AND NOT TEST_NAME IN_LIST
                                    ALLOCATION_FAILURE_TESTS)
    target_precompile_headers(${TEST_NAME} REUSE_FROM libfiction_test_pch)
  endif()

  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME}) # group tests by file
  if(TEST_NAME IN_LIST ALLOCATION_FAILURE_TESTS)
    # Catch2 returns 4 when every test case skips the unsupported configuration.
    set_tests_properties(${TEST_NAME} PROPERTIES SKIP_RETURN_CODE 4)
  endif()
endforeach()
