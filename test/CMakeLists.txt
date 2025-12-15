# Benchmarking (depends on Catch2)
option(
        FICTION_BENCHMARK
        "Build fiction benchmarks, which can evaluate the performance of certain code fragments"
        OFF)
if (FICTION_BENCHMARK)
    message(STATUS "Building fiction benchmarks")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
endif ()

# Define libfiction_test as an interface target
add_library(libfiction_test INTERFACE)

# Apply compile definitions to libfiction_test that point to the test folder
target_compile_definitions(
        libfiction_test
        INTERFACE "TEST_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/\"")

target_include_directories(libfiction_test INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(fiction_tests)

# Add test subdirectories (populates fiction_tests sources)
add_subdirectory(algorithms)
add_subdirectory(io)
add_subdirectory(layouts)
add_subdirectory(networks)
add_subdirectory(technology)
add_subdirectory(utils)

# Link the test executable against libfiction and Catch2
target_link_libraries(fiction_tests PRIVATE libfiction_test fiction::libfiction Catch2::Catch2WithMain)

# Make Catch2 ignore SIGTERMs sent to applications when timeouts are reached
target_compile_definitions(fiction_tests PRIVATE CATCH_CONFIG_NO_POSIX_SIGNALS)

# Register tests
list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(fiction_tests)
