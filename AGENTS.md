# AGENTS.md

_fiction_ is a C++20 header-only library with Python bindings for Field-coupled
Nanocomputing (FCN) design automation. This file holds the rules that apply everywhere.
Subdirectories carry their own `AGENTS.md` with rules that apply only there.

## Working Principles

- Prefer the smallest change that fully solves the task. Keep unrelated cleanup,
  reformatting, and dependency bumps out of the same pull request; open a separate one.
- Do not add an abstraction, a template parameter, or a configuration option until a
  second concrete caller needs it. _fiction_ is header-only and template-heavy, so every
  added template parameter costs compile time and instantiation surface in every
  translation unit that includes the header.
- Prefer an existing facility from the STL, `mockturtle`, or `kitty` over a new
  implementation. If you add a helper that duplicates one of them, say in the pull request
  description why the existing one does not fit.
- Test the documented contract, not provisional implementation choices. A test that pins
  down an internal detail blocks the next refactor without protecting a user.
- Inspect the working tree before editing, and never revert or overwrite a change you did
  not make.
- Remove scaffolding before handoff: debug output, commented-out code, and `NOLINT`
  suppressions. A suppression that has to stay names the technical reason in a comment.
- Warn when you spot a sub-par design decision, including in existing code, and say what
  you would do instead. Do not act on it in the same change without asking.
- Prioritize the architecture and maintainability of the project as a whole.

## Writing

These rules apply to every piece of prose you produce: commit messages, pull request
descriptions, review replies, changelog entries, Doxygen comments and Python docstrings,
code comments, and error messages.

- Cut every word that does not change the meaning. Prefer the short word.
- Use active voice and name the actor: "`hexagonalization` now rejects empty layouts", not
  "empty layouts are now rejected".
