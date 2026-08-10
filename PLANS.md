# PLANS.md

This file defines how to use repository-local execution plans for long-running or interruption-prone work in
`fiction`. It is adapted from OpenAI's Codex exec-plan guidance:
https://developers.openai.com/cookbook/articles/codex_exec_plans

## When to Use an Exec Plan

Create or update an exec plan when a task is likely to require more than one focused pass, spans multiple areas of the
repository, depends on PR or CI feedback, or needs careful sequencing. Typical examples:

- merging `main` into a feature branch with conflicts;
- addressing reviewer comments across C++, Python bindings, docs, and tests;
- debugging CI failures whose cause is not obvious from the first log;
- moving APIs, bindings, or CMake structure in a way that touches multiple modules;
- implementing user-facing behavior that needs tests and changelog updates.

For small single-file fixes, an exec plan is optional. Prefer direct implementation plus normal validation.

## Core Rules

- Keep the plan in the repository so another developer or Codex session can resume from it.
- Treat the plan as live state, not as a one-time proposal.
- Update the plan whenever the next action changes, a blocker appears, validation results are known, or a decision is
  made.
- Write steps as concrete work items tied to files, commands, or observable outcomes.
- Capture surprises and rejected approaches. Future readers should not need to rediscover the same facts.
- Mark work complete only after the requested outcome and relevant validation are actually done.

## Plan Location

Use `PLANS.md` for persistent project-level planning conventions.

For a specific large task, create a task section under `## Active Plans` in this file. If several unrelated large tasks
are active at once, use separate headings. Remove or archive completed task sections once the work is merged and no
longer useful.

## Active Plans

### PR 846 Streamlining

**Goal**
Bring PR #846 ("Add molecular QCA library") back into a clean, reviewable, and mergeable state.

**Scope**
Update the `molQCA_lib` branch only. Expected work includes merging current `upstream/main`, resolving any resulting
conflicts, addressing unresolved actionable PR feedback, and validating with the repository's bounded build commands.
Do not modify `vendors/`, do not reintroduce `libs/`, and do not run high-parallelism builds.

**Current State**
PR: https://github.com/cda-tum/fiction/pull/846.
Branch: `molQCA_lib`.
GitHub currently reports the PR as conflicting with `main`, with changes requested, and with `pre-commit.ci` failing
because its mergeability check cannot run.
The latest fetched `upstream/main` is `b02d4104`, which merged C++20 modernization changes in SiDB simulation code.

**Plan**
- [ ] Merge current `upstream/main` into `molQCA_lib` and resolve conflicts.
- [ ] Address non-outdated actionable review feedback:
  - `include/fiction/io/write_svg_layout.hpp`: fix the MolQCA SVG cell-type scope issue.
  - `include/fiction/types.hpp`: add MolQCA handling to `tech_cell_name`.
  - `test/algorithms/simulation/sidb/random_sidb_layout_generator.cpp`: restore strong `REQUIRE` assertions for
    generated-layout optionals.
  - `test/technology/molecular_qca_library.cpp`: improve documentation/comment density for the large test helper or
    function flagged by code scanning.
- [ ] Re-check outdated review comments after the merge and fix any that become current again.
- [ ] Validate with the approved local commands.
- [ ] Push the branch and re-check PR status.

**Validation**
Use:

```bash
cmake -S . -B build
cmake --build build -j 4
ctest --test-dir build --output-on-failure -j 4
```

If `prek` is available, also run:

```bash
prek run -a
```

**Decisions**
- Use only `cmake -S . -B build` and `cmake --build build -j 4` for local build validation.
- Treat `ctest` reporting no tests in the default `build` configuration as a validation limitation, not as test
  coverage.

**Open Questions**
- Whether the code-scanning "poorly documented large function" warning should be fixed by splitting the function or by
  adding focused comments depends on the current test structure after merging `main`.

**Progress Log**
- 2026-08-10: Merged `main` once, fixed `write_qll_layout.hpp`, pushed merge commit `b54cce81`.
- 2026-08-10: Added repository exec-plan guidance in `PLANS.md` and pushed `e2a9e245`.
- 2026-08-10: Fetched current PR status: PR is still conflicting because `upstream/main` advanced to `b02d4104`.

## Active Plan Template

Copy this template for substantial tasks:

````markdown
### <Short Task Name>

**Goal**
State the user-visible outcome in one or two sentences.

**Scope**
List the files, modules, or workflows expected to change. Also list explicit non-goals.

**Current State**
Summarize what is already known: branch, PR, failing checks, conflicts, relevant commits, and any local constraints.

**Plan**
- [ ] Step with a concrete edit, investigation, or validation target.
- [ ] Step with a concrete edit, investigation, or validation target.
- [ ] Step with a concrete edit, investigation, or validation target.

**Validation**
Record the exact commands that should be run, and later record their result. For this repository, prefer:

```bash
cmake -S . -B build
cmake --build build -j 4
ctest --test-dir build --output-on-failure -j 4
```

If `prek` is available, also run:

```bash
prek run -a
```

**Decisions**
Record meaningful technical choices and why they were made.

**Open Questions**
List missing information that blocks or meaningfully changes the plan.

**Progress Log**
- YYYY-MM-DD: Short factual entry.
````

## PR Streamlining Workflow

When a PR needs to be brought back into a clean, reviewable state:

1. Sync and inspect:
   - confirm the current branch and upstream relation;
   - inspect unresolved conflicts, dirty files, and untracked files;
   - identify the PR number or URL when GitHub context is needed.

2. Resolve structural issues first:
   - finish merges or rebases;
   - resolve conflicts in public headers and tests carefully;
   - avoid committing generated build artifacts or stale dependency directories.

3. Validate with bounded commands:
   - use `cmake -S . -B build`;
   - use `cmake --build build -j 4`;
   - run tests only from the matching build directory;
   - avoid high parallelism unless explicitly requested.

4. Clean the worktree:
   - keep ignored build directories only if useful;
   - remove stale untracked directories when they are known merge leftovers;
   - never remove user-created files without confirming they are disposable.

5. Finish and push:
   - commit with a single gitmoji-prefixed subject;
   - push the branch;
   - check CI and address only actionable failures.

## Project-Specific Notes

- Do not modify `vendors/`.
- Do not add `libs/` back after the repository has moved dependencies to `vendors/`.
- The canonical local build commands are:

```bash
cmake -S . -B build
cmake --build build -j 4
```

- If `ctest --test-dir build --output-on-failure -j 4` reports no tests, note that explicitly instead of treating it as
  full test coverage.
- If `prek` is not installed, state that it was unavailable and do not fabricate a passing result.
- For Python binding work, keep the source-based binding architecture described in `AGENTS.md`.

## Completed Plans

Archive short summaries here only when they remain useful for later PRs. Otherwise, remove completed task sections.
