include_directories(catch2)

set(DEFINE_TEST_HEADER test.cpp)
file(GLOB_RECURSE FILENAMES tests/*.cpp)
foreach (FILE IN LISTS FILENAMES)
    set(TEST_NAME test_${NAME})
    get_filename_component(NAME ${FILE} NAME_WE)
    add_executable(${TEST_NAME} ${FILE} ${DEFINE_TEST_HEADER})
    add_test(NAME ${NAME} COMMAND ${TEST_NAME})

#    if (ENABLE_COVERAGE)
#        target_link_libraries(${TEST_NAME} gcov)
#    endif ()
endforeach ()
