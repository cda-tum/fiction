add_executable(fiction)

target_sources(fiction PRIVATE
    fiction.cpp
)

# Add command subdirectories
add_subdirectory(cmd/general)
add_subdirectory(cmd/io)
add_subdirectory(cmd/logic)
add_subdirectory(cmd/physical_design)
add_subdirectory(cmd/simulation)
add_subdirectory(cmd/technology)
add_subdirectory(cmd/verification)

# Add include directory for command headers
target_include_directories(fiction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link against the project settings, libfiction and alice
target_link_libraries(fiction PRIVATE fiction::libfiction alice)

# Register header file set for IDE integration and installation metadata
target_sources(fiction
        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${PROJECT_SOURCE_DIR}/cli
        FILES
        stores.hpp
        pch_cli.hpp
)
# Ensure header verification is enabled for this target
set_property(TARGET fiction PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Enforce project-wide C++ standard feature requirement (redundant but explicit)
target_compile_features(fiction PRIVATE cxx_std_${CMAKE_CXX_STANDARD})

# ============================================================================
# Pre-Compiled Headers (PCH) for faster compilation
# ============================================================================
# Enable PCH for the fiction CLI to speed up compilation of all command files.
# The PCH includes stable, expensive headers like alice, fiction types, etc.
if (FICTION_ENABLE_PCH)
    target_precompile_headers(fiction PRIVATE pch_cli.hpp)
endif ()

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
fiction_strip_target(fiction)

# Installation
install(TARGETS fiction
    RUNTIME DESTINATION bin
)
