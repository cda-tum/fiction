# Getting started

The _fiction_ framework provides a stand-alone CLI tool as well as a C++20 header-only library and a Python module which
can be used in external projects. Additionally, we provide an experimentation playground that can be used to quickly
prototype new ideas or script evaluations.

We are continuously testing on Ubuntu, macOS, and Windows with multiple compilers and various Python versions.
See the badges in the README file for more information.

CI builds every commit in Debug and Release with the following combinations. Any C++20 compiler should work; these are
the ones we verify.

| Platform                     | Compilers                                    |
| ---------------------------- | -------------------------------------------- |
| Ubuntu 22.04 (x86-64)        | GCC 11                                       |
| Ubuntu 24.04 (x86-64)        | GCC 13, GCC 14, Clang 18, Clang 19, Clang 20 |
| Ubuntu 24.04 (ARM64)         | Clang 19, Clang 20                           |
| macOS 15 (ARM64)             | Apple Clang                                  |
| Windows Server 2025 (x86-64) | MSVC `v143`, `ClangCL`                       |

## Quick Start

To help you getting started with _fiction_, pick the interface that best fits your use case:

| Goal                                   | Recommended Path          | Section                                  |
| -------------------------------------- | ------------------------- | ---------------------------------------- |
| Try the tool immediately               | 🐳 Docker CLI image       | {ref}`CLI (Docker) <cli-docker>`         |
| Full-featured local CLI build          | 💻 Native build           | {ref}`CLI (Source) <cli-source>`         |
| Integrate into a C++ project           | 📚 Header-only library    | {ref}`C++ Library <header-only>`         |
| Script / notebooks / rapid prototyping | 🐍 Python bindings (PyPI) | {ref}`Python Bindings <python-bindings>` |

For a full CLI command list or API reference, see the respective documentation sections.

<span id="cli"></span>

(cli-docker)=

## CLI (Docker)

