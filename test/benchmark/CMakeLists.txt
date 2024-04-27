file(GLOB_RECURSE FILENAMES *.cpp)

foreach(FILE IN LISTS FILENAMES)
  get_filename_component(NAME ${FILE} NAME_WE)
  set(BENCH_NAME bench_${NAME})
  add_executable(${BENCH_NAME} ${FILE})
  target_compile_definitions(
    ${BENCH_NAME} INTERFACE CATCH_CONFIG_NO_POSIX_SIGNALS) # make catch2 ignore
                                                           # SIGTERMs sent to
                                                           # applications when
                                                           # timeouts are
                                                           # reached
  target_link_libraries(
    ${BENCH_NAME} PRIVATE fiction::fiction_warnings fiction::fiction_options
                          libfiction Catch2::Catch2WithMain)

  add_test(NAME ${NAME} COMMAND ${BENCH_NAME}) # group tests by file
  # catch_discover_tests(${BENCH_NAME})

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
      TARGET ${BENCH_NAME}
      POST_BUILD
      COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${BENCH_NAME}>)
  endif()
endforeach()
