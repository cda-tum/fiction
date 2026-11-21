# AGENTS.md — `pyfiction` bindings

Use these instructions for binding sources, public `mnt.pyfiction` module
wiring, stubs, and generated binding docstrings.

The Python bindings use **nanobind**, with one translation unit per binding to keep
compile time and memory usage manageable. `docs/getting_started.md` §"Bindings
Architecture" describes the layout in full; read it before adding a binding.

Three trees make up `mnt.pyfiction`:

- `bindings/` holds the C++ sources. Each top-level C++ namespace (`layouts`, `sidb`, …)
  is one extension module, built from `bindings/<namespace>/`.
- `python/mnt/` holds the Python package: the lazy `mnt/pyfiction/__init__.py`, the
  generated `.pyi` stubs, and the pure-Python CLI in `mnt/fiction/cli/`.
- `test/python/` holds the Python tests, laid out like the Python module tree.

**The Python module tree mirrors the C++ namespaces.** `fiction::sidb::simulation::engines`
is `mnt.pyfiction.sidb.simulation.engines`, built from `bindings/sidb/simulation/engines/`.
The `technology` directory level of `include/fiction/` has no namespace, so it has no
module either.

A new binding:

1. Gets its own `.cpp` file in the directory of its namespace, defining a single
   `void register_<name>(nanobind::module_& m)`, where `<name>` matches the file.
   Internal template helpers use the same `register_` prefix.
2. Is forward-declared and called in that directory's `register_<path>.cpp`. Every
   directory that holds binding sources has exactly one registry, named after the
   directory (`register_sidb_simulation_engines.cpp`).
3. Needs nothing else if its directory already exists. A new nested namespace also gets a
   `pyfiction::def_submodule` call in the `NB_MODULE` block of its top-level
   `register_<namespace>.cpp`. A new top-level namespace also goes into the module list
   of `bindings/CMakeLists.txt`, `python/mnt/pyfiction/__init__.py`, and `nox -s stubs`.
4. Names types of other modules only if its `NB_MODULE` block imports them with
   `nanobind::module_::import_("mnt.pyfiction.<module>")`. Keep the imports acyclic: the
   modules form the chain in the list of `bindings/CMakeLists.txt`, and a module imports
   only modules before it.
5. Translates the C++ exceptions its own code throws in its own module. A translator catches
   an exception only when it shares the module that threw it: each extension carries its own
   copy of the exception's type information, and macOS does not match the copies. An
   exception whose Python class lives in another module is raised through that class, as
   `physical_design` does for `high_degree_fanin_exception`.
6. Comes with regenerated stubs. Run `nox -s stubs` and commit the `.pyi` changes. CI
   fails when the committed stubs differ from the generated ones. Where stubgen cannot infer a
   type, state it at the binding with `nanobind::sig`, or `nanobind::for_getter` and
   `nanobind::for_setter` on a property.

## Build discovery and generated files

- Let `file(GLOB_RECURSE ...)` discover binding sources.
- Fix bindings or docstrings at the source and regenerate the `.pyi` files. Keep
  manual edits and post-processing out of the generated stubs.
- Get the generated `include/pyfiction/pybind11_mkdoc_docstrings.hpp` from the
  `pyfiction-docstrings` artifact when the `🐍 Docstrings` job reports a
  mismatch. The job generates the header from Doxygen comments in
  `include/fiction/`.
