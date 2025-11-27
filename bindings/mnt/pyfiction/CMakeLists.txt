if (APPLE)
    set(BASEPOINT @loader_path)
else ()
    set(BASEPOINT $ORIGIN)
endif ()
list(APPEND CMAKE_INSTALL_RPATH ${BASEPOINT} ${BASEPOINT}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Create an INTERFACE library for pyfiction headers (for clang-tidy and IDE support)
add_library(libpyfiction INTERFACE)
target_include_directories(libpyfiction INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/libs/pybind11/include
)
target_link_libraries(libpyfiction INTERFACE libfiction pybind11::pybind11)

# Collect pyfiction headers under bindings/mnt/pyfiction/
file(GLOB_RECURSE FICTION_PYFICTION_HEADERS CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/include/*.hpp)

# Register header file set for IDE integration and clang-tidy support
# This ensures compile commands are generated for the headers
target_sources(libpyfiction
        INTERFACE
        FILE_SET HEADERS
        BASE_DIRS
        ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/include
        FILES
        ${FICTION_PYFICTION_HEADERS}
)
# Ensure header verification is enabled for this target.
# This generates wrapper .cxx files in the build directory that include each header,
# providing compilation database entries that clang-tidy can use.
set_property(TARGET libpyfiction PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Workaround for clang-tidy in CI: pybind11_add_module creates a MODULE library whose
# compilation commands may not be properly discovered by cpp-linter when analyzing headers.
# Create an additional OBJECT library that compiles the same source file, ensuring
# pyfiction.cpp appears in compile_commands.json in a form that cpp-linter can discover.
# This library is not built by default and only exists for compilation database generation.
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_library(pyfiction_compile_commands OBJECT EXCLUDE_FROM_ALL pyfiction.cpp)
    target_link_libraries(pyfiction_compile_commands PRIVATE libpyfiction)
    target_compile_features(pyfiction_compile_commands PRIVATE cxx_std_${CMAKE_CXX_STANDARD})
endif()

pybind11_add_module(pyfiction
        # Prefer thin LTO if available
        THIN_LTO pyfiction.cpp)
target_link_libraries(pyfiction PRIVATE libpyfiction)
set_property(TARGET pyfiction PROPERTY POSITION_INDEPENDENT_CODE ON)

# Enforce project-wide C++ standard feature requirement (redundant but explicit)
target_compile_features(pyfiction PRIVATE cxx_std_${CMAKE_CXX_STANDARD})

# Install directive for scikit-build-core
install(TARGETS pyfiction
        DESTINATION .
        COMPONENT fiction_Python)
