include(${CMAKE_SOURCE_DIR}/libs/Catch2/extras/Catch.cmake)

include_directories(.)

file(GLOB_RECURSE FILENAMES */*.cpp)

foreach (FILE IN LISTS FILENAMES)
    get_filename_component(NAME ${FILE} NAME_WE)
    set(TEST_NAME test_${NAME})
    add_executable(${TEST_NAME} ${FILE})
    target_compile_definitions(${TEST_NAME} INTERFACE CATCH_CONFIG_NO_POSIX_SIGNALS) # make catch2 ignore SIGTERMs sent to applications when timeouts are reached
    target_link_libraries(${TEST_NAME} PRIVATE project_warnings project_options libfiction Catch2::Catch2WithMain)

    add_test(NAME ${NAME} COMMAND ${TEST_NAME}) # group tests by file
    #    catch_discover_tests(${TEST_NAME})
endforeach ()
