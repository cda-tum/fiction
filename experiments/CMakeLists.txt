# largely based on mockturtle/experiments/CMakeLists.txt

add_library(fiction_experiments INTERFACE)
target_include_directories(
  fiction_experiments
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/libs/mockturtle/experiments/)
target_link_libraries(fiction_experiments INTERFACE libfiction)
if(ENABLE_MATPLOTLIB)
  target_link_libraries(fiction_experiments INTERFACE matplot)
endif()

# check for git revision
if(EXISTS ${PROJECT_SOURCE_DIR}/.git)
  find_package(Git)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      OUTPUT_VARIABLE "GIT_SHORT_REVISION"
      ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    target_compile_definitions(
      fiction_experiments
      INTERFACE "GIT_SHORT_REVISION=\"${GIT_SHORT_REVISION}\"")
  endif()
endif()

# path to the experiments cpp files and EPFL benchmarks
target_compile_definitions(
  fiction_experiments
  INTERFACE "EXPERIMENTS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/\"")

file(GLOB_RECURSE FILENAMES *.cpp)

foreach(filename ${FILENAMES})
  get_filename_component(expname ${filename} NAME_WE)
  add_executable(${expname} ${filename})
  target_link_libraries(${expname} PUBLIC fiction_experiments)

  # Strip the executable if we are in Release mode
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_STRIP)
      add_custom_command(
        TARGET ${expname}
        POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${expname}>)
    else()
      message(
        WARNING
          "Strip command is not available. The executables will not be stripped."
      )
    endif()
  endif()
endforeach()
