# AGENTS.md

_fiction_ is a header-only C++20 library with Python bindings for field-coupled
nanocomputing design automation.

## Scope and worktrees

- Complete the requested change. Include directly related fixes and simplifications
  that you can verify with the same checks. Put those cleanups in separate commits.
  Report unrelated findings and their impact. Ask before a redesign. Keep dependency
  updates in their own PRs.
- For a read-only review or audit, report findings and evidence. Edit or publish
  only when the maintainer authorizes that work. For contract audits, follow
  [SpecAudit](.agent/AUDITS.md) and keep notes in ignored `.ai/audits/`.
- Inspect the working tree before editing. Preserve existing changes and pause if
  they overlap with yours. Make edits only in an isolated, task-owned worktree.
  Reuse one already provided for the task or create one with
  `git worktree add .ai/worktrees/<task> -b <branch>`.
- Keep plans and notes in ignored `.ai/`. Use one CMake preset per worktree when a
  build is needed. Preserve your changes in a commit or deliver them before removing
  a worktree you created with `git worktree remove <path>` and `git worktree prune`.
- Ask before adding a third-party dependency or making a major build configuration
  change. Leave existing `vendors/` sources and generated files untouched by hand.
  Keep secrets and large binaries out of commits.

## Scoped instructions

Read the applicable file when your tool has not loaded it. A task can need more
than one file.

| Work                                                                                           | Instructions                         |
| ---------------------------------------------------------------------------------------------- | ------------------------------------ |
| Nanobind sources, public `mnt.pyfiction` module wiring, stubs, or generated binding docstrings | [Bindings](bindings/AGENTS.md)       |
| Tests                                                                                          | [Tests](test/AGENTS.md)              |
| Documentation or changelog                                                                     | [Docs](docs/AGENTS.md)               |
| Published reproductions and other C++ experiments                                              | [Experiments](experiments/AGENTS.md) |

## Code and public contracts

- Trace the affected behavior and callers. Fix bugs at their source. Reuse
  _fiction_, the standard library, `mockturtle`, or `kitty` where they already solve
  the problem. Add an abstraction or configuration option when a concrete use
  requires it. Note the limit and upgrade path for a deliberate shortcut with a
  known ceiling.
- Validate inputs at public boundaries and handle failures that could silently
  produce a wrong answer. Add a runnable contract test for new functionality or
  changed nontrivial logic. A bug fix needs a regression test that fails before
  the fix. Mechanical and prose-only changes need behavioral tests only if they
  change a testable contract.
- When public behavior changes, document its scope, defaults, limits, failure
  modes, and how to check it. Record user-facing changes in
  [Unreleased](docs/changelog.md#unreleased) using the [changelog rules](docs/AGENTS.md#the-changelog).
- Remove debug output and scaffolding before handoff. Use a check-specific
  `NOLINT(check-name)` only when necessary, with a comment explaining the reason.

## Code and writing style

- Use `.hpp` with `#pragma once`, const correctness, and braced initialization.
  Qualify names in headers and bindings. In test and experiment `.cpp` files, open
  the namespaces used, from `fiction` to the most specific. Write
  `clocking::scheme` and avoid opening `fiction::layouts::clocking`, `detail`, or
  `std`.
- Document every new or edited C++ symbol with `/** ... */` and appropriate Doxygen
  commands. Start C++ files with the `license-tools` copyright block at offset zero
  followed by the Doxygen file block. Let the hook manage copyright text. Follow
  the [file header convention](docs/contributing.md#c-file-headers) for authors and
  the generated-header exception.
- Use `snake_case` for Python functions and variables, `PascalCase` for new classes,
  type hints on all function arguments and returns, and Google-style docstrings.
- Use short, direct sentences and established terms such as SiDB, defect, gate
  library, and operational domain. Match the project's capitalization. Comments,
  docs, and docstrings describe current behavior and constraints. The changelog
  records what changed and what users need to do.

## Verification and handoff

- For C++ changes, use a [preset](docs/getting_started.md#cmake-presets) that enables
  the affected features. Start with `debug` for local tests. Use `ci-debug` for
  tests that need Z3 or ALGLIB, and `experiments` for experiment executables.
  Configure with
  `cmake -S . --preset <preset>`, build the affected target with
  `cmake --build --preset <preset> --target <target> --parallel`, and run the affected
  tests with `ctest --preset <preset> --output-on-failure -R <pattern>`. Confirm the
  intended tests ran. Broaden the run when the change affects shared behavior.
- For binding changes, build and test the changed extension with `nox -s tests`.
  For pure-Python changes, use targeted `pytest` when a compatible extension is
  installed. Use `nox -s minimums` for supported dependency floors and
  `nox -s lint` for Python linting. See the contributor guide for
  [Python test selection](docs/contributing.md#selecting-python-tests) and
  [Clang-Tidy](docs/contributing.md#c-static-analysis).
- Run `prek run -a` before handing off changed files. Use `uvx prek run -a` if
  `prek` is not installed. Fix failures introduced by the change and rerun the
  affected checks. Report any check you could not run or make pass. For read-only
  work, verify findings against the relevant source and report the evidence.

## Commits and pull requests

- Start each commit subject and PR title with one plain gitmoji. Add
  `Assisted-by: <Model Name> via <Tool Name>` to AI-authored commits. Disclose AI
  assistance in every public body an agent drafts or edits. Describe any human
  verification accurately.
- For authorized PR work, follow the [PR template](.github/pull_request_template.md)
  and the contributor guide's
  [PR workflow](docs/contributing.md#pull-request-workflow) and
  [review procedures](docs/contributing.md#code-review). Follow required checks
  and address review feedback before declaring the PR complete. Leave thread
  resolution to reviewers.
