#Benchmarking(depends on Catch2)
option(
        FICTION_BENCHMARK
        "Build fiction benchmarks, which can evaluate the performance of certain code fragments"
        OFF)
if (FICTION_BENCHMARK)
    message(STATUS "Building fiction benchmarks")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
endif ()

#Define fiction_test as an interface target
add_library(fiction_test INTERFACE)

#Apply compile definitions for TEST_PATH and others
target_compile_definitions(
        fiction_test
        INTERFACE "TEST_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/\"")


include_directories(.)

file(GLOB_RECURSE FILENAMES */*.cpp)
list(FILTER FILENAMES EXCLUDE REGEX "benchmark/.*$")

foreach (FILE IN LISTS FILENAMES)
    get_filename_component(NAME ${FILE} NAME_WE)
    set(TEST_NAME test_${NAME})
    add_executable(${TEST_NAME} ${FILE})

    # Link to fiction_test to inherit the compile definitions
    target_link_libraries(${TEST_NAME} PRIVATE fiction_test)

    target_compile_definitions(
            ${TEST_NAME} INTERFACE CATCH_CONFIG_NO_POSIX_SIGNALS) # make catch2 ignore
    # SIGTERMs sent to
    # applications when
    # timeouts are reached
    target_link_libraries(${TEST_NAME} PRIVATE libfiction Catch2::Catch2WithMain)

  add_test(NAME ${NAME} COMMAND ${TEST_NAME}) # group tests by file
  # catch_discover_tests(${TEST_NAME})
endforeach()
