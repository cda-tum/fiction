set(SOURCES ${PROJECT_SOURCE_DIR}/cli/fiction.cpp)

# Include configuration file
include_directories(${PROJECT_BINARY_DIR}/include/)

# Build executable
add_executable(fiction ${SOURCES})

# Link against the project settings, libfiction and alice
target_link_libraries(fiction PRIVATE libfiction alice)

# Compile-time decisions on which flows to compile

# Logic synthesis flow
option(FICTION_LOGIC_SYNTHESIS_FLOW "Enable the logic synthesis flow for the fiction CLI" ON)
if (FICTION_LOGIC_SYNTHESIS_FLOW)
    target_compile_definitions(fiction PRIVATE FICTION_LOGIC_SYNTHESIS_FLOW)
endif ()

# Physical design flow
option(FICTION_PHYSICAL_DESIGN_FLOW "Enable the physical design flow for the fiction CLI" ON)
if (FICTION_PHYSICAL_DESIGN_FLOW)
    target_compile_definitions(fiction PRIVATE FICTION_PHYSICAL_DESIGN_FLOW)
endif ()

# Physical simulation flow
option(FICTION_SIMULATION_FLOW "Enable the physical simulation flow for the fiction CLI" ON)
if (FICTION_SIMULATION_FLOW)
    target_compile_definitions(fiction PRIVATE FICTION_SIMULATION_FLOW)
endif ()

# If the logic synthesis flow is enabled, we can enable ABC as a callback
if (FICTION_LOGIC_SYNTHESIS_FLOW)
    # Enable ABC
    option(FICTION_ABC "Find, include, and utilize ABC. It needs to be installed manually." OFF)
    if (FICTION_ABC)
        message(STATUS "Usage of the Z3 solver was enabled. Make sure that it is installed on your system!")
        # Option for a user-defined path to ABC
        set(ABC_ROOT "" CACHE STRING "Path to the ABC directory")

        find_program(
                ABC_BINARY abc
                HINTS
                ${ABC_ROOT}
                $ENV{HOME}/.local/bin
                /usr/local/bin
                /usr/bin
                /opt/abc/bin
                ENV PATH
                DOC "Path to the ABC executable"
        )
        if (ABC_BINARY)
            message(STATUS "Found ABC binary: ${ABC_BINARY}")
            target_compile_definitions(fiction PRIVATE FICTION_ABC)
            target_compile_definitions(fiction PRIVATE ABC_EXECUTABLE="${ABC_BINARY}")
        else ()
            message(FATAL_ERROR "ABC not found. Please specify `ABC_ROOT` or ensure ABC is in your PATH.")
        endif ()
    endif ()
endif ()

# Strip the executable if we are in Release mode
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_STRIP)
        add_custom_command(
                TARGET fiction
                POST_BUILD
                COMMAND ${CMAKE_STRIP} $<TARGET_FILE:fiction>)
    else ()
        message(
                WARNING
                "Strip command is not available. The executables will not be stripped.")
    endif ()
endif ()

# Package the CLI executable
include(../cmake/PackageProject.cmake)

# Add other targets that you want installed here, by default we just package the
# one executable we know we want to ship
fiction_package_project(TARGETS fiction fiction_options fiction_warnings)

# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you track a
# build to a specific GIT SHA
set(CPACK_PACKAGE_FILE_NAME
        "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)
