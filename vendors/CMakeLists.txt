# Prevent CMake from finding a system-installed fmt package to avoid version conflicts
# We use the bundled fmt version from alice/mockturtle submodules (now fetched)
set(CMAKE_DISABLE_FIND_PACKAGE_fmt TRUE)

# Include Dependencies (FetchContent) configuration
include(${PROJECT_SOURCE_DIR}/cmake/Dependencies.cmake)

################################################################################
# Fetched Dependencies (Managed via FetchContent in Dependencies.cmake)
################################################################################

# alice
target_link_system_libraries(libfiction INTERFACE
    $<BUILD_INTERFACE:alice>
    $<INSTALL_INTERFACE:fiction::alice>
)
install(DIRECTORY ${alice_SOURCE_DIR}/include/ DESTINATION include)

# mockturtle
target_link_system_libraries(libfiction INTERFACE
    $<BUILD_INTERFACE:mockturtle>
    $<INSTALL_INTERFACE:fiction::mockturtle>
)
install(DIRECTORY ${mockturtle_SOURCE_DIR}/include/ DESTINATION include)

# nlohmann_json
target_link_system_libraries(libfiction INTERFACE
    $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
    $<INSTALL_INTERFACE:fiction::nlohmann_json>
)
install(DIRECTORY ${nlohmann_json_SOURCE_DIR}/include/ DESTINATION include)

# parallel-hashmap
target_include_directories(libfiction SYSTEM INTERFACE
    $<BUILD_INTERFACE:${parallel-hashmap_SOURCE_DIR}/parallel_hashmap>
)
install(DIRECTORY ${parallel-hashmap_SOURCE_DIR}/parallel_hashmap DESTINATION include)

