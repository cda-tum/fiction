# FetchALGLIB.cmake - Robust ALGLIB download with mirror support and retry logic

include(FetchContent)

# Set policy to allow FetchContent_Populate with declared content
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

set(ALGLIB_VERSION "3.14.0")
set(ALGLIB_FILE "alglib-${ALGLIB_VERSION}.cpp.gpl.tgz")

# Multiple mirror URLs for reliability Note: The official ALGLIB site uses HTTP
# (not HTTPS)
set(ALGLIB_URLS "http://www.alglib.net/translator/re/${ALGLIB_FILE}")

# Function to download ALGLIB tarball with retry logic
function(download_alglib_tarball DESTINATION_PATH)
  message(STATUS "Downloading ALGLIB ${ALGLIB_VERSION}...")

  set(DOWNLOAD_SUCCESS FALSE)

  # Try each mirror URL
  foreach(ALGLIB_URL IN LISTS ALGLIB_URLS)
    message(STATUS "Attempting download from: ${ALGLIB_URL}")

    # Try downloading with retry logic
    set(RETRY_COUNT 3)

    foreach(ATTEMPT RANGE 1 ${RETRY_COUNT})
      if(NOT DOWNLOAD_SUCCESS)
        # Determine if we should verify TLS (only for HTTPS URLs)
        string(FIND "${ALGLIB_URL}" "https://" IS_HTTPS)
        if(IS_HTTPS EQUAL 0)
          set(TLS_VERIFY_OPTION TLS_VERIFY ON)
        else()
          set(TLS_VERIFY_OPTION)
        endif()

        file(
          DOWNLOAD "${ALGLIB_URL}" "${DESTINATION_PATH}"
          STATUS DOWNLOAD_STATUS
          TIMEOUT 120
          ${TLS_VERIFY_OPTION} SHOW_PROGRESS)

        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 STATUS_MESSAGE)

        if(STATUS_CODE EQUAL 0)
          # Verify the file was actually downloaded and has reasonable size
          if(EXISTS "${DESTINATION_PATH}")
            file(SIZE "${DESTINATION_PATH}" FILE_SIZE)
            if(FILE_SIZE GREATER 100000) # ALGLIB tarball should be > 100KB
              message(
                STATUS "Successfully downloaded ALGLIB (${FILE_SIZE} bytes)")
              set(DOWNLOAD_SUCCESS
                  TRUE
                  PARENT_SCOPE)
              return()
            else()
              message(
                WARNING
                  "Downloaded file is too small (${FILE_SIZE} bytes), retrying..."
              )
              file(REMOVE "${DESTINATION_PATH}")
            endif()
          endif()
        else()
          message(
            WARNING
              "Download attempt ${ATTEMPT}/${RETRY_COUNT} failed: ${STATUS_MESSAGE}"
          )
          if(EXISTS "${DESTINATION_PATH}")
            file(REMOVE "${DESTINATION_PATH}")
          endif()
        endif()

        # Wait before retry (except on last attempt)
        if(NOT DOWNLOAD_SUCCESS AND ATTEMPT LESS RETRY_COUNT)
          message(STATUS "Waiting 5 seconds before retry...")
          execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 5)
        endif()
      endif()
    endforeach()

    if(DOWNLOAD_SUCCESS)
      return()
    endif()
  endforeach()

  set(DOWNLOAD_SUCCESS
      FALSE
      PARENT_SCOPE)
endfunction()

# Pre-download ALGLIB tarball before FetchContent processes alglib-cmake This
# avoids the unreliable download during alglib-cmake's CMakeLists.txt execution
set(ALGLIB_CACHE_DIR "${CMAKE_BINARY_DIR}/_alglib_cache")
set(ALGLIB_TARBALL_PATH "${ALGLIB_CACHE_DIR}/${ALGLIB_FILE}")

