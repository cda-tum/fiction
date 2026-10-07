cmake_minimum_required(VERSION 3.23)
project(fiction_package_consumer LANGUAGES CXX)

if(EXPECT_MISSING_MOCKTURTLE)
  set(CMAKE_MODULE_PATH "consumer-module-path")
  set(PACKAGE_PREFIX_DIR "consumer-prefix")
  find_package(fiction CONFIG QUIET)
  if(fiction_FOUND
     OR NOT CMAKE_MODULE_PATH STREQUAL "consumer-module-path"
     OR NOT PACKAGE_PREFIX_DIR STREQUAL "consumer-prefix")
    message(
      FATAL_ERROR "Unavailable mockturtle changed consumer package lookup state"
    )
  endif()
  return()
endif()

find_package(fiction CONFIG REQUIRED)
add_executable(package_consumer package_consumer.cpp)
target_link_libraries(package_consumer PRIVATE fiction::libfiction)

enable_testing()
add_test(NAME installed_equivalence COMMAND package_consumer)
