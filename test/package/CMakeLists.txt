cmake_minimum_required(VERSION 3.23)
project(fiction_package_consumer LANGUAGES CXX)

find_package(fiction CONFIG REQUIRED)
add_executable(package_consumer package_consumer.cpp)
target_link_libraries(package_consumer PRIVATE fiction::libfiction)

enable_testing()
add_test(NAME installed_equivalence COMMAND package_consumer)
