add_library(libfiction INTERFACE fiction/layouts/siqad_to_cartesian.hpp)
target_include_directories(libfiction INTERFACE ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include/)

# Add configuration file
configure_file(${PROJECT_SOURCE_DIR}/include/fiction/utils/version_info.hpp.in utils/version_info.hpp)
