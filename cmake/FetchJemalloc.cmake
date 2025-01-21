include(ExternalProject)

set(JEMALLOC_TAG "5.3.0" CACHE STRING "Which tag/revision of jemalloc to use")
set(JEMALLOC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/libs/jemalloc")
set(JEMALLOC_PREFIX_DIR "${CMAKE_BINARY_DIR}/libs/jemalloc")
set(JEMALLOC_INSTALL_DIR "${JEMALLOC_PREFIX_DIR}/install")

message(STATUS "jemalloc will override the standard malloc implementation globally.")

# We only do this on non-Windows, as jobserver is a Make/GNU concept.
if (NOT MSVC)
    if ("$ENV{MAKEFLAGS}" MATCHES "jobserver")
        set(PARALLEL_BUILD_ARGS "")
        # Force sub-`make` to use jobserver with `-j`.
        # The presence of --jobserver-fds or similar in MAKEFLAGS is enough.
        # So typically we just call `make` with no explicit `-jN`.
    else ()
        # If there is no jobserver, you might pick a default parallel level:
        set(PARALLEL_BUILD_ARGS "-j${CMAKE_BUILD_PARALLEL_LEVEL}")
    endif ()
endif ()

ExternalProject_Add(
        jemalloc_project
        PREFIX ${JEMALLOC_PREFIX_DIR}
        URL "https://github.com/jemalloc/jemalloc/archive/refs/tags/${JEMALLOC_TAG}.tar.gz"
        SOURCE_DIR ${JEMALLOC_SOURCE_DIR}
        UPDATE_COMMAND echo "${JEMALLOC_TAG}-0-g`git ls-remote --tags https://github.com/jemalloc/jemalloc.git | grep ${JEMALLOC_TAG} | tail -1 | cut -f1`" > ${JEMALLOC_SOURCE_DIR}/VERSION
        CONFIGURE_COMMAND cd ${JEMALLOC_SOURCE_DIR} && ./autogen.sh --prefix=${JEMALLOC_INSTALL_DIR}
        BUILD_COMMAND cd ${JEMALLOC_SOURCE_DIR} && make ${PARALLEL_BUILD_ARGS}
        INSTALL_COMMAND cd ${JEMALLOC_SOURCE_DIR} && make ${PARALLEL_BUILD_ARGS} install
        BUILD_IN_SOURCE 1
)

# Define jemalloc as an imported library
add_library(jemalloc STATIC IMPORTED)
add_dependencies(jemalloc jemalloc_project)

set_target_properties(jemalloc PROPERTIES
        IMPORTED_LOCATION "${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a"
        INTERFACE_INCLUDE_DIRECTORIES "${JEMALLOC_INSTALL_DIR}/include"
)
