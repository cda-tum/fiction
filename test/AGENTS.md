# AGENTS.md — tests

Catch2 tests. The tree mirrors `include/fiction/` exactly: `layouts/`, `networks/`,
`synthesis/`, `physical_design/`, `verification/`, `technology/` (with `fcn/`, `qca/`,
`inml/`, and the `sidb/` subtree), and `utils/`, plus `benchmark/`, shared `resources/`,
and the blueprints under `utils/blueprints/`. A test lives at the path its header does,
so `technology/sidb/model/charge_state.hpp` is tested by
`test/technology/sidb/model/test_charge_state.cpp`.

## Wiring

- **Test files are named `test_<header>.cpp`** and mirror their header's path, so
  `technology/sidb/model/charge_state.hpp` is tested by
  `test/technology/sidb/model/test_charge_state.cpp`. The `test_` prefix is part of the
  file name; CMake no longer synthesizes it.
- **Test file base names must be unique across the whole tree.** `test/CMakeLists.txt`
  globs `*/*.cpp` and builds `add_executable()` from the base name alone, so two files
  with the same name in different subdirectories collide and CMake fails. The `test_`
  prefix also keeps them clear of `benchmark/`, which is filtered out of that glob and
  whose files are unprefixed.
- Do not edit `test/CMakeLists.txt` to register a test. The glob picks it up; re-run
  CMake.
- Load test data through the `TEST_PATH` compile definition, not a relative path.
- Include the shared helpers as `"utils/blueprints/..."`, without a `../` prefix:
  `test/CMakeLists.txt` puts `test/` on the include path. Benchmarks are the exception
  and do need `../`, because `add_subdirectory(benchmark)` runs before that
  `include_directories(.)` call.
- Benchmarks go in `test/benchmark/` and use `BENCHMARK`. They are excluded from the test
  glob; `test/benchmark/CMakeLists.txt` registers them with CTest only when
  `FICTION_BENCHMARK` is on, which the CI presets leave off.

## Writing tests

- **Open the namespaces the file uses.** `using namespace fiction;` alone shortens nothing
  below the top level, and the tree is three levels deep in places. Add one directive per
  namespace the file draws symbols from, general to specific, so a SiDB engine test opens

  ```cpp
  using namespace fiction;
  using namespace fiction::sidb::model;
  using namespace fiction::sidb::simulation;
  using namespace fiction::sidb::simulation::engines;
  ```

  and writes `quickexact(lyt, params)` rather than
  `sidb::simulation::engines::quickexact(lyt, params)`.
  - `fiction::layouts::coords` and `fiction::layouts::clocking` are never opened: bare
    `offset`, `cube`, `siqad`, and `scheme` say nothing. Open `fiction::layouts` instead and
    write `coords::offset` and `clocking::scheme`. `detail` namespaces are never opened.
  - Opening a namespace can collide with a local alias or variable -- `using lattice = ...`
    against `sidb::surfaces::lattice`, or a variable named `defect` against
    `sidb::model::defect`. Rename the local one; the compiler names both candidates.
  - The blueprints under `utils/blueprints/` stay fully qualified. A directive inside
    `namespace blueprints` reaches every translation unit that reopens it, and nearly every
    test does.

- Use `TEST_CASE` for a concrete case and `TEMPLATE_TEST_CASE` to sweep layout and network
  types. Both are the house style; `SCENARIO` is unused, so do not introduce it.
- Test the documented contract. Do not assert on iteration order, tie-breaking, or the
  particular solution a heuristic returns unless the documentation promises it — those
  assertions block the next refactor without protecting a user.
- A bug fix ships with a test that fails before the fix.
- Keep runtime bounded. Every test runs on Linux, macOS, and Windows on every pull
  request.
