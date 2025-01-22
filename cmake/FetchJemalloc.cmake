# FetchJemalloc.cmake   |   this file is intended to be run only on UNIX-like operating systems

if (WIN32)
    message(SEND_ERROR "Install jemalloc on Windows using the instructions found in https://github.com/jemalloc/jemalloc/blob/dev/INSTALL.md#building-for-windows.")
endif ()

include(ExternalProject)

set(JEMALLOC_TAG "5.3.0" CACHE STRING "Which tag/revision of jemalloc to use")
set(JEMALLOC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/libs/jemalloc")
set(JEMALLOC_PREFIX_DIR "${CMAKE_BINARY_DIR}/libs/jemalloc")
set(JEMALLOC_INSTALL_DIR "${JEMALLOC_PREFIX_DIR}/install")

# Decide parallel build args, if no jobserver is in MAKEFLAGS.
if ("$ENV{MAKEFLAGS}" MATCHES "jobserver")
    set(PARALLEL_BUILD_ARGS "")
else ()
    set(PARALLEL_BUILD_ARGS "-j${CMAKE_BUILD_PARALLEL_LEVEL}")
endif ()

ExternalProject_Add(
        jemalloc_project
        PREFIX ${JEMALLOC_PREFIX_DIR}
        URL "https://github.com/jemalloc/jemalloc/archive/refs/tags/${JEMALLOC_TAG}.tar.gz"
        SOURCE_DIR ${JEMALLOC_SOURCE_DIR}

        # Optional: embed version info if you want
        UPDATE_COMMAND bash -c "echo \"${JEMALLOC_TAG}-0-g$(git ls-remote --tags https://github.com/jemalloc/jemalloc.git | grep ${JEMALLOC_TAG} | tail -1 | cut -f1)\" > ${JEMALLOC_SOURCE_DIR}/VERSION"
        CONFIGURE_COMMAND cd ${JEMALLOC_SOURCE_DIR} && ./autogen.sh --prefix=${JEMALLOC_INSTALL_DIR} && rm VERSION
        BUILD_COMMAND cd ${JEMALLOC_SOURCE_DIR} && make ${PARALLEL_BUILD_ARGS}
        INSTALL_COMMAND cd ${JEMALLOC_SOURCE_DIR} && make -j1 install
        BUILD_IN_SOURCE 1
)

# Create an imported target (works on all platforms).
add_library(jemalloc STATIC IMPORTED)
add_dependencies(jemalloc jemalloc_project)

set_target_properties(jemalloc PROPERTIES
        IMPORTED_LOCATION "${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a"
        INTERFACE_INCLUDE_DIRECTORIES "${JEMALLOC_INSTALL_DIR}/include"
)
