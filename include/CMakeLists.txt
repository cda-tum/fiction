add_library(libfiction INTERFACE)

target_include_directories(libfiction INTERFACE ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include/)

target_compile_features(libfiction INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

set_target_properties(
        libfiction
        PROPERTIES VERSION ${PROJECT_VERSION}
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES)

# Add configuration file
configure_file(${PROJECT_SOURCE_DIR}/include/fiction/utils/version_info.hpp.in utils/version_info.hpp)
