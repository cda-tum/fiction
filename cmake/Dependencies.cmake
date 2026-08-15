include(FetchContent)

# Dependencies are fetched as release tarballs rather than as git clones
# wherever that is possible. A clone carries the upstream history into every
# build directory -- for nlohmann_json that is 273 MB of `.git` against 2 MB of
# sources -- and a developer with several build directories or worktrees pays
# that cost once per directory. mockturtle is the exception; see the note below
# it.
#
# None of these declarations uses `FIND_PACKAGE_ARGS`. vendors/CMakeLists.txt
# reads `<dep>_SOURCE_DIR` and the target names that FetchContent creates, so a
# dependency satisfied by an installed package instead would break the configure
# step. Supporting both paths means reworking vendors/CMakeLists.txt first.

# nlohmann_json
set(JSON_VERSION
    3.12.0
    CACHE STRING "nlohmann_json version")
set(JSON_URL
    https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.tar.xz
)
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
FetchContent_Declare(nlohmann_json URL ${JSON_URL})
FetchContent_MakeAvailable(nlohmann_json)

# Catch2
if(FICTION_TEST)
  set(CATCH2_VERSION
      3.15.3
      CACHE STRING "Catch2 version")
  set(CATCH2_URL
      https://github.com/catchorg/Catch2/archive/refs/tags/v${CATCH2_VERSION}.tar.gz
  )
  FetchContent_Declare(Catch2 URL ${CATCH2_URL})
  FetchContent_MakeAvailable(Catch2)
endif()

# Note: nanobind (used for the Python bindings) is *not* declared here. Unlike
# the other dependencies in this file, it is resolved as an installed Python
# build dependency rather than via FetchContent, and is only needed when
# FICTION_PYTHON_BINDINGS is enabled -- see
# bindings/mnt/pyfiction/CMakeLists.txt, which is the first place in the
# configure run where that option is guaranteed to be known.

# parallel-hashmap
set(PARALLEL_HASHMAP_VERSION
    2.0.0
    CACHE STRING "parallel-hashmap version")
set(PARALLEL_HASHMAP_URL
    https://github.com/greg7mdp/parallel-hashmap/archive/refs/tags/v${PARALLEL_HASHMAP_VERSION}.tar.gz
)
FetchContent_Declare(parallel-hashmap URL ${PARALLEL_HASHMAP_URL})
FetchContent_MakeAvailable(parallel-hashmap)

# tinyxml2
set(TINYXML2_VERSION
    11.0.0
    CACHE STRING "tinyxml2 version")
set(TINYXML2_URL
    https://github.com/leethomason/tinyxml2/archive/refs/tags/${TINYXML2_VERSION}.tar.gz
)
set(tinyxml2_BUILD_TESTING OFF)
FetchContent_Declare(tinyxml2 URL ${TINYXML2_URL})
FetchContent_MakeAvailable(tinyxml2)

# alice
set(ALICE_REV
    6b7f941ca44f38226f5e2545224fa1194940cd73
    CACHE STRING "alice revision -- head of the master branch")
set(ALICE_URL https://github.com/marcelwa/alice/archive/${ALICE_REV}.tar.gz)
set(ALICE_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(ALICE_TEST
    OFF
    CACHE BOOL "" FORCE)
FetchContent_Declare(alice URL ${ALICE_URL})
FetchContent_MakeAvailable(alice)

# mockturtle
#
# The one dependency still fetched as a clone. mockturtle carries
# `lib/parallel-hashmap` as a git submodule, and a GitHub source archive leaves
# submodule directories empty, so a tarball build fails on `#include
# <parallel_hashmap/phmap.h>`. Do not convert this to a `URL` without first
# arranging for that header to resolve.
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
