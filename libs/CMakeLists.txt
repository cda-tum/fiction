# Include alice
add_subdirectory(alice/)

# Include mockturtle
add_subdirectory(mockturtle/)

# Option to enable Mugen
option(FICTION_ENABLE_MUGEN "Enable the usage of Mugen, a Python3 library by Winston Haaswijk for FCN one-pass synthesis, and its dependencies" OFF)
if (FICTION_ENABLE_MUGEN)
    target_compile_definitions(fictionlib INTERFACE MUGEN)
endif ()

if (FICTION_ENABLE_MUGEN)
    # Build glucose-syrup-4.1-parallel if Mugen is enabled
    message(STATUS "Building glucose for Mugen")
    add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/glucose-syrup
            PRE_BUILD
            COMMAND make
            COMMAND mv glucose-syrup ${PROJECT_BINARY_DIR}/glucose-syrup
            COMMAND make clean
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mugen/glucose-syrup-4.1/parallel/)

    # Make sure glucose's custom build commands are actually being executed
    add_custom_target(glucose_syrup
            ALL
            DEPENDS ${PROJECT_BINARY_DIR}/glucose-syrup)

    # Embedding the pybind11 interpreter
    target_link_libraries(fictionlib INTERFACE pybind11::embed)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mugen/mugen_info.hpp.in utils/mugen_info.hpp)
    target_include_directories(fictionlib INTERFACE ${PROJECT_BINARY_DIR}/libs/)

    message(STATUS "Mugen was enabled. Please note that it relies on the Python3 libraries 'graphviz', 'PySAT v0.1.6.dev6', and 'wrapt_timeout_decorator' to be properly installed")
endif ()
