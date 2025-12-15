include(FetchContent)

# nlohmann_json
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

# Catch2
if(FICTION_TEST)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.11.0
  )
  FetchContent_MakeAvailable(Catch2)
endif()

# pybind11
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v3.0.1
)
# Suppress warnings about removed FindPython modules in newer CMake versions
if(POLICY CMP0148)
  set(CMAKE_POLICY_DEFAULT_CMP0148 OLD)
endif()
set(PYBIND11_FINDPYTHON ON)
FetchContent_MakeAvailable(pybind11)

# parallel-hashmap
FetchContent_Declare(
        parallel-hashmap
        GIT_REPOSITORY https://github.com/greg7mdp/parallel-hashmap.git
        GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(parallel-hashmap)

# tinyxml2
set(tinyxml2_BUILD_TESTING OFF)
FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG 11.0.0
)
FetchContent_MakeAvailable(tinyxml2)

# alice
set(ALICE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALICE_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  alice
  GIT_REPOSITORY https://github.com/marcelwa/alice.git
  GIT_TAG        master # Using master as per submodule
)
FetchContent_MakeAvailable(alice)

# mockturtle
set(MOCKTURTLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MOCKTURTLE_EXPERIMENTS OFF CACHE BOOL "" FORCE)
set(MOCKTURTLE_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  mockturtle
  GIT_REPOSITORY https://github.com/marcelwa/mockturtle.git
  GIT_TAG        mnt # Using mnt branch as per submodule
)
FetchContent_MakeAvailable(mockturtle)

# ALGLIB
if(FICTION_ALGLIB)
  FetchContent_Declare(
    alglib-cmake
    GIT_REPOSITORY https://github.com/wlambooy/alglib-cmake.git
    GIT_TAG        master
  )
  FetchContent_MakeAvailable(alglib-cmake)
endif()
