cmake_minimum_required(VERSION 3.23)
project(fiction_package_consumer LANGUAGES CXX)

if(EXPECT_MISSING_DEPENDENCY)
  set(CMAKE_MODULE_PATH "consumer-module-path")
  set(PACKAGE_PREFIX_DIR "consumer-prefix")
  set(mockturtle_DIR "consumer-mockturtle")
  set(tinyxml2_DIR "consumer-tinyxml2")
  find_package(fiction CONFIG QUIET)
  if(fiction_FOUND
     OR NOT CMAKE_MODULE_PATH STREQUAL "consumer-module-path"
     OR NOT PACKAGE_PREFIX_DIR STREQUAL "consumer-prefix"
     OR NOT mockturtle_DIR STREQUAL "consumer-mockturtle"
     OR NOT tinyxml2_DIR STREQUAL "consumer-tinyxml2"
     OR NOT fiction_NOT_FOUND_MESSAGE MATCHES "${EXPECT_MISSING_DEPENDENCY}")
    message(
      FATAL_ERROR
        "Unavailable ${EXPECT_MISSING_DEPENDENCY} changed lookup state or omitted its diagnostic"
    )
  endif()
  return()
endif()

find_package(fiction CONFIG REQUIRED)
get_target_property(tinyxml2_type tinyxml2::tinyxml2 TYPE)
if(NOT tinyxml2_type STREQUAL "STATIC_LIBRARY")
  message(FATAL_ERROR "The installed TinyXML2 target is not static")
endif()
add_executable(package_consumer package_consumer.cpp)
target_link_libraries(package_consumer PRIVATE fiction::libfiction)
add_executable(sqd_writer_consumer sqd_writer_consumer.cpp)
target_link_libraries(sqd_writer_consumer PRIVATE fiction::libfiction)

enable_testing()
add_test(NAME installed_equivalence COMMAND package_consumer)
add_test(NAME installed_sqd_writer COMMAND sqd_writer_consumer)
