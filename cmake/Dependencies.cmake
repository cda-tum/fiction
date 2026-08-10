include(FetchContent)

# nlohmann_json
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0)
FetchContent_MakeAvailable(nlohmann_json)

# Catch2
if(FICTION_TEST)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.15.3)
  FetchContent_MakeAvailable(Catch2)
endif()

# Note: nanobind (used for the Python bindings) is *not* declared here. Unlike
# the other dependencies in this file, it is resolved as an installed Python
# build dependency rather than via FetchContent, and is only needed when
# FICTION_PYTHON_BINDINGS is enabled -- see
# bindings/mnt/pyfiction/CMakeLists.txt, which is the first place in the
# configure run where that option is guaranteed to be known.

# parallel-hashmap
FetchContent_Declare(
  parallel-hashmap
  GIT_REPOSITORY https://github.com/greg7mdp/parallel-hashmap.git
  GIT_TAG v2.0.0)
FetchContent_MakeAvailable(parallel-hashmap)

# tinyxml2
set(tinyxml2_BUILD_TESTING OFF)
FetchContent_Declare(
  tinyxml2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG 11.0.0)
FetchContent_MakeAvailable(tinyxml2)

# alice
set(ALICE_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(ALICE_TEST
    OFF
    CACHE BOOL "" FORCE)
FetchContent_Declare(
  alice
  GIT_REPOSITORY https://github.com/marcelwa/alice.git
  GIT_TAG 6b7f941ca44f38226f5e2545224fa1194940cd73 # Head of the master branch
)
FetchContent_MakeAvailable(alice)

# mockturtle
set(MOCKTURTLE_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(MOCKTURTLE_EXPERIMENTS
    OFF
    CACHE BOOL "" FORCE)
set(MOCKTURTLE_TEST
    OFF
    CACHE BOOL "" FORCE)
FetchContent_Declare(
  mockturtle
  GIT_REPOSITORY https://github.com/marcelwa/mockturtle.git
  GIT_TAG c5807b7e2be424ec6cb2ea2152882ac3847af19c # Head of the mnt branch
)
FetchContent_MakeAvailable(mockturtle)

# ALGLIB
if(FICTION_ALGLIB)
  include(${PROJECT_SOURCE_DIR}/cmake/FetchALGLIB.cmake)
endif()
