add_library(libfiction INTERFACE)

target_include_directories(libfiction INTERFACE ${PROJECT_SOURCE_DIR}/include
                                                ${PROJECT_BINARY_DIR}/include/)

target_compile_features(libfiction INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

target_link_libraries(libfiction INTERFACE fiction::fiction_options
                                           fiction::fiction_warnings)

set_target_properties(
  libfiction
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

# Set UTF-8 encoding for MSVC
if (MSVC)
    target_compile_options(libfiction INTERFACE /utf-8)
    add_definitions(-DUNICODE -D_UNICODE)
endif ()

# Add configuration file
configure_file(${PROJECT_SOURCE_DIR}/include/fiction/utils/version_info.hpp.in
               utils/version_info.hpp)

# Collect top-level public headers under include/
file(GLOB_RECURSE FICTION_PUBLIC_HEADERS CONFIGURE_DEPENDS
     RELATIVE ${PROJECT_SOURCE_DIR}/include
     ${PROJECT_SOURCE_DIR}/include/*.hpp)

# Register header file set for IDE integration and installation metadata
# Multiple BASE_DIRS so CMake knows how to layout the installed tree
#
# Using a FILE_SET ensures proper exposure in CMake package exports later
# if export() logic is added.
#
# (GLOB with CONFIGURE_DEPENDS keeps IDE view in sync when adding headers.)
target_sources(libfiction
  INTERFACE
    FILE_SET HEADERS
      BASE_DIRS
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/include
      FILES
        ${FICTION_PUBLIC_HEADERS}
        ${PROJECT_BINARY_DIR}/include/utils/version_info.hpp
)
# Ensure header verification is enabled for this target
set_property(TARGET libfiction PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Enforce project-wide C++ standard feature requirement (redundant but explicit)
target_compile_features(libfiction INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
