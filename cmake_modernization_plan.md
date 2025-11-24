# CMake Modernization Plan for `fiction`

This document outlines a comprehensive plan to modernize the CMake configuration of the `fiction` project. The goal is to align with modern CMake best practices (3.20+), improve build times, simplify dependency management, and enable proper packaging and installation.

## 1. Dependency Management

**Current Status:** Dependencies are managed via Git Submodules in `libs/` and manually added via `add_subdirectory`.
**Recommendation:** Switch to **FetchContent** (built-in) or **CPM.cmake** (wrapper around FetchContent).

- **Action:** Create a `dependencies.cmake` file in `cmake/`.
- **Action:** For each dependency (Alice, Mockturtle, nlohmann_json, etc.), replace `check_if_present` + `add_subdirectory` with `FetchContent_Declare` and `FetchContent_MakeAvailable`.
- **Benefit:** No need to manually manage submodules. CMake handles downloading and versioning. Dependencies are built as part of the project but kept separate in the build folder.
- **Migration:**
  - Remove `libs/` folder content (or keep only truly vendored code that isn't available upstream).
  - Update `libs/CMakeLists.txt` to just include the dependency logic.

## 2. Target-Based Modernization

**Current Status:** Mixed usage. `cli/CMakeLists.txt` uses `include_directories` (global pollution). Targets are not consistently namespaced.
**Recommendation:** Enforce strict target-based usage and namespacing.

- **Action:** In `cli/CMakeLists.txt`, remove `include_directories(${PROJECT_BINARY_DIR}/include/)`.
- **Action:** Ensure `fiction` CLI target links `libfiction` and uses `target_include_directories` if it needs private headers.
- **Action:** Verify `libfiction` interface library propagates all necessary usage requirements (includes, compile features) via `INTERFACE`.
- **Action:** **Namespaced Targets**: Define `add_library(fiction::libfiction ALIAS libfiction)` immediately after defining `libfiction`. Use `fiction::libfiction` internally. This unifies the usage for both in-tree and installed builds (where `find_package(fiction)` would provide `fiction::libfiction`).

## 3. Source Management (Remove Globs)

**Current Status:** Extensive use of `file(GLOB_RECURSE ...)` in `cli`, `experiments`, and `test`.
**Recommendation:** Use explicit source lists.

- **Action:** Replace `file(GLOB_RECURSE ...)` with explicit lists of source files in `cli/CMakeLists.txt`, `experiments/CMakeLists.txt`, and `test/CMakeLists.txt`.
- **Benefit:** CMake knows exactly when to rebuild. Adding a file requires modifying the CMake list, which ensures the build system is always in sync. It prevents "missing file" errors when switching branches.

## 4. Testing Strategy

**Current Status:** One executable per test file (`test/CMakeLists.txt`).
**Recommendation:** Consolidate into a single (or few) test runner executables.

- **Action:** In `test/CMakeLists.txt`, create a single `fiction_tests` executable that links all test sources.
- **Action:** Use `Catch2`'s `catch_discover_tests` (if using CMake integration) or simply register the single executable.
- **Benefit:** Massive reduction in link time. Linking hundreds of small executables is very slow. A single test runner is standard practice for Catch2.

## 5. Installation and Packaging

**Current Status:** No `install()` rules visible. `FILE_SET HEADERS` is present (good start).
**Recommendation:** Implement full installation rules.

- **Action:** Add `install(TARGETS libfiction ...)` in `include/CMakeLists.txt`.
- **Action:** Add `install(TARGETS fiction ...)` in `cli/CMakeLists.txt`.
- **Action:** Generate `fictionConfig.cmake` and `fictionConfigVersion.cmake` using `CMakePackageConfigHelpers`.
- **Action:** Install headers (already partially handled by `FILE_SET`, just need the `install` command).
- **Benefit:** Allows `find_package(fiction)` from other projects. Enables package managers (Debian, Homebrew, vcpkg) to package `fiction`.

## 6. CMake Presets

**Current Status:** None.
**Recommendation:** Add `CMakePresets.json`.

- **Action:** Create `CMakePresets.json` at the root.
- **Action:** Define presets for `dev` (Debug, ASan), `release` (Release, LTO), `ci`, etc.
- **Benefit:** Standardizes build commands across IDEs (VS Code, CLion, Visual Studio) and CLI. "Works on my machine" becomes "Works with the preset".

## 7. Refactoring and Cleanup

**Current Status:** Duplicated `strip` logic.
**Recommendation:** Centralize utility functions.

- **Action:** Move the `strip` command logic to a function `fiction_strip_target(target_name)` in `cmake/Utilities.cmake`.
- **Action:** Call this function in `cli` and `experiments`.

## 8. Folder Structure Suggestions

- **`libs/`**: Rename to `third_party/` or remove if fully switching to FetchContent.
- **`cmake/`**: Keep as is, it's well structured.

## 9. Root `CMakeLists.txt` Improvements

**Current Status:** Generally good, but can be tightened.
**Recommendation:** Refine variable scoping and future-proof.

- **Action:** Bump `cmake_minimum_required` to `3.25...4.2` to enable latest policies.
- **Action:** Remove global `set(CMAKE_CXX_VISIBILITY_PRESET hidden)`. Instead, apply it to `fiction_options` using `target_compile_options` with generator expressions (e.g., `-fvisibility=hidden` for GCC/Clang) to avoid breaking third-party dependencies that expect default visibility.
- **Action:** Group `option()` calls at the top or in `fiction_setup_options` (which is already done, but some are still in the root). Consolidate all options in `cmake/ProjectOptions.cmake` if possible.
- **Action:** Remove `set(CMAKE_CXX_STANDARD 17)` if `target_compile_features` is sufficient, or keep it as a fallback but ensure it doesn't override user intent (the current `if(NOT DEFINED)` is correct).
- **Action:** **Cleanup**: Remove the debug `message("all_targets=${all_targets}")` line.
- **Action:** **Cleanup**: Remove or fix `set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT intro)` as target `intro` does not exist. Change to `fiction` if desired.

## 10. CMake 4+ Usage

**Context:** With CMake 4.2 available, we should leverage the latest policies and features.
**Recommendation:**

- **Action:** Bump `cmake_minimum_required` to `3.25...4.2` to enable the latest policies by default.
- **Action:** Audit for any new deprecations introduced in the 4.x series.
- **Action:** Use `block()` (introduced in 3.25, standard in 4.x) for variable scoping.
- **Action:** Check for new 4.x specific features for dependency handling or presets (if applicable).

## Implementation Steps for LLM

1.  **Create `CMakePresets.json`**: Define standard build configurations.
2.  **Refactor `libs/CMakeLists.txt`**: Switch to `FetchContent` for at least one dependency to demonstrate the pattern, then apply to others.
3.  **Fix `cli/CMakeLists.txt`**: Remove `include_directories`, use explicit sources.
4.  **Fix `test/CMakeLists.txt`**: Consolidate into one test runner.
5.  **Add `install()` rules**: Make the project installable.
6.  **Update Root `CMakeLists.txt`**: Apply visibility and version changes.
