SET(SOURCES ${PROJECT_SOURCE_DIR}/cli/fiction.cpp)

# Include configuration file
include_directories(${PROJECT_BINARY_DIR}/include/)

# Build executable
add_executable(fiction ${SOURCES})

# Link against the project settings, libfiction, alice, and mockturtle
target_link_libraries(fiction PRIVATE project_options project_warnings libfiction alice)
