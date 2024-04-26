pybind11_add_module(pyfiction pyfiction.cpp)
target_link_libraries(pyfiction PRIVATE libfiction)
target_include_directories(pyfiction
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET pyfiction PROPERTY POSITION_INDEPENDENT_CODE ON)
