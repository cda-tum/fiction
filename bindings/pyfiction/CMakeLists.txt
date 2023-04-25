pybind11_add_module(pyfiction pyfiction.cpp)
target_link_libraries(pyfiction PRIVATE libfiction)
target_include_directories(pyfiction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Python3 COMPONENTS Interpreter)

if (Python3_FOUND)
    message(STATUS "Python directories found, enabling installation command for pyfiction")
    install(TARGETS pyfiction
            COMPONENT python
            LIBRARY DESTINATION ${Python3_SITELIB})
endif ()
