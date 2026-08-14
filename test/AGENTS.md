# AGENTS.md — tests

Catch2 tests. The tree mirrors `include/fiction/`: `algorithms/`, `io/`, `layouts/`,
`networks/`, `technology/`, `utils/`, plus `benchmark/` and shared `resources/`.

## Wiring

- **Test file base names must be unique across the whole tree.** `test/CMakeLists.txt`
  globs `*/*.cpp` and builds `add_executable(test_${NAME})` from the base name alone, so
  two files called `foo.cpp` in different subdirectories collide and CMake fails. The
  existing duplicates (`distance_map.cpp`, `determine_clocking.cpp`, …) are safe only
  because `benchmark/` is filtered out of that glob.
- Do not edit `test/CMakeLists.txt` to register a test. The glob picks it up; re-run
  CMake.
- Load test data through the `TEST_PATH` compile definition, not a relative path.
- Benchmarks go in `test/benchmark/` and use `BENCHMARK`. They are excluded from the test
  glob and do not run in CI.

## Writing tests

- Use `TEST_CASE` for a concrete case and `TEMPLATE_TEST_CASE` to sweep layout and network
  types. Both are the house style; `SCENARIO` is unused, so do not introduce it.
- Test the documented contract. Do not assert on iteration order, tie-breaking, or the
  particular solution a heuristic returns unless the documentation promises it — those
  assertions block the next refactor without protecting a user.
- A bug fix ships with a test that fails before the fix.
- Keep runtime bounded. Every test runs on Linux, macOS, and Windows on every pull
  request.
