include(ExternalProject)

set(JEMALLOC_TAG "5.3.0" CACHE STRING "Which tag/revision of jemalloc to use")
set(JEMALLOC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/libs/jemalloc")
set(JEMALLOC_PREFIX_DIR "${CMAKE_BINARY_DIR}/libs/jemalloc")
set(JEMALLOC_INSTALL_DIR "${JEMALLOC_PREFIX_DIR}/install")

message(STATUS "jemalloc will override the standard malloc implementation globally. Note that your program may run slower when jemalloc is used!")


if (NOT WIN32)
    # Decide parallel build args for non-Windows, if no jobserver is in MAKEFLAGS.
    if ("$ENV{MAKEFLAGS}" MATCHES "jobserver")
        set(PARALLEL_BUILD_ARGS "")
    else ()
        set(PARALLEL_BUILD_ARGS "-j${CMAKE_BUILD_PARALLEL_LEVEL}")
    endif ()
    #
    #  Unix-like path: Use autoconf+make
    #
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
else ()
    #
    #  Windows path: Use CMake-based build for jemalloc.
    #
    ExternalProject_Add(
            jemalloc_project
            PREFIX "${JEMALLOC_PREFIX_DIR}"
            URL "https://github.com/jemalloc/jemalloc/archive/refs/tags/${JEMALLOC_TAG}.tar.gz"
            SOURCE_DIR "${JEMALLOC_SOURCE_DIR}"

            # No special update command (you could do something else if you like).
            UPDATE_COMMAND ""

            # We'll do an in-source CMake build for simplicity. You can set BUILD_IN_SOURCE to OFF
            # if you prefer an out-of-source build (but then you must adjust paths).
            BUILD_IN_SOURCE ON

            # Step 1: Configure with CMake
            # (Choose your generator: "NMake Makefiles", "Visual Studio 17 2022", "Ninja", etc.)
            CONFIGURE_COMMAND
            ${CMAKE_COMMAND}
            -G "${CMAKE_GENERATOR}"          # e.g. "Visual Studio 17 2022"
            -A "${CMAKE_GENERATOR_PLATFORM}" # typically "x64"
            -T "${CMAKE_GENERATOR_TOOLSET}"  # e.g. "v143"
            -DCMAKE_INSTALL_PREFIX=${JEMALLOC_INSTALL_DIR}
            -DCMAKE_BUILD_TYPE=Release
            -DJEMALLOC_BUILD_TESTS=OFF
            -DJEMALLOC_MSVC_SHARED=OFF
            .

            # Step 2: Build
            # Multi-config generators require `--config Release`; for single-config (NMake), you can omit it.
            BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release

            # Step 3: Install
            INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config Release
    )
endif ()


# Create an imported target (works on all platforms).
add_library(jemalloc STATIC IMPORTED)
add_dependencies(jemalloc jemalloc_project)

# On non-Windows builds, the library name is usually "libjemalloc.a"; on Windows,
# depending on your CMake config, you might see "jemalloc.lib" or "jemalloc_static.lib".
# Adjust IMPORTED_LOCATION to match what you actually get in ${JEMALLOC_INSTALL_DIR}/lib.

if (WIN32)
    set_target_properties(jemalloc PROPERTIES
            IMPORTED_LOCATION "${JEMALLOC_INSTALL_DIR}/lib/jemalloc_static.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${JEMALLOC_INSTALL_DIR}/include"
    )
else ()
    set_target_properties(jemalloc PROPERTIES
            IMPORTED_LOCATION "${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a"
            INTERFACE_INCLUDE_DIRECTORIES "${JEMALLOC_INSTALL_DIR}/include"
    )
endif ()