This is the fastest zero-install path. We release pre-built images of the latest CLI
on [Docker Hub](https://hub.docker.com/r/mawalter/fiction). Make sure you
have [Docker installed](https://docs.docker.com/get-docker/) on your local system.

Pull the latest image:

```console
$ docker pull mawalter/fiction:latest
```

Run the interactive CLI session:

```console
$ docker run --rm -it mawalter/fiction
```

Internally, the repository lives at `/app/fiction`.

(cli-source)=

## CLI (Source)

When you want to add your own algorithms or contribute to the project, you should build _fiction_ from source.

### Compilation requirements

The repository should always be cloned recursively with all submodules:

```console
$ git clone --recursive https://github.com/cda-tum/fiction.git
$ cd fiction
```

Several third-party libraries will be cloned within the `libs` folder. The `cmake` build process will take care of
them automatically. Should the repository have been cloned before, the commands:

```text
git submodule update --init --recursive
```

will fetch the latest version of all external modules used. Additionally, only `CMake` and a C++20 compiler are
required for the C++ part. If you want to work with the Python bindings, you need a Python 3.10+ installation.

At the time of writing, for parallel STL algorithms to work when using GCC, the TBB library (`libtbb-dev` on Ubuntu) is
needed. It is an optional dependency that can be installed for a performance boost in certain scenarios. For your
preferred compiler, see the current implementation state of [P0024R2](https://en.cppreference.com/w/cpp/compiler_support/17).

On Ubuntu, all required and optional dependencies can be installed via:

```text
sudo apt-get install build-essential cmake python3 libreadline-dev libtbb-dev
```

### Building the CLI

For auto-completion in the CLI, it is recommended but not required to install the `libreadline-dev` package (see above).

Configure and build with CMake:

```console
$ cmake -S . -B build
$ cmake --build build --parallel
```

Several options can be toggled during the build. For a more interactive interface, please refer to `ccmake` for a
full list of supported customizations.

(cmake-presets)=

### CMake Presets

The repository ships a [CMakePresets.json](https://github.com/cda-tum/fiction/blob/main/CMakePresets.json) with a
curated set of configurations for common tasks, so that you do not have to remember all relevant `-D` flags
yourself. List them with:

```console
$ cmake --list-presets
```

Noteworthy presets include `dev` (a quick Debug build with only the CLI and tests enabled), `dev-full` (the same,
but with Z3 and ALGLIB also enabled), `dev-asan` (`dev` with sanitizers), `tests-slim`/`tests-full`
(test-only builds, without/with all optional components, for the fastest edit-compile-test loop), `pyfiction`
(mirrors the `pyproject.toml` configuration for iterating on the Python bindings directly with CMake), and
`release` (an optimized, IPO-enabled build). The `ci-*` and `coverage` presets provide the shared baseline
configuration used by the corresponding GitHub Actions workflows; each job layers a few compiler- and
platform-specific `-D` overrides on top, so reproducing a specific failing job locally may require adding those
too, e.g.:

```console
$ cmake -S . --preset ci-debug
$ cmake --build --preset ci-debug
$ ctest --preset ci-debug
```

Any preset can still be combined with additional `-D` overrides on the command line.

Run the CLI:

```console
$ build/cli/fiction
```

Here is an example of running _fiction_ to perform a full physical design flow on a QCA circuit layout that can
afterward be simulated in QCADesigner:

:::{figure} /_static/fiction_cli_example.gif
:align: center
:alt: CLI example
:::

See {ref}`cli` for a full user guide.

(header-only)=

## C++ Library

If you want to use _fiction_ as a dependency in your project to utilize its header-only library for your own tool.

Add _fiction_ as a sub-directory to your CMake project and link against `libfiction` (assuming your project is
called _fanfiction_):

```cmake
add_subdirectory(fiction)
target_link_libraries(fanfiction PRIVATE libfiction)
```

:::{note}
The command `target_link_libraries` must be called after the respective `add_executable` statement that defines
`fanfiction`.

By default _fiction_'s CLI is enabled and will be built, which can be time-consuming. If you do not need it, you can
disable it by passing `-DFICTION_CLI=OFF` to your `cmake` call or adding
`set(FICTION_CLI OFF CACHE BOOL "" FORCE)` **before** `add_subdirectory(fiction/)`.
:::

Then include what you need:

```c++
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/technology/qca/io/write_qca_layout.hpp>
#include <fiction/...>
```

Symbols live in namespaces that mirror the directory tree, so a header under
`fiction/technology/sidb/simulation/engines/` declares into
`fiction::sidb::simulation::engines`. `fiction/technology/` itself adds no namespace
level: its subdirectories map straight to `fiction::fcn`, `fiction::qca`,
`fiction::inml`, and `fiction::sidb`.

(python-bindings)=

## Python Bindings

Ideal for notebooks, exploratory scripts, and integration with Python tooling.

Install the library from PyPI:

```console
$ pip install mnt.pyfiction
```

Import it in your script:

```python
from mnt import pyfiction
```

The Python synopsis is modeled after the C++ API to make it feel as familiar as possible. However, all available Python
bindings are additionally documented together with the C++ code on this site to make it easier to get started. For each
module, you can toggle between the two languages using the tabs.

:::{note}
The _fiction_ framework is primarily developed for C++ as a header-only library. The Python bindings are a thin
wrapper around the C++ code. We try our best to keep the bindings in sync with the C++ code, and to expose most of
_fiction_'s functionality in both C++ and Python. This is, unfortunately, not always possible. Should you encounter
features that are not (yet) available in _pyfiction_, please open
an [issue on GitHub](https://github.com/cda-tum/fiction/issues).
:::

### Virtual Environment Setup

In order to set up a virtual environment on UNIX-like systems, you can use the following commands:

```console
$ python3 -m venv venv
$ source venv/bin/activate
```

If you are using Windows, you can use the following commands instead:

```console
$ python3 -m venv venv
$ venv\Scripts\activate.bat
```

(bindings-architecture)=

### Bindings Architecture

If you want to add or extend Python bindings, the code under `bindings/mnt/pyfiction/` follows a source-based
layout, one translation unit per binding, chosen to keep compile time and memory usage manageable as the number of
bindings grows:

```text
bindings/mnt/pyfiction/
├── CMakeLists.txt
├── pyfiction.cpp                                  # top-level NB_MODULE entry point
└── src/pyfiction/
    ├── physical_design/
    │   ├── register_physical_design.cpp           # calls exact(m), orthogonal(m), ...
    │   └── path_finding/
    │       ├── a_star.cpp                         # defines a_star(nanobind::module_&)
    │       └── register_path_finding.cpp          # calls a_star(m), distance(m), ...
    ├── technology/sidb/simulation/engines/
    │   ├── quickexact.cpp                         # defines quickexact(nanobind::module_&)
    │   └── register_sidb_simulation_engines.cpp   # calls quickexact(m), quicksim(m), ...
    └── ...
```

The tree mirrors `include/fiction/`: a binding sits in the directory of the header it wraps, so
`a_star.cpp` is under `physical_design/path_finding/` because `a_star.hpp` is. Each leaf `.cpp` file defines
exactly one binding function named after the file (e.g. `void a_star(nanobind::module_& m)`) that binds a
single class, function, or closely related group thereof. Each directory that holds binding sources has exactly one
`register_<path>.cpp`, named after the directory, that forward-declares and calls the binding functions beside it
and nothing else.

The registries are flat: `pyfiction.cpp` calls every one of them from its `NB_MODULE` block, and none is nested
inside another. That order is load-bearing — a type has to be registered before anything names it in a signature or
a default argument — so the block runs the type-defining directories first, then the readers and writers, then the
algorithms built on all of them. New source files do not need to be added anywhere manually: `CMakeLists.txt`
collects them automatically via `file(GLOB_RECURSE FICTION_PYFICTION_SOURCES CONFIGURE_DEPENDS "src/*.cpp")`, so
re-running `cmake` picks up new files on its own — you only need to wire the new function into the directory's
`register_<path>.cpp` and forward-declare it there.

:::{note}
The Python-facing `mnt.pyfiction` namespace must not change shape when adding new bindings. In particular, do
not introduce new Python-level submodules (e.g. `mnt.pyfiction.algorithms`) — all registration functions attach
their bindings to the single top-level module object that is threaded through the call chain, matching the
existing flat API that user scripts depend on.
:::

:::{note}
The bindings are built with [nanobind](https://github.com/wjakob/nanobind), which (unlike the previous
[pybind11](https://github.com/pybind/pybind11)-based setup) is resolved as an installed Python package rather
than fetched by CMake. When configuring the `pyfiction` preset directly (e.g. for IDE-based iteration, outside
of `pip install`), make sure the Python interpreter CMake picks up has `nanobind` installed — the project's
`uv`-managed virtual environment already does, so pass
`-DPython_EXECUTABLE=<path_to_repo>/.venv/bin/python3` (or the equivalent `.venv\Scripts\python.exe` on
Windows) if CMake would otherwise pick up a different interpreter.
:::

:::{note}
nanobind is used in _split mode_: the extension contains no nanobind library code and resolves it at import time
from the separate `nanobind-backend` package, which is therefore a runtime dependency of `mnt.pyfiction`.
That is what lets a single `abi3` wheel per platform serve every supported interpreter. Free-threaded
interpreters are not supported until Python 3.15 gives them a stable ABI
([PEP 803](https://peps.python.org/pep-0803/)); building on an earlier one stops the CMake configure with a
message naming that version.
:::

\---

## Advanced Configuration

### Enabling dependent functions

Some functionalities require the presence of third-party dependencies. In the following, it is discussed how to enable
them.

#### SMT-based `exact` P&R

The {ref}`exact placement and routing algorithm <exact>` utilizes the [SMT solver Z3](https://github.com/Z3Prover/z3).
Follow the [installation instructions](https://github.com/Z3Prover/z3/blob/master/README-CMake.md) and call
`sudo make install` to install headers, scripts, and the binary.

:::{note}
Be sure to compile Z3 in **Release mode** to avoid performance issues when running _fiction_'s dependent functions!
This can be achieved by passing `-DCMAKE_BUILD_TYPE=Release` to Z3's `cmake` call.
:::

Finally, before building _fiction_, pass `-DFICTION_Z3=ON` to the `cmake` call. It should be able to find
Z3's include path and link against the binary automatically if installed correctly. Otherwise, you can use
`-DZ3_ROOT=<path_to_z3_root>` to set Z3's root directory that is to be searched for the installed solver.

(abc-cmake)=

#### ABC callback

[ABC](https://github.com/berkeley-abc/abc/) by Alan Mishchenko can be used as a callback for logic synthesis and
optimization from within the _fiction_ CLI. It must be compiled and installed manually and can be enabled by passing
`-DFICTION_ABC=ON` to the `cmake` call. If ABC is not in your `PATH`, you can specify the path to the folder
where the `abc` binary is located by passing `-DABC_ROOT=<path_to_abc_root>` to the `cmake` call. On the other
hand, if you installed ABC in a default location on UNIX-like operating systems (e.g., `/usr/bin/`
or `/usr/local/bin/`), it should be detected automatically without the need to pass the root directory.

:::{note}
Be sure to compile ABC in **Release mode** to avoid performance issues during synthesis and optimization!
This can be achieved by passing `-DCMAKE_BUILD_TYPE=Release` to ABC's `cmake` call.
:::

For information on usage, see the {ref}`ABC callback <abc-cli>` section in the CLI documentation.

#### ALGLIB-dependent `ClusterComplete` exact SiDB simulation

The {ref}`ClusterComplete <clustercomplete>` exact SiDB simulation algorithm relies on functionality offered by
[ALGLIB](https://www.alglib.net) by the ALGLIB Project.
When enabled, it will be downloaded automatically and linked against _fiction_.

To enable it, before building _fiction_, pass `-DFICTION_ALGLIB=ON` to the `cmake` call.

### Building experiments

The `experiments` folder provides a playground for quickly scripting some ideas by plugging algorithms together.
A `fictionlib_demo.cpp` demonstrates the usage. Any `*.cpp` file that is placed in on of its sub-folders is
automatically linked against `libfiction` and compiled as a stand-alone binary. Simply add a `main` function and
include the desired header files to get started:

```c++
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/technology/qca/io/write_qca_layout.hpp>
#include <fiction/...>

int main(int argc, char* argv[])
{
  // your code goes here
}
```

Each file can be built individually via CMake:

```console
$ cmake -S . -B build -DFICTION_EXPERIMENTS=ON
$ cmake --build build --parallel
```

### Building tests

Unit tests can be built with CMake via a respective flag on the command line and executed via `ctest`:

```console
$ cmake -S . -B build -DFICTION_TEST=ON
$ cmake --build build --parallel
$ ctest
```

### Building code benchmarks

Using `Catch2`'s micro-benchmarking feature, you can compile and run code tests that evaluate the performance of
certain code constructs. The `test/benchmark` folder provides a selection of benchmarks we were running to evaluate
the performance of our code during development. Any `*.cpp` file that is placed in that folder is automatically
linked against _fiction_ and compiled as a stand-alone binary using the following commands:

```console
$ cmake -S . -B build -DFICTION_BENCHMARK=ON
$ cmake --build build --parallel
```

### Noteworthy CMake options

The following CMake options are available which have a potential positive impact on the build process, debugging
attempts, or performance of the resulting binaries:

- `-DFICTION_ENABLE_IPO=ON`: Enable IPO/LTO to improve performance of resulting binaries on some systems.
- `-DFICTION_ENABLE_PCH=ON`: Enable precompiled headers (PCH) for the CLI and the test suite to speed up compilation.
  The `dev` and `tests-slim` presets turn this on. On Windows, add `sloppiness = pch_defines,time_macros` to your
  ccache configuration, or ccache will stop caching the compilations that use the PCH.
- `-DFICTION_LIGHTWEIGHT_DEBUG_BUILDS=ON`: Cut debug information down to `-g1` and disable inlining. This is by far
  the largest single lever on Debug build cost; the CI Debug preset and the `dev`/`tests-slim` presets enable it.
- `-DFICTION_ENABLE_SANITIZER_ADDRESS=ON`: Enable the address sanitizer to detect memory issues.
- `-DFICTION_ENABLE_SANITIZER_LEAK=ON`: Enable the leak sanitizer to detect memory leaks.
- `-DFICTION_ENABLE_SANITIZER_UNDEFINED=ON`: Enable the undefined behavior sanitizer to detect undefined behavior.
- `-DFICTION_ENABLE_SANITIZER_THREAD=ON`: Enable the thread sanitizer to detect multithreading-related problems.
- `-DFICTION_ENABLE_SANITIZER_MEMORY=ON`: Enable the memory sanitizer to detect uninitialized reads.
- `-DFICTION_ENABLE_JEMALLOC=ON`: Enable the usage of jemalloc by Jason Evans to speed up `malloc` in parallelized processes.
- `-DFICTION_ENABLE_TIME_TRACE=ON`: Emit Clang `-ftime-trace` compilation profiles to find out where build time goes.

#### Profiling compilation time

_fiction_ is header-only and template-heavy, so a translation unit's build time is dominated by the headers it pulls in
and the templates it instantiates. `-DFICTION_ENABLE_TIME_TRACE=ON` makes Clang write a `.json` profile next to
every object file. [ClangBuildAnalyzer](https://github.com/aras-p/ClangBuildAnalyzer) aggregates those into a ranking
of the most expensive headers, template instantiations, and functions:

```console
$ cmake -S . --preset tests-slim -DCMAKE_CXX_COMPILER=clang++ -DFICTION_ENABLE_TIME_TRACE=ON
$ cmake --build --preset tests-slim
$ ClangBuildAnalyzer --all build-tests-slim trace.bin
$ ClangBuildAnalyzer --analyze trace.bin
```

The option is Clang-only; it warns and does nothing on GCC and MSVC. Combine it with `-DFICTION_ENABLE_CACHE=OFF`, as
a ccache hit produces no profile.

#### Usage of jemalloc

While enabling jemalloc through the above CMake is not beneficial to every application (and may add runtime due to overhead), it can bring significant runtime improvements to some applications. In particular, it is recommended to use jemalloc for parallelized applications in which allocations are predominantly non-ephemeral.

:::{note}
Windows users need to install jemalloc manually. It can be done by following [these steps](https://github.com/jemalloc/jemalloc/blob/dev/INSTALL.md#building-for-windows).
:::

### Uninstall

Since all tools were built locally, simply delete the git folder cloned initially to uninstall this project.
