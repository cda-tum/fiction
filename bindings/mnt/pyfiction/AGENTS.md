# AGENTS.md — `pyfiction`

The Python bindings use **nanobind**, with one translation unit per binding to keep
compile time and memory usage manageable. `docs/getting_started.rst` §"Bindings
Architecture" describes the layout in full; read it before adding a binding.

The short version: a new Python-exposed feature gets its own `.cpp` file under
`src/pyfiction/<module>/<submodule>/` defining a single
`void xxx(nanobind::module_& m)`. Forward-declare that function in the enclosing
`register_<name>.cpp` and call it from `register_<name>(m)`, which the
`NB_MODULE(pyfiction, m)` block in `pyfiction.cpp` calls in turn.

**The tree mirrors `include/fiction/`,** so a binding lives beside the header it wraps:
`quickexact.cpp` under `technology/sidb/simulation/engines/`, `write_qca_layout.cpp` under
`technology/qca/io/`. Registries are flat — one per module, all called from `pyfiction.cpp`,
none nested inside another. That call order is load-bearing: a type has to be registered
before anything names it in a signature or a default argument, which is why the block runs
types, then readers and writers, then utilities, then the algorithms built on them.

Never:

- Add bindings through a monolithic header included into `pyfiction.cpp`. That is the old
  pattern and it is gone.
- Introduce new Python-level submodules. The `mnt.pyfiction` namespace shape must stay
  unchanged.
- Add source files to a manual list in `CMakeLists.txt`. `file(GLOB_RECURSE ...)` picks
  them up; just wire the new function into its `register_<name>.cpp`.
- Edit `include/pyfiction/pybind11_mkdoc_docstrings.hpp` by hand. It is generated from the
  Doxygen comments in `include/fiction/`, and keeps its historical name. CI's
  `🐍 Docstrings` job regenerates it and fails when the committed file differs; take the
  replacement from that job's `pyfiction-docstrings` artifact.

## The test suite belongs outside the package

`test/` sits inside `mnt.pyfiction`, and `test/__init__.py` makes it a subpackage of the
package it tests. pytest therefore prepends `bindings/` to `sys.path`, and the source
directory shadows any non-editable install of `mnt.pyfiction`: the tests only run because
`nox -s tests` installs the project editable. A wheel install fails with
`ModuleNotFoundError: No module named 'mnt.pyfiction.pyfiction'`.

`[tool.cibuildwheel] test-sources` already works around it by copying the suite alone,
without the two `__init__.py` files above it.

Move the suite to `test/python/` when the project structure is next reworked. That is what
`mqt-core` does, and it removes the workaround rather than reproducing it. The move is
mechanical — `git mv` of 104 tracked files plus seven references in `pyproject.toml` — and
`test/CMakeLists.txt` globs `*/*.cpp`, so a
Python subdirectory there is inert. `sdist.exclude` and `check-sdist`'s `git-only` both
already list `/test`, so their `/bindings/mnt/pyfiction/test` entries go away with it.
