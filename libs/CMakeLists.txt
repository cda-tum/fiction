# Include alice
add_subdirectory(alice/)

# Include mockturtle
add_subdirectory(mockturtle/)


## Enable Mugen
#option(ENABLE_MUGEN "Enable the usage of Mugen, a Python3 library by Winston Haaswijk for FCN one-pass synthesis, and its dependencies" ON)
#if (ENABLE_MUGEN)
#    add_definitions(-DMUGEN)
#endif ()
#
## Build glucose-syrup-4.1-parallel if Mugen is enabled
#if (ENABLE_MUGEN)
#    add_custom_command(
#            OUTPUT ${PROJECT_BINARY_DIR}/glucose-syrup
#            PRE_BUILD
#            COMMAND make ${LIB_OUTPUT_CHANNEL}
#            COMMAND mv glucose-syrup ${PROJECT_BINARY_DIR}/glucose-syrup ${LIB_OUTPUT_CHANNEL}
#            COMMAND make clean ${LIB_OUTPUT_CHANNEL}
#            WORKING_DIRECTORY ${LIB_PREFIX}/mugen/glucose-syrup-4.1/parallel/)
#
#    # Make sure glucose's custom build commands are actually being executed
#    add_custom_target(glucose_syrup
#            ALL
#            DEPENDS ${PROJECT_BINARY_DIR}/glucose-syrup)
#endif ()

# Enable the usage of Z3
option(FICTION_Z3 "Find, include, and utilize the Z3 solver by Microsoft Research. It needs to be installed manually.")
if (FICTION_Z3)
    message(STATUS "Usage of the Z3 solver was enabled. Make sure that it is installed on your system!")
    option(FICTION_Z3_SEARCH_PATHS "Hints CMake where to search for the Z3 package. Usually only necessary if Z3 cannot be found automatically.")
    # Try to locate Z3
    find_package(Z3 PATHS ${FICTION_Z3_SEARCH_PATHS})

    if (Z3_FOUND)
        # Status update
        message(STATUS "Found Z3 solver version: ${Z3_VERSION}")
        message(STATUS "Found Z3 library: ${Z3_LIBRARIES}")
        message(STATUS "Found Z3 include directories: ${Z3_CXX_INCLUDE_DIRS}")

        # Compile definition to guard include files
        target_compile_definitions(fictionlib INTERFACE FICTION_Z3_SOLVER)
        # Include Z3 library
        target_include_directories(fictionlib INTERFACE ${Z3_CXX_INCLUDE_DIRS})
        # Link Z3
        target_link_libraries(fictionlib INTERFACE ${Z3_LIBRARIES})

    else ()
        message(SEND_ERROR "Z3 solver could not be detected")
    endif ()

endif ()
