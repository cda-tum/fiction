include(FetchContent)

# Dependencies are fetched as release tarballs rather than as git clones
# wherever that is possible. A clone carries the upstream history into every
# build directory -- for nlohmann_json that is 273 MB of `.git` against 2 MB of
# sources -- and a developer with several build directories or worktrees pays
# that cost once per directory. mockturtle is the exception; see the note below
# it.
#
# Every `URL` carries a `URL_HASH`. A `GIT_TAG` commit SHA verifies its own
# content; a URL does not, and a tag can be repointed at different bytes, so the
# hash is what keeps the switch from weakening the supply chain. When bumping a
# version, recompute the hash with `sha256sum` over the downloaded archive.
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
set(JSON_SHA256
    42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa)
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
FetchContent_Declare(
  nlohmann_json
  URL ${JSON_URL}
  URL_HASH SHA256=${JSON_SHA256})
FetchContent_MakeAvailable(nlohmann_json)

# Catch2
if(FICTION_TEST)
  set(CATCH2_VERSION
      3.15.3
      CACHE STRING "Catch2 version")
  set(CATCH2_URL
      https://github.com/catchorg/Catch2/archive/refs/tags/v${CATCH2_VERSION}.tar.gz
  )
  set(CATCH2_SHA256
      b0299ae552918220a7a6e21e7de5b714777f4e8c883fb70c4bb23fe01df8c6e3)
  FetchContent_Declare(
    Catch2
    URL ${CATCH2_URL}
    URL_HASH SHA256=${CATCH2_SHA256})
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
set(PARALLEL_HASHMAP_SHA256
    4f462f51a3468166ea4cf87c80e001dc1999093264cf55cbda3492ca39a7730b)
FetchContent_Declare(
  parallel-hashmap
  URL ${PARALLEL_HASHMAP_URL}
  URL_HASH SHA256=${PARALLEL_HASHMAP_SHA256})
FetchContent_MakeAvailable(parallel-hashmap)

# tinyxml2
set(TINYXML2_VERSION
    11.0.0
    CACHE STRING "tinyxml2 version")
set(TINYXML2_URL
    https://github.com/leethomason/tinyxml2/archive/refs/tags/${TINYXML2_VERSION}.tar.gz
)
set(TINYXML2_SHA256
    5556deb5081fb246ee92afae73efd943c889cef0cafea92b0b82422d6a18f289)
set(tinyxml2_BUILD_TESTING OFF)
FetchContent_Declare(
  tinyxml2
  URL ${TINYXML2_URL}
  URL_HASH SHA256=${TINYXML2_SHA256})
FetchContent_MakeAvailable(tinyxml2)

# alice
set(ALICE_REV
    6b7f941ca44f38226f5e2545224fa1194940cd73
    CACHE STRING "alice revision -- head of the master branch")
set(ALICE_URL https://github.com/marcelwa/alice/archive/${ALICE_REV}.tar.gz)
set(ALICE_SHA256
    38709e50db916639c4baf7b2a7e56449baa65d6b17e6616d62439853e65163d2)
set(ALICE_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(ALICE_TEST
    OFF
    CACHE BOOL "" FORCE)
FetchContent_Declare(
  alice
  URL ${ALICE_URL}
  URL_HASH SHA256=${ALICE_SHA256})
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
