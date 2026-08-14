# AGENTS.md — experiments

These programs reproduce the results of published papers. They are build artifacts of the
scientific record, not library code.

- **Do not modernize, refactor, generalize, or reformat an existing experiment.** The
  ongoing C++20 modernization campaign in `include/fiction/` stops at this directory.
  Change an experiment only to fix a compilation break or a genuine correctness bug, and
  say so in the changelog entry.
- `experiments/CMakeLists.txt` globs `*.cpp` and derives the executable name from the file
  base name, so base names must be unique across the whole directory. Do not edit that
  file to register a new experiment.
- Experiments are not built or run in CI. They take minutes to hours. Do not add one to a
  CI workflow.
- `*.json` and `*.csv` are gitignored: those are experiment outputs. Do not force-add
  result files.
