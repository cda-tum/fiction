# AGENTS.md — experiments

These programs reproduce the results of published papers. They are build artifacts of the
scientific record, so their results are fixed even though their code is not.

- The C++20 modernization campaign in `include/fiction/` covers this directory too.
  Modernize, refactor, and reformat an experiment as you would library code.
- **Never change what an experiment computes.** An experiment that reproduces a published
  result keeps producing that result: same parameters, same inputs, same output columns.
- Record a change to an experiment under `Experiments` in `docs/changelog.rst`.
- `experiments/CMakeLists.txt` globs `*.cpp` and derives the executable name from the file
  base name, so base names must be unique across the whole directory. Do not edit that
  file to register a new experiment.
- The `🧪 Experiments` job in `.github/workflows/cpp-experiments.yml` builds every
  experiment on every pull request that change detection classifies as touching the C++
  tree, and that job is what catches a refactor which breaks the build. It compiles with
  g++-14 in Debug on Ubuntu and is the only place experiments are built, so a break
  specific to another compiler or platform still ships unnoticed. Iterate locally rather
  than waiting on it: `cmake -S . --preset dev-full` enables the experiments, then
  `cmake --build --preset dev-full --target <base name>`.
- CI never runs an experiment — a run takes minutes to hours — so nothing checks that a
  result stayed the same. That check is yours. Never add an experiment to another
  workflow: the test matrix sets `FICTION_EXPERIMENTS: OFF` on purpose, because building
  them in every matrix entry cost time and caught nothing the one job does not.
- The `🚨 Lint` check lints a changed experiment as well: it configures with `ci-tidy`,
  which sets `FICTION_EXPERIMENTS: ON`, and `experiments/` is not in its ignore list.
  `cpp-linter` reports the whole file, not just your diff, so editing a long-untouched
  experiment surfaces its entire backlog at once.
- `*.json` and `*.csv` are gitignored: those are experiment outputs. Do not force-add
  result files.
