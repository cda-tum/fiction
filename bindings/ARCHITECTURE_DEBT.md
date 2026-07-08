# Bindings Architecture: Deferred Debt Notes

Observations surfaced while restructuring `bindings/mnt/pyfiction/` into per-module translation units
(see the "Bindings Architecture" section in `docs/getting_started.rst` for the resulting, current
architecture). These are explicitly **not** being acted on as part of that restructuring PR — captured
here so they aren't lost, for a later pass.

- **`file(GLOB_RECURSE ... CONFIGURE_DEPENDS)` as the long-term source-collection strategy.**
  `bindings/mnt/pyfiction/CMakeLists.txt` collects all binding translation units via
  `file(GLOB_RECURSE FICTION_PYFICTION_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/bindings/mnt/pyfiction/src/*.cpp")`.
  `CONFIGURE_DEPENDS` mitigates the classic "new file not picked up" problem, but it's a build-system
  hint honored on a best-effort basis by the generator (not guaranteed by CMake docs), and GLOB-based
  source lists are known to have rough edges at scale: some IDEs / generators (older Ninja/Make setups,
  some cross-compilation or sandboxed CI environments) don't reliably re-run CMake's configure step on
  a new file appearing, and glob-based lists make "what's actually compiled" harder to audit at a
  glance than an explicit list. Worth revisiting (e.g. an explicit source list, or a generated one
  checked in) once the binding tree stabilizes and stops growing quickly.

- **Norm for agent-authored scratch/planning docs landing in the tree.**
  `bindings/modernization_plan.md` was a working-notes file written by an earlier agent session to plan
  this very restructuring. It was never linked from the Sphinx doc build and, once the migration it
  described was complete, read as stale internal scratch content rather than living documentation — it
  has now been folded into real docs and deleted. There's currently no written norm in `AGENTS.md` (or
  CI check) preventing a future agent from dropping a similar planning doc into the tree and forgetting
  to clean it up. Worth deciding on a convention — e.g. explicitly disallow committing standalone
  planning/scratch markdown outside of PR descriptions, or require any such file to be temporary
  (`.gitignore`'d) rather than committed.

- **CI `clang-tidy` pipeline (`cpp-linter-action`) not reliably honoring inline `NOLINT` suppressions.**
  Observed during this PR's review; a parallel session is fixing the underlying Clang-Tidy warnings and
  investigating the specifics. Flagging here only as a known rough edge in the CI setup worth a
  dedicated look later — not duplicating that investigation.

- **Inconsistent wiring depth for `register_sidb_support`.**
  `bindings/mnt/pyfiction/pyfiction.cpp` calls `pyfiction::register_sidb_support(m)` directly from the
  top-level `PYBIND11_MODULE` block, alongside `register_layouts`, `register_networks`,
  `register_technology`, `register_inout`, `register_utils`, and `register_algorithms`. But the actual
  function lives three directories deep, in
  `bindings/mnt/pyfiction/src/pyfiction/algorithms/simulation/sidb/register_sidb_support.cpp` — i.e. it
  is physically nested under `algorithms/simulation/sidb/` but wired as if it were a top-level module,
  bypassing the intermediate `register_algorithms(m)` / `register_simulation(m)` chain that every other
  file under `algorithms/` goes through. Either the file should move to reflect its top-level wiring, or
  the wiring should go through `register_algorithms` → `register_simulation` to match its location on
  disk.

- **Leftover migration-era comments in `register_algorithms.cpp`.**
  `bindings/mnt/pyfiction/src/pyfiction/algorithms/register_algorithms.cpp:14` and `:25` still read
  `// ... add others as we migrate them` and `// ... call others`, respectively. The migration these
  comments refer to is complete (all algorithm sub-categories are already registered), so the comments
  are now stale and should be removed in a follow-up cleanup pass.

- **A couple of bindings have their Doxygen-sourced docstrings disabled with a `TODO`.**
  `bindings/mnt/pyfiction/src/pyfiction/utils/placement_utils.cpp:38` and
  `bindings/mnt/pyfiction/src/pyfiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.cpp:23`
  both have a commented-out `DOC(...)` docstring argument (`// TODO, DOC(fiction_place)` and
  `// TODO DOC(fiction_occupation_probability_non_gate_based)`), meaning those two Python-bound
  functions currently ship without the auto-generated docstring that most other bindings get via
  `include/pyfiction/documentation.hpp`. Worth a follow-up to find out why the macro didn't resolve for
  these two (likely a name mismatch with the Doxygen-generated identifier) and wire them up properly.
