# AGENTS.md — `pyfiction` bindings

The Python bindings use **nanobind**, with one translation unit per binding to keep
compile time and memory usage manageable. `docs/getting_started.md` §"Bindings
Architecture" describes the layout in full; read it before adding a binding.

Three trees make up `mnt.pyfiction`:

- `bindings/` holds the C++ sources. Each top-level C++ namespace (`layouts`, `sidb`, …)
  is one extension module, built from `bindings/<namespace>/`.
- `python/mnt/` holds the Python package: the lazy `mnt/pyfiction/__init__.py`, the
  generated private `.pyi` stubs, typed public facades, and the pure-Python CLI in `mnt/fiction/cli/`.
- `test/python/` holds the Python tests, laid out like the Python module tree.

The private `mnt.pyfiction._native` tree mirrors the C++ namespaces. Public Python
modules under `python/mnt/pyfiction/` select and adapt the supported FCN tools.
Native modules import only native dependencies; they never import the public facades.
The CLI and user examples import only public modules.

A new binding:

1. Gets its own `.cpp` file in the directory of its namespace, defining a single
   `void xxx(nanobind::module_& m)` named after the file.
2. Is forward-declared and called in that directory's `register_<path>.cpp`. Every
   directory that holds binding sources has exactly one registry, named after the
   directory (`register_sidb_simulation_engines.cpp`).
3. Needs nothing else if its directory already exists. A new nested namespace also gets a
   `pyfiction::def_submodule` call in the `NB_MODULE` block of its top-level
   `register_<namespace>.cpp`. A new top-level namespace also goes into the module list
   of `bindings/CMakeLists.txt` and `nox -s stubs`. Public exports are explicit and
   belong to the domain module that provides the user-facing operation.
4. Names types of other modules only if its `NB_MODULE` block imports them with
   `nanobind::module_::import_("mnt.pyfiction._native.<module>")`. Keep the imports acyclic: the
   modules form the chain in the list of `bindings/CMakeLists.txt`, and a module imports
   only modules before it.
5. Translates the C++ exceptions its own code throws in its own module. A translator catches
   an exception only when it shares the module that threw it: each extension carries its own
   copy of the exception's type information, and macOS does not match the copies. An
   exception whose Python class lives in another module is raised through that class, as
   `physical_design` does for `high_degree_fanin_exception`.
6. Comes with regenerated stubs: run `nox -s stubs` and commit the `.pyi` changes. CI
   fails when the committed stubs differ from the generated ones. Where stubgen cannot infer a
   type, state it at the binding with `nanobind::sig`, or `nanobind::for_getter` and
   `nanobind::for_setter` on a property; `pyfiction/progress.hpp` does so for the progress
   callbacks.

Never:

- Add source files to a manual list in `CMakeLists.txt`. `file(GLOB_RECURSE ...)` picks
  them up.
- Edit the `.pyi` files by hand, or rewrite them in `nox -s stubs`. Fix the binding, or its
  docstring, and regenerate.
- Edit `include/pyfiction/pybind11_mkdoc_docstrings.hpp` by hand. It is generated from the
  Doxygen comments in `include/fiction/`, and keeps its historical name. CI's
  `🐍 Docstrings` job regenerates it and fails when the committed file differs; take the
  replacement from that job's `pyfiction-docstrings` artifact.
