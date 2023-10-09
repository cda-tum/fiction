pybind11_add_module(pyfiction pyfiction.cpp)
target_link_libraries(pyfiction PRIVATE libfiction)
target_include_directories(pyfiction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET pyfiction PROPERTY POSITION_INDEPENDENT_CODE ON)

# Strip the executable if we are in Release mode
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_STRIP)
        add_custom_command(
                TARGET pyfiction
                POST_BUILD
                COMMAND ${CMAKE_STRIP} $<TARGET_FILE:pyfiction>
        )
    else ()
        message(WARNING "Strip command is not available. The executables will not be stripped.")
    endif ()
endif ()
