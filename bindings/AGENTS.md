# AGENTS.md — `pyfiction` bindings

The Python bindings use **nanobind**, with one translation unit per binding to keep
compile time and memory usage manageable. `docs/getting_started.md` §"Bindings
Architecture" describes the layout in full; read it before adding a binding.

Three trees make up `mnt.pyfiction`:

- `bindings/` holds the C++ sources. Each top-level C++ namespace (`layouts`, `sidb`, …)
  is one extension module, built from `bindings/<namespace>/`.
- `python/mnt/` holds the Python package: the lazy `mnt/pyfiction/__init__.py` and the
  pure-Python CLI in `mnt/fiction/cli/`.
- `test/python/` holds the Python tests, laid out like the Python module tree.

**The Python module tree mirrors the C++ namespaces.** `fiction::sidb::simulation::engines`
is `mnt.pyfiction.sidb.simulation.engines`, built from `bindings/sidb/simulation/engines/`.
The `technology` directory level of `include/fiction/` has no namespace, so it has no
module either.

A new binding:

1. Gets its own `.cpp` file in the directory of its namespace, defining a single
   `void xxx(nanobind::module_& m)` named after the file.
2. Is forward-declared and called in that directory's `register_<path>.cpp`. Every
   directory that holds binding sources has exactly one registry, named after the
   directory (`register_sidb_simulation_engines.cpp`).
3. Needs nothing else if its directory already exists. A new nested namespace also gets a
   `pyfiction::def_submodule` call in the `NB_MODULE` block of its top-level
   `register_<namespace>.cpp`. A new top-level namespace also goes into the module list
   of `bindings/CMakeLists.txt` and `python/mnt/pyfiction/__init__.py`.
4. Names types of other modules only if its `NB_MODULE` block imports them with
   `nanobind::module_::import_("mnt.pyfiction.<module>")`. Keep the imports acyclic: the
   modules form the chain in the list of `bindings/CMakeLists.txt`, and a module imports
   only modules before it.

Never:

- Add source files to a manual list in `CMakeLists.txt`. `file(GLOB_RECURSE ...)` picks
  them up.
- Edit `include/pyfiction/pybind11_mkdoc_docstrings.hpp` by hand. It is generated from the
  Doxygen comments in `include/fiction/`, and keeps its historical name. CI's
  `🐍 Docstrings` job regenerates it and fails when the committed file differs; take the
  replacement from that job's `pyfiction-docstrings` artifact.
