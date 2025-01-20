include(${PROJECT_SOURCE_DIR}/cmake/CheckSubmodules.cmake)

# Include pybind11
add_subdirectory(pybind11)

# Include alice
set(ALICE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALICE_TEST OFF CACHE BOOL "" FORCE)
check_if_present(alice)
add_subdirectory(alice)

# Include mockturtle
set(MOCKTURTLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MOCKTURTLE_EXPERIMENTS OFF CACHE BOOL "" FORCE)
set(MOCKTURTLE_TEST OFF CACHE BOOL "" FORCE)
check_if_present(mockturtle)
add_subdirectory(mockturtle)
target_link_system_libraries(libfiction INTERFACE mockturtle)

# Include Catch2
if (FICTION_TEST)
    check_if_present(Catch2)
    add_subdirectory(Catch2)
endif ()

# Mugen is not available under Windows
if (NOT WIN32)
    # Option to enable Mugen
    option(FICTION_ENABLE_MUGEN "Enable the usage of Mugen, a Python3 library by Winston Haaswijk for FCN one-pass synthesis, and its dependencies" OFF)
    if (FICTION_ENABLE_MUGEN)
        target_compile_definitions(libfiction INTERFACE MUGEN)
    endif ()

    if (FICTION_ENABLE_MUGEN)

        # Apple does not need glucose because it seems to have issues there anyways
        if (NOT APPLE)
            # Build glucose-syrup-4.1-parallel if Mugen is enabled
            message(STATUS "Building glucose for Mugen")
            add_custom_command(
                    OUTPUT ${PROJECT_BINARY_DIR}/glucose-syrup
                    PRE_BUILD
                    COMMAND make
                    COMMAND mv glucose-syrup ${PROJECT_BINARY_DIR}/glucose-syrup
                    COMMAND make clean
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mugen/glucose-syrup-4.1/parallel/)

            # Make sure glucose's custom build commands are actually being executed
            add_custom_target(glucose_syrup
                    ALL
                    DEPENDS ${PROJECT_BINARY_DIR}/glucose-syrup)
        endif ()

        # Embedding the pybind11 interpreter
        target_link_system_libraries(libfiction INTERFACE pybind11::embed)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mugen/mugen_info.hpp.in utils/mugen_info.hpp)
        target_include_directories(libfiction INTERFACE ${PROJECT_BINARY_DIR}/libs/)

        message(STATUS "Mugen was enabled. Please note that it relies on the Python3 libraries 'graphviz', 'PySAT v0.1.6.dev6', and 'wrapt_timeout_decorator' to be properly installed")
    endif ()
endif ()

# Enable the usage of Z3
option(FICTION_Z3 "Find, include, and utilize the Z3 solver by Microsoft Research. It needs to be installed manually." OFF)
if (FICTION_Z3)
    message(STATUS "Usage of the Z3 solver was enabled. Make sure that it is installed on your system!")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/") # include FindZ3.cmake

    # Try to locate Z3 (minimum required version is 4.8.5 due to its performance improvements on the QBF solver)
    find_package(Z3 4.8.5)

    if (Z3_FOUND)
        # Status update
        message(STATUS "Found Z3 solver version: ${Z3_VERSION_STRING}")
        message(STATUS "Found Z3 library: ${Z3_LIBRARIES}")
        message(STATUS "Found Z3 include directories: ${Z3_CXX_INCLUDE_DIRS}")

        # Threads are used by Z3 and are, thus, required at this point
        find_package(Threads REQUIRED)

        # Compile definition to guard include files
        target_compile_definitions(libfiction INTERFACE FICTION_Z3_SOLVER)
        # Include Z3 library
        target_include_directories(libfiction INTERFACE SYSTEM ${Z3_CXX_INCLUDE_DIRS})
        # Link Z3
        target_link_system_libraries(libfiction INTERFACE ${Z3_LIBRARIES})

        # use libc++ on macOS
        if (APPLE)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
        endif ()

    else ()
        message(SEND_ERROR "Z3 solver could not be detected")
    endif ()

endif ()

# If using GCC or Clang, find TBB if installed
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/") # include FindTBB.cmake
    find_package(TBB)
    if (TBB_FOUND)
        # If TBB version >= 2021.1 then GCC 9 and 10 do not work properly due to incompatible ABI changes
        if (${TBB_VERSION_MAJOR} GREATER_EQUAL 2021 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.0.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0.0)
                target_compile_definitions(libfiction INTERFACE _GLIBCXX_USE_TBB_PAR_BACKEND=0)
                message(STATUS "TBB version ${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR} detected. Disabling parallel policies for GCC 9 and 10 due to incompatible interfaces.")
            else ()
                # Status update
                message(STATUS "Found TBB version: ${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR}")
                message(STATUS "Parallel STL algorithms are enabled")
            endif ()
        else ()
            # Status update
            message(STATUS "Found TBB version: ${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR}")
            message(STATUS "Parallel STL algorithms are enabled")
        endif ()

        # Include TBB
        target_include_directories(libfiction INTERFACE ${TBB_INCLUDE_DIRS})
        # Link TBB
        target_link_system_libraries(libfiction INTERFACE TBB::tbb)
    else ()
        message(STATUS "Parallel STL algorithms are disabled. If you want to use them, please install TBB and set the TBB_ROOT_DIR, TBB_INCLUDE_DIR, and TBB_LIBRARY variables accordingly.")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Status update
    message(STATUS "Parallel STL algorithms are enabled on MSVC by default")
endif ()

# Include JSON by Niels Lohmann
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json EXCLUDE_FROM_ALL)
target_link_system_libraries(libfiction INTERFACE nlohmann_json::nlohmann_json)

# Include parallel_hashmap by Gregory Popovitch
check_if_present(parallel-hashmap)
target_include_directories(libfiction SYSTEM INTERFACE parallel-hashmap/parallel_hashmap)

# Include undirected_graph by Fabian Löschner
target_include_directories(libfiction SYSTEM INTERFACE undirected_graph/source)

# Include combinations by Howard Hinnant
target_include_directories(libfiction SYSTEM INTERFACE combinations)

# Include graph-coloring by Brian Crites
add_subdirectory(graph-coloring EXCLUDE_FROM_ALL)
target_include_directories(libfiction SYSTEM INTERFACE graph-coloring/Header)
target_link_system_libraries(libfiction INTERFACE graph-coloring)

# Include tinyXML2
set(tinyxml2_BUILD_TESTING OFF)
check_if_present(tinyxml2)
add_subdirectory(tinyxml2 EXCLUDE_FROM_ALL)
set_property(TARGET tinyxml2 PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_system_libraries(libfiction INTERFACE tinyxml2::tinyxml2)

# Enable jemalloc to be linked
option(FICTION_JEMALLOC "Automatically download and link jemalloc by Jason Evans.")
if (FICTION_JEMALLOC)
    include(${PROJECT_SOURCE_DIR}/cmake/FetchJemalloc.cmake)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(JEMALLOC jemalloc)

    pkg_search_module(JEMALLOC REQUIRED jemalloc)
    include_directories(${JEMALLOC_INCLUDE_DIRS})

    target_link_system_libraries(libfiction INTERFACE ${JEMALLOC_LIBRARIES})

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ljemalloc -lm")
endif ()