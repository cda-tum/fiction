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
# Ensure header verification is enabled for this target
set_property(TARGET libpyfiction PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

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