# tinyxml2
set_property(TARGET tinyxml2 PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_system_libraries(libfiction INTERFACE
    $<BUILD_INTERFACE:tinyxml2::tinyxml2>
    $<INSTALL_INTERFACE:fiction::tinyxml2>
)
install(FILES ${tinyxml2_SOURCE_DIR}/tinyxml2.h DESTINATION include)

# ALGLIB
option(FICTION_ALGLIB "Automatically download, include, and utilize ALGLIB by the ALGLIB project.")
if (FICTION_ALGLIB)
    target_compile_definitions(libfiction INTERFACE FICTION_ALGLIB_ENABLED)
    target_include_directories(libfiction SYSTEM INTERFACE
        $<BUILD_INTERFACE:${alglib-cmake_SOURCE_DIR}/src/cpp/src/headers>
    )
    target_link_system_libraries(libfiction INTERFACE
        $<BUILD_INTERFACE:ALGLIB>
        $<INSTALL_INTERFACE:fiction::alglib>
    )
    install(DIRECTORY ${alglib-cmake_SOURCE_DIR}/src/cpp/src/headers/ DESTINATION include/alglib)
endif ()

# pybind11 is embedded via mugen or used for bindings, handled via FetchContent target where needed

################################################################################
# Vendored Dependencies (Shipped with fiction in vendors/)
################################################################################

# Undirected Graph (Header-only)
target_include_directories(libfiction SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/undirected_graph/source>
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/undirected_graph/source/ DESTINATION include/undirected_graph)

# Combinations (Header-only)
target_include_directories(libfiction SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/combinations>
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/combinations/ DESTINATION include/combinations FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

# Graph Coloring
add_subdirectory(graph-coloring EXCLUDE_FROM_ALL)
target_include_directories(libfiction SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/graph-coloring/Header>
)
target_link_system_libraries(libfiction INTERFACE
    $<BUILD_INTERFACE:graph-coloring>
    $<INSTALL_INTERFACE:fiction::graph-coloring>
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/graph-coloring/Header/ DESTINATION include/graph-coloring)

# Mugen (Python/C++ Hybrid)
if (NOT WIN32)
    option(FICTION_ENABLE_MUGEN "Enable the usage of Mugen, a Python3 library by Winston Haaswijk for FCN one-pass synthesis, and its dependencies" OFF)

    if (FICTION_ENABLE_MUGEN)
        target_compile_definitions(libfiction INTERFACE MUGEN)

        if (NOT APPLE)
            message(STATUS "Building glucose for Mugen")
            add_custom_command(
                    OUTPUT ${PROJECT_BINARY_DIR}/glucose-syrup
                    COMMAND make
                    COMMAND mv glucose-syrup ${PROJECT_BINARY_DIR}/glucose-syrup
                    COMMAND make clean
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mugen/glucose-syrup-4.1/parallel/)

            add_custom_target(glucose_syrup ALL DEPENDS ${PROJECT_BINARY_DIR}/glucose-syrup)
        endif ()

        target_link_system_libraries(libfiction INTERFACE pybind11::embed)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mugen/mugen_info.hpp.in utils/mugen_info.hpp)
        # Note: vendors/ path usage because we are in vendors/CMakeLists.txt, so CMAKE_CURRENT_SOURCE_DIR isvendors/
        # But we need to include generated header which is in PROJECT_BINARY_DIR/vendors/utils (actually generated to utils/ in configure_file above relative to binary dir?)
        # configure_file dst is "utils/mugen_info.hpp". Since we are in sub dir, it is relative to CMAKE_CURRENT_BINARY_DIR which is build/vendors.
        # So header is at build/vendors/utils/mugen_info.hpp.
        # Previous cmake code used ${PROJECT_BINARY_DIR}/libs/. since we are now in vendors, relative path utils/ goes to build/vendors/utils/.
        # We should include build/vendors/.
        target_include_directories(libfiction INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

        message(STATUS "Mugen was enabled. Please note that it relies on the Python3 libraries 'graphviz', 'PySAT v0.1.6.dev6', and 'wrapt_timeout_decorator' to be properly installed")
    endif ()
endif ()

################################################################################
# System / Other Dependencies
################################################################################

# Z3
option(FICTION_Z3 "Find, include, and utilize the Z3 solver by Microsoft Research. It needs to be installed manually." OFF)
if (FICTION_Z3)
    message(STATUS "Usage of the Z3 solver was enabled. Make sure that it is installed on your system!")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
    find_package(Z3 4.8.5)

    if (Z3_FOUND)
        message(STATUS "Found Z3 solver version: ${Z3_VERSION_STRING}")
        find_package(Threads REQUIRED)
        target_compile_definitions(libfiction INTERFACE FICTION_Z3_SOLVER)
        target_include_directories(libfiction SYSTEM INTERFACE ${Z3_CXX_INCLUDE_DIRS})
        target_link_system_libraries(libfiction INTERFACE ${Z3_LIBRARIES})
        if (APPLE)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
        endif ()
    else ()
        message(SEND_ERROR "Z3 solver could not be detected")
    endif ()
endif ()

# TBB
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
    find_package(TBB)
    if (TBB_FOUND)
        if (${TBB_VERSION_MAJOR} GREATER_EQUAL 2021 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.0.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0.0)
                target_compile_definitions(libfiction INTERFACE _GLIBCXX_USE_TBB_PAR_BACKEND=0)
                message(STATUS "TBB version ${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR} detected. Disabling parallel policies for GCC 9 and 10 due to incompatible interfaces.")
            else ()
                message(STATUS "Found TBB version: ${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR}")
                message(STATUS "Parallel STL algorithms are enabled")
            endif ()
        else ()
            message(STATUS "Found TBB version: ${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR}")
            message(STATUS "Parallel STL algorithms are enabled")
        endif ()
        target_include_directories(libfiction INTERFACE ${TBB_INCLUDE_DIRS})
        target_link_system_libraries(libfiction INTERFACE TBB::tbb)
    else ()
        message(STATUS "Parallel STL algorithms are disabled. If you want to use them, please install TBB and set the TBB_ROOT_DIR, TBB_INCLUDE_DIR, and TBB_LIBRARY variables accordingly.")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Parallel STL algorithms are enabled on MSVC by default")
endif ()

# jemalloc
option(FICTION_ENABLE_JEMALLOC "Automatically download and link jemalloc by Jason Evans.")
if (FICTION_ENABLE_JEMALLOC)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(STATUS "Automatic downloading and linking of jemalloc is not supported on Windows. Make sure that it is installed on your system!")
        find_package(jemalloc CONFIG REQUIRED)
        target_link_libraries(libfiction INTERFACE jemalloc)
    else ()
        find_package(jemalloc)
        if (NOT jemalloc_FOUND)
            message(STATUS "Building and installing jemalloc will proceed automatically")
            include(${PROJECT_SOURCE_DIR}/cmake/FetchJemalloc.cmake)
            set(JEMALLOC_LIBRARIES jemalloc)
            set(JEMALLOC_INCLUDE_DIRS jemalloc)
        endif ()
        if (APPLE)
            target_link_system_libraries(libfiction INTERFACE ${JEMALLOC_LIBRARIES} c++ dl pthread m)
        elseif (UNIX)
            target_link_system_libraries(libfiction INTERFACE ${JEMALLOC_LIBRARIES} stdc++ dl pthread m)
        else ()
            message(FATAL_ERROR "Unsupported environment")
        endif ()
        target_include_directories(libfiction SYSTEM INTERFACE ${JEMALLOC_INCLUDE_DIRS})
    endif ()
endif ()
