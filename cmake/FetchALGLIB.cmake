# FetchALGLIB.cmake - Robust ALGLIB download with mirror support and retry logic

include(FetchContent)

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

# Now fetch alglib-cmake using modern FetchContent
FetchContent_Declare(
  alglib-cmake
  GIT_REPOSITORY https://github.com/wlambooy/alglib-cmake.git
  GIT_TAG master
  PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ALGLIB_TARBALL_PATH}"
                "<SOURCE_DIR>/${ALGLIB_FILE}")

# Use modern FetchContent_MakeAvailable
FetchContent_MakeAvailable(alglib-cmake)