if(NOT EXISTS "${ALGLIB_TARBALL_PATH}")
  file(MAKE_DIRECTORY "${ALGLIB_CACHE_DIR}")
  download_alglib_tarball("${ALGLIB_TARBALL_PATH}")

  if(NOT EXISTS "${ALGLIB_TARBALL_PATH}")
    message(
      FATAL_ERROR
        "Failed to download ALGLIB from all mirror sources. "
        "Please manually download ${ALGLIB_FILE} from http://www.alglib.net/ "
        "and place it in ${ALGLIB_CACHE_DIR}/")
  endif()
else()
  message(STATUS "ALGLIB tarball already cached at ${ALGLIB_TARBALL_PATH}")
endif()

# Now fetch alglib-cmake using modern FetchContent. Note: We need to handle the
# extraction ourselves because alglib-cmake has a waiting loop that can cause
# timeouts in CI when the tarball exists but hasn't been extracted yet.
FetchContent_Declare(
  alglib-cmake
  GIT_REPOSITORY https://github.com/wlambooy/alglib-cmake.git
  GIT_TAG master)

# Populate alglib-cmake manually so we can prepare the files before it processes
FetchContent_GetProperties(alglib-cmake)
if(NOT alglib-cmake_POPULATED)
  FetchContent_Populate(alglib-cmake)

  # Copy the tarball to the source directory
  file(COPY "${ALGLIB_TARBALL_PATH}" DESTINATION "${alglib-cmake_SOURCE_DIR}")

  # Extract the tarball and prepare files to avoid the waiting loop
  set(ALGLIB_UNZIP_DIR "${alglib-cmake_SOURCE_DIR}/src")
  set(ALGLIB_EXTRACTED_SRC_DIR "${ALGLIB_UNZIP_DIR}/cpp/src")

  if(NOT EXISTS "${ALGLIB_EXTRACTED_SRC_DIR}")
    message(STATUS "Extracting ALGLIB tarball...")
    file(REMOVE_RECURSE "${ALGLIB_UNZIP_DIR}")
    file(MAKE_DIRECTORY "${ALGLIB_UNZIP_DIR}")

    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xfz
              "${alglib-cmake_SOURCE_DIR}/${ALGLIB_FILE}"
      WORKING_DIRECTORY "${ALGLIB_UNZIP_DIR}"
      RESULT_VARIABLE extract_result)

    if(NOT extract_result EQUAL 0)
      message(FATAL_ERROR "Failed to extract ALGLIB tarball")
    endif()

    # Reorganize headers as alglib-cmake expects
    file(GLOB ALGLIB_HEADERS "${ALGLIB_EXTRACTED_SRC_DIR}/*.h")
    file(GLOB ALGLIB_SOURCES "${ALGLIB_EXTRACTED_SRC_DIR}/*.cpp")

    set(HEADER_OUTPUT_DIR "${ALGLIB_EXTRACTED_SRC_DIR}/headers")
    file(MAKE_DIRECTORY "${HEADER_OUTPUT_DIR}")

    foreach(header ${ALGLIB_HEADERS})
      get_filename_component(header_filename ${header} NAME)
      file(RENAME ${header} "${HEADER_OUTPUT_DIR}/${header_filename}")
    endforeach()

    foreach(source_file ${ALGLIB_SOURCES})
      file(READ "${source_file}" file_content)
      string(REGEX
             REPLACE "#include \"([^\"]+)\\.h\"" "#include \"headers/\\1.h\""
                     modified_content "${file_content}")
      file(WRITE "${source_file}" "${modified_content}")
    endforeach()

    message(STATUS "ALGLIB extraction and preparation complete")
  endif()

  # Create the success file to signal completion
  file(TOUCH "${alglib-cmake_SOURCE_DIR}/success")

  # Now add the subdirectory
  add_subdirectory(${alglib-cmake_SOURCE_DIR} ${alglib-cmake_BINARY_DIR})
endif()
