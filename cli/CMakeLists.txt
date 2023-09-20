SET(SOURCES ${PROJECT_SOURCE_DIR}/cli/fiction.cpp)

# Include configuration file
include_directories(${PROJECT_BINARY_DIR}/include/)

# Build executable
add_executable(fiction ${SOURCES})

# Link against the project settings, libfiction and alice
target_link_libraries(fiction PRIVATE fiction::fiction_options fiction::fiction_warnings libfiction alice)


# Package the CLI executable
include(../cmake/PackageProject.cmake)

# Add other targets that you want installed here, by default we just package the one executable
# we know we want to ship
fiction_package_project(
        TARGETS
        fiction
        fiction_options
        fiction_warnings
)

# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you
# track a build to a specific GIT SHA
set(CPACK_PACKAGE_FILE_NAME
        "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)
