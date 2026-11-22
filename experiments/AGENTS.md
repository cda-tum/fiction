# AGENTS.md — experiments

Experiments reproduce published results. Preserve each existing reproduction's
inputs, parameters, output columns, and reported results when changing its code.
Record experiment changes under `Experiments` in `docs/changelog.md`.

- Give each `.cpp` file a unique basename across `experiments/`. CMake discovers
  files recursively and uses their basenames as executable names.
- Configure with `cmake -S . --preset dev-full` and build the affected executable
  with `cmake --build --preset dev-full --target <basename>`.
- The [Experiments workflow](../.github/workflows/cpp-experiments.yml) builds the
  executables. It does not run them. Compare the affected experiment's output
  with the baseline or published result for the same inputs and parameters.
  Check the output columns and the reported values. Use the published comparison
  tolerance when one is defined. Report what you verified and any run you could
  not complete.
- Keep experiments out of the test matrix. The dedicated workflow builds them,
  and the Clang-Tidy job checks changed experiment sources.
- Keep generated `.json` and `.csv` outputs untracked.
