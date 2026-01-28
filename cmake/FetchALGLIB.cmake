# FetchALGLIB.cmake - Fetch ALGLIB library with GitHub mirror support
#
# Downloads ALGLIB from a reliable GitHub mirror (cda-tum/alglib-mirror) with
# fallback to the official ALGLIB site. The GitHub mirror provides fast,
# reliable downloads in CI environments where the official site frequently times
# out.
#
# Change ALGLIB_VERSION below to use a different version. All versions 3.9.0
# through 4.07.0 are available on the mirror.

include(FetchContent)

# Set policy to allow FetchContent_Populate with declared content
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

# ALGLIB version configuration
set(ALGLIB_VERSION "3.14.0")
set(ALGLIB_FILE "alglib-${ALGLIB_VERSION}.cpp.gpl.tgz")

# Mirror URLs - GitHub mirror is tried first for reliability
set(ALGLIB_URLS
    "https://github.com/cda-tum/alglib-mirror/releases/download/v${ALGLIB_VERSION}/${ALGLIB_FILE}"
    "http://www.alglib.net/translator/re/${ALGLIB_FILE}")

# Fetch alglib-cmake wrapper library. Note: We use manual population and
# extraction instead of FetchContent_MakeAvailable because alglib-cmake has a
# problematic waiting loop that causes CI timeouts. By extracting and preparing
# the files ourselves, we bypass this issue entirely.
FetchContent_Declare(
  alglib-cmake
  GIT_REPOSITORY https://github.com/wlambooy/alglib-cmake.git
  GIT_TAG master)

# Download ALGLIB tarball (without extraction) using FetchContent. FetchContent
# handles retries and multiple URLs automatically.
FetchContent_Declare(
  alglib
  URL ${ALGLIB_URLS}
  DOWNLOAD_NO_EXTRACT TRUE)

# Get the ALGLIB tarball
FetchContent_GetProperties(alglib)
if(NOT alglib_POPULATED)
  FetchContent_Populate(alglib)
endif()

# Locate the downloaded tarball
set(ALGLIB_TARBALL_PATH "${alglib_SOURCE_DIR}/${ALGLIB_FILE}")

if(NOT EXISTS "${ALGLIB_TARBALL_PATH}")
  message(FATAL_ERROR "ALGLIB tarball not found at: ${ALGLIB_TARBALL_PATH}")
endif()

# Manually populate and prepare alglib-cmake
FetchContent_GetProperties(alglib-cmake)
if(NOT alglib-cmake_POPULATED)
  FetchContent_Populate(alglib-cmake)

  # Copy the downloaded ALGLIB tarball to alglib-cmake's expected location
  file(COPY "${ALGLIB_TARBALL_PATH}" DESTINATION "${alglib-cmake_SOURCE_DIR}")

  # Extract and reorganize the ALGLIB source files This must be done before
  # alglib-cmake processes its CMakeLists.txt
  set(ALGLIB_UNZIP_DIR "${alglib-cmake_SOURCE_DIR}/src")
  set(ALGLIB_EXTRACTED_SRC_DIR "${ALGLIB_UNZIP_DIR}/cpp/src")

  if(NOT EXISTS "${ALGLIB_EXTRACTED_SRC_DIR}")
    message(STATUS "Extracting and preparing ALGLIB ${ALGLIB_VERSION}...")

    file(REMOVE_RECURSE "${ALGLIB_UNZIP_DIR}")
    file(MAKE_DIRECTORY "${ALGLIB_UNZIP_DIR}")

    # Extract tarball
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xfz
              "${alglib-cmake_SOURCE_DIR}/${ALGLIB_FILE}"
      WORKING_DIRECTORY "${ALGLIB_UNZIP_DIR}"
      RESULT_VARIABLE extract_result)

    if(NOT extract_result EQUAL 0)
      message(FATAL_ERROR "Failed to extract ALGLIB tarball")
    endif()

    # Reorganize headers into separate directory (alglib-cmake expectation)
    file(GLOB ALGLIB_HEADERS "${ALGLIB_EXTRACTED_SRC_DIR}/*.h")
    file(GLOB ALGLIB_SOURCES "${ALGLIB_EXTRACTED_SRC_DIR}/*.cpp")

    set(HEADER_OUTPUT_DIR "${ALGLIB_EXTRACTED_SRC_DIR}/headers")
    file(MAKE_DIRECTORY "${HEADER_OUTPUT_DIR}")

    foreach(header ${ALGLIB_HEADERS})
      get_filename_component(header_filename ${header} NAME)
      file(RENAME ${header} "${HEADER_OUTPUT_DIR}/${header_filename}")
    endforeach()

    # Update include paths in source files
    foreach(source_file ${ALGLIB_SOURCES})
      file(READ "${source_file}" file_content)
      string(REGEX
             REPLACE "#include \"([^\"]+)\\.h\"" "#include \"headers/\\1.h\""
                     modified_content "${file_content}")
      file(WRITE "${source_file}" "${modified_content}")
    endforeach()

    message(STATUS "ALGLIB extraction and preparation complete")
  endif()

  # Create success marker file to prevent alglib-cmake's waiting loop
  file(TOUCH "${alglib-cmake_SOURCE_DIR}/success")

  # Add alglib-cmake to the build
  add_subdirectory(${alglib-cmake_SOURCE_DIR} ${alglib-cmake_BINARY_DIR})
endif()
