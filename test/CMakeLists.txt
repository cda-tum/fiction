include_directories(catch2)

set(DEFINE_TEST_HEADER test.cpp)
file(GLOB_RECURSE FILENAMES tests/*.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)
endif ()


foreach (FILE IN LISTS FILENAMES)
    get_filename_component(NAME ${FILE} NAME_WE)
    set(TEST_NAME test_${NAME})
    add_executable(${TEST_NAME} ${FILE} ${DEFINE_TEST_HEADER})
    target_link_libraries(${TEST_NAME} PRIVATE mockturtle)

    if (ENABLE_COVERAGE)
        target_compile_options(${TEST_NAME} INTERFACE --coverage -O0 -g)
        target_link_libraries(${TEST_NAME} INTERFACE mockturtle --coverage gcov)
    endif ()

    add_test(NAME ${NAME} COMMAND ${TEST_NAME})
endforeach ()
