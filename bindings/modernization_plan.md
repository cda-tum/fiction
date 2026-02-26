# Modernization Plan for Python Bindings

## Goal

Modernize the CMake configuration and file structure of the `bindings` folder to improve compilation performance and maintainability. The current setup relies on a monolithic `pyfiction.cpp` that includes numerous header-only implementation files, leading to serialized and slow builds, and high memory usage (OOM).

## Lessons Learned from `mqt-core`

The `mqt-core` project demonstrates a scalable approach to bindings:

1.  **Source-Based Bindings**: Binding logic is implemented in `.cpp` files, not headers. This isolates template instantiations to single translation units.
2.  **Explicit Registration**: Modules use explicit registration functions (e.g., `void registerAlgorithms(py::module& m)`) called from the main module.
3.  **Modular Structure**: The source tree mirrors the logical structure of the library.

## Strategy

1.  **Split Compilation Units**: Break the monolithic build into multiple `.cpp` files, one for each logical component.
2.  **Source-Based Implementation**: Move binding code from `include/pyfiction/**/*.hpp` to `src/pyfiction/**/*.cpp`.
3.  **Preserve Python API**: Maintain the existing `mnt.pyfiction` namespace structure. Do not introduce Python-level submodules (like `mnt.pyfiction.algorithms`) that would break existing scripts.
4.  **Verification**: Ensure strict testing with `nox` before and after changes.

## Detailed Implementation Steps

### Phase 0: Verification Baseline

Before making any changes, establish a known good state.

- Run `nox -s tests --python 3.12` (or another specific version) to ensure the current bindings build and pass tests.
- **Do not proceed** if the baseline fails.

### Phase 1: Infrastructure Setup

1.  Create the `src` directory structure:
    ```text
    bindings/mnt/pyfiction/
    ├── CMakeLists.txt
    ├── pyfiction.cpp          # Main entry point
    ├── include/
    │   └── pyfiction/         # Minimal headers for forward declarations if needed
    └── src/                   # Implementation files (.cpp)
        └── pyfiction/
            ├── algorithms/
            ├── layouts/
            ├── networks/
            ├── technology/
            ├── utils/
            └── inout/
    ```

### Phase 2: Variable-Size Migration (Heaviest First)

Migrate components starting with the most resource-intensive ones (Algorithms and Layouts) to immediate alleviate OOM pressure.

#### Step 2.1: Algorithms

1.  Create `src/pyfiction/algorithms/register_algorithms.cpp`.
2.  Define `void registerAlgorithms(py::module& m)`.
3.  Move the content of `include/pyfiction/algorithms/**/*.hpp` into this file (or multiple files like `register_path_finding.cpp` if still too large).
4.  **Important**: Keep the Python module attachment flat.
    ```cpp
    // src/pyfiction/algorithms/register_algorithms.cpp
    void registerAlgorithms(py::module& m) {
        // ... binding code ...
        // m is the main 'pyfiction' module
        pyfiction::a_star(m);
    }
    ```
5.  Update `pyfiction.cpp` to call `registerAlgorithms(m)`.

#### Step 2.2: Layouts

Repeat the process for Layouts, creating `src/pyfiction/layouts/register_layouts.cpp`.

### Phase 3: Complete Restructuring

Convert the remaining components (Networks, Technology, InOut, Utils) to the source-based pattern.

### Phase 4: CMake Configuration Updates

Update `bindings/mnt/pyfiction/CMakeLists.txt` to compile separate sources.

```cmake
# Collect sources
file(GLOB_RECURSE PYFICTION_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

pybind11_add_module(pyfiction
    THIN_LTO
    pyfiction.cpp
    ${PYFICTION_SOURCES}
)
```

## Continuous Verification

- After each migration step (e.g., after moving Algorithms), run `nox -s tests --python 3.12` to ensure:
  1.  The project compiles using the new structure.
  2.  The Python API remains unchanged and tests pass.
