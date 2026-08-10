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