- No metaphors, no figures of speech, no filler openers ("Note that", "It is worth
  mentioning that", "Basically").
- Use the established domain term, and use one term per concept. Do not paraphrase `SiDB`,
  `defect`, `gate library`, or `operational domain` into everyday words, and do not switch
  between synonyms for variety.
- Preserve the capitalization of project names: _fiction_, `pyfiction`, `nanobind`,
  `mockturtle`, `kitty`, `alice`, `Catch2`, `CMake`, `GitHub`, `SiDB`, `QCA`, `iNML`.
- Write for the final design, not for the history of how you got there. Do not narrate
  review rounds, prompts, former names, or the order in which you did the work. Where a
  rejected alternative is worth recording because a reader would otherwise retry it, put
  it in a code comment at the site or in the pull request — not in the changelog.
- Break any of these rules rather than write something unclear or imprecise.

## Project Map

Each subtree below adds rules to this file and never contradicts it. Read the one that
matches what you touch. **If your tool does not load nested instruction files
automatically, open the file yourself** — several do not.

| Touching                  | Read                               | Why it matters                                                     |
| ------------------------- | ---------------------------------- | ------------------------------------------------------------------ |
| `bindings/mnt/pyfiction/` | `bindings/mnt/pyfiction/AGENTS.md` | nanobind wiring; five steps, no compiler reminder                  |
| `test/`                   | `test/AGENTS.md`                   | test file base names must be globally unique                       |
| `docs/`                   | `docs/AGENTS.md`                   | a page missing from a `toctree` builds silently and is unreachable |
| `cli/`                    | `cli/AGENTS.md`                    | the one subtree with manual source lists, in two places            |
| `experiments/`            | `experiments/AGENTS.md`            | published-paper reproductions; do not refactor them                |

Everything else: `include/fiction/` is the header-only C++20 library; `vendors/` holds
third-party sources and is never modified; `benchmarks/` is input data. Build presets live
in `CMakePresets.json` and are documented in `docs/getting_started.rst`.

## Commands

Use these commands to validate your work.

### C++ (Primary)

- **Configure**: `cmake -S . --preset dev-full` (see `cmake --list-presets` for
  `tests-slim`/`tests-full`/`pyfiction`/etc.)
- **Build**: `cmake --build --preset dev-full -j`
- **Test**: `ctest --preset dev-full --output-on-failure`
- **Format**: `prek run clang-format --all-files` (or let prek handle it)

### Python (Bindings)

- **Test (Full)**: `nox -s tests` (Runs pytest in isolated environments)
- **Test (Quick)**: `pytest` (Use if only Python code changed to avoid C++ rebuilds)
- **Lint**: `nox -s lint` (Runs prek hooks including ruff and mypy)

### General

- **Prek**: `prek run -a` (Runs all checks: formatting, linting, static analysis). Run this
  before finishing a task.

## Code Review

Fetch open comments before considering a pull request done:
`gh api repos/{owner}/{repo}/pulls/<PR>/comments --paginate`. Classify each one against the
current code before acting:

- already fixed by a later commit — reply with the commit hash, change nothing;
- wrong or out of scope — reply with one sentence saying why, change nothing;
- valid — fix it, and consolidate duplicates into one reply.

`clang-tidy` findings from the `Clang-Tidy Review` workflow are binding: fix them, or
suppress the specific check with a `// NOLINT(check-name)` comment stating the reason.
CodeRabbit findings are suggestions; disagreeing with a stated reason is a normal outcome,
and LLM reviewers skew conservative. Reply to comments, do not resolve them — resolution
belongs to the reviewer. Full workflow: `docs/contributing.rst`, "Code Review".

## Git and GitHub

Prefix every commit subject and PR title with a single plain [gitmoji](https://gitmoji.dev)
emoji character (not the `:shortcode:` text form) matching the change's _dominant_ nature,
e.g. `🐛 Fix off-by-one error in hexagonalization`. A few common ones: `🐛` bug fix, `✨` new
feature, `♻️` refactor, `⚡️` perf, `👷`/`💚` CI, `🔧` config (e.g. `CMakePresets.json`), `📝`
docs, `✅` tests, `🚨` fix warnings, `🔥` remove code. Don't stack multiple emoji by hand —
`⬆️🪝 ...` dependency-bump commits are Renovate's own automated convention, not one to
imitate.

- Add an `Assisted-by: <Model Name> via <Tool Name>` trailer to any commit whose content an
  AI agent authored, for example `Assisted-by: Claude Opus 4.5 via Claude Code`.
- State in the pull request description that the change was AI-assisted, and that you
  reviewed and understood it before opening the request.
- Never push, open or merge a pull request, post a comment, or otherwise change remote
  state unless the maintainer authorized that specific action. Authorization for one task
  does not carry over to the next.

## Code Style

`clang-format` and `clang-tidy` enforce C++ formatting and identifier naming; `ruff` and
`mypy` cover Python. The rules below are the ones no tool checks.

### C++

- **Headers**: `.hpp` extension. Use `#ifndef FICTION_FILENAME_HPP` include guards.
- **Documentation**:
  - Apply to **ALL** new and edited code.
  - Doxygen style `/** ... */` before **ALL** symbols (members, aliases, functions,
    classes, etc.).
  - Use modern Doxygen commands (`@brief`, `@param`, `@return`, `@tparam`, `@file`,
    `@author`, `@ref`, `@see`, `@throws` etc.).
  - The current codebase uses `// Created by ...` comments. A migration to using `@file`
    and `@author` tags per file (with full name and GitHub handle) is planned. After
    migration, the new convention will be enforced and `// Created by ...` comments should
    no longer be used.

### Python

`ruff` currently ignores the `D` (pydocstyle) and `N` (naming) rule sets, so nothing
enforces the following:

- **Naming**: functions and variables `snake_case`; classes `PascalCase` (this differs
  from the legacy C++ bindings — use `PascalCase` for new code).
- **Type Hints**: mandatory for all function arguments and return values.
- **Docstrings**: Google style.

## Boundaries

- ✅ **Always**:
  - Run `prek run -a` before finishing a task.
  - Write tests for new functionality (`test/` for C++, `bindings/mnt/pyfiction/test/` for
    Python).
  - Update `docs/changelog.rst`'s `Unreleased` section for any user-facing change; see
    `docs/AGENTS.md` for the entry style.
  - Satisfy every box in `.github/pull_request_template.md` before calling a PR done.
  - Use `const` correctness and braced initialization.
  - Keep plans, notes, and analyses in `.ai/` (gitignored). Never write them to the
    repository root and never commit them.
- ⚠️ **Ask First**:
  - Before adding new third-party dependencies to `vendors/` or `CMakeLists.txt`.
  - Before changing major build configurations.
- 🚫 **Never**:
  - Modify files in `vendors/`.
  - Edit generated files by hand, including
    `bindings/mnt/pyfiction/include/pyfiction/pybind11_mkdoc_docstrings.hpp`.
  - Commit secrets or large binary files.
  - Use `using namespace std;`.
