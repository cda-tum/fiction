# Legacy C++ CLI

The shipped Python shell lives in `bindings/mnt/pyfiction/cli/`. Add Python commands through its existing
registry and cover them in `bindings/mnt/pyfiction/test/cli/`; update `docs/cli.md`. Native binding changes follow
`bindings/mnt/pyfiction/AGENTS.md`. This directory retains the legacy alice CLI during the parity transition.
The source-list rules below apply only to that legacy implementation.

The `fiction` CLI is built on [alice](https://github.com/msoeken/alice). Commands are
grouped into `cmd/<category>/`.

**Unlike `test/`, `experiments/`, and the Python bindings, this subtree does not glob.**
Every source and header is listed by hand, so a new command that compiles locally can
still be missing from the build if you skip a step.

To add a command:

1. Add `cmd/<category>/include/<name>.hpp` and `cmd/<category>/src/<name>.cpp`.
2. Register it in `cmd/<category>/cmd_<category>.hpp` with
   `ALICE_ADD_COMMAND(<name>, FICTION_CLI_CATEGORY_<CATEGORY>)`.
3. Add the `.cpp` to the first `target_sources(fiction PRIVATE ...)` list in
   `cmd/<category>/CMakeLists.txt`, **and** the `.hpp` to the second `target_sources(...)`
   `FILE_SET HEADERS FILES` list in the same file. Both lists are required — the first
   builds the source, the second registers the header for IDE integration and install
   metadata.
4. Document the command in `docs/cli.md`.

A new category additionally needs an `add_subdirectory(cmd/<category>)` line in
`cli/CMakeLists.txt`.
