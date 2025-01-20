# FetchJemalloc.cmake

include(ExternalProject)

# Function to fetch and build jemalloc
#function(fetch_jemalloc)
#    if(TARGET jemalloc)
#        message(STATUS "Jemalloc is already included, skipping duplicate inclusion.")
#        return()
#    endif()

set(JEMALLOC_TAG "5.3.0" CACHE STRING "Which tag/revision of jemalloc to use")
set(JEMALLOC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/libs/jemalloc")
set(JEMALLOC_PREFIX_DIR "${CMAKE_BINARY_DIR}/libs/jemalloc")
set(JEMALLOC_INSTALL_DIR "${JEMALLOC_PREFIX_DIR}/install")

message(STATUS "jemalloc will override the standard malloc implementation globally. Note that your program may run slower when jemalloc is used!")

# Check if the source directory already exists
if (NOT EXISTS "${JEMALLOC_SOURCE_DIR}/src")
    #    message(STATUS "Jemalloc source not found. Downloading...")
    set(JEMALLOC_DOWNLOAD_URL "https://github.com/jemalloc/jemalloc/archive/refs/tags/${JEMALLOC_TAG}.tar.gz")
    set(DOWNLOAD_COMMAND "")
else ()
    #    message(STATUS "Jemalloc source found at ${JEMALLOC_SOURCE_DIR}, skipping download.")
    set(JEMALLOC_DOWNLOAD_URL "")
    set(DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E echo "jemalloc source already exists, skipping download.")
endif ()

# Define the autogen.sh script path
set(JEMALLOC_AUTOGEN_SCRIPT "${JEMALLOC_SOURCE_DIR}/autogen.sh")

## Check if the autogen.sh script exists
#if (NOT EXISTS "${JEMALLOC_SOURCE_DIR}/configure")
#    message(STATUS "Running autogen.sh to generate configure script...")
#    execute_process(
#            COMMAND ${JEMALLOC_AUTOGEN_SCRIPT}
#            WORKING_DIRECTORY ${JEMALLOC_SOURCE_DIR}
#            RESULT_VARIABLE AUTOGEN_RESULT
#    )
#
#    if (NOT AUTOGEN_RESULT EQUAL 0)
#        message(FATAL_ERROR "Failed to run autogen.sh")
#    endif ()
#endif ()

if (WIN32)
    ExternalProject_Add(
            jemalloc_project
            PREFIX "${JEMALLOC_PREFIX_DIR}"
            GIT_REPOSITORY "https://github.com/jemalloc/jemalloc.git"
            GIT_TAG "${JEMALLOC_TAG}"
            UPDATE_DISCONNECTED TRUE
            CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Running configure under Windows (MSYS2 required)"
            COMMAND ./autogen.sh
            COMMAND ./configure --prefix=${JEMALLOC_INSTALL_DIR}
            BUILD_COMMAND make
            INSTALL_COMMAND make install
    )
else ()
    ExternalProject_Add(
            jemalloc_project
            PREFIX ${JEMALLOC_PREFIX_DIR}
            URL ${JEMALLOC_DOWNLOAD_URL}
            SOURCE_DIR ${JEMALLOC_SOURCE_DIR}
            DOWNLOAD_COMMAND ${DOWNLOAD_COMMAND}
            CONFIGURE_COMMAND cd ${JEMALLOC_SOURCE_DIR} && ./autogen.sh --prefix=${JEMALLOC_INSTALL_DIR}
            BUILD_COMMAND cd ${JEMALLOC_SOURCE_DIR} && $(MAKE)
            INSTALL_COMMAND cd ${JEMALLOC_SOURCE_DIR} && make install
    )
endif ()

# Define jemalloc as an imported library
add_library(jemalloc STATIC IMPORTED)
add_dependencies(jemalloc jemalloc_project)

# Set library location and includes
#if (WIN32)
#    set_target_properties(jemalloc PROPERTIES
#            IMPORTED_LOCATION "${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a"
#            INTERFACE_INCLUDE_DIRECTORIES "${JEMALLOC_INSTALL_DIR}/include"
#    )
#else ()
set_target_properties(jemalloc PROPERTIES
        IMPORTED_LOCATION "${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a"
        INTERFACE_INCLUDE_DIRECTORIES "${JEMALLOC_INSTALL_DIR}/include"
)
#endif ()

#endfunction()
