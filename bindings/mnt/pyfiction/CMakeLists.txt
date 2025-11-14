if (APPLE)
    set(BASEPOINT @loader_path)
else ()
    set(BASEPOINT $ORIGIN)
endif ()
list(APPEND CMAKE_INSTALL_RPATH ${BASEPOINT} ${BASEPOINT}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

pybind11_add_module(pyfiction
        # Prefer thin LTO if available
        THIN_LTO pyfiction.cpp)
target_link_libraries(pyfiction PRIVATE libfiction)
target_include_directories(pyfiction
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET pyfiction PROPERTY POSITION_INDEPENDENT_CODE ON)

# Collect pyfiction headers under bindings/mnt/pyfiction/
file(GLOB_RECURSE FICTION_PYFICTION_HEADERS CONFIGURE_DEPENDS
        RELATIVE ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/
        ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/include/*.hpp)

# Register header file set for IDE integration and installation metadata
# Multiple BASE_DIRS so CMake knows how to layout the installed tree
#
# Using a FILE_SET ensures proper exposure in CMake package exports later
# if export() logic is added.
#
# (GLOB with CONFIGURE_DEPENDS keeps IDE view in sync when adding headers.)
target_sources(pyfiction
        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
        ${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/
        FILES
        ${FICTION_PYFICTION_HEADERS}
)
# Ensure header verification is enabled for this target
set_property(TARGET pyfiction PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Enforce project-wide C++ standard feature requirement (redundant but explicit)
target_compile_features(pyfiction PRIVATE cxx_std_${CMAKE_CXX_STANDARD})

# Install directive for scikit-build-core
install(TARGETS pyfiction
        DESTINATION .
        COMPONENT fiction_Python)
