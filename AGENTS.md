# AGENTS.md

_fiction_ is a C++20 header-only library with Python bindings for Field-coupled
Nanocomputing (FCN) design automation. This file holds the rules that apply everywhere.
Subdirectories carry their own `AGENTS.md` with rules that apply only there.

## Working Principles

- Prefer the smallest change that fully solves the task, then improve what you found on
  the way there. `Coding` below says how to get there. Dependency bumps are the one thing
  that still belongs in its own pull request; Renovate opens those itself.
- Test the documented contract. Never pin down a provisional implementation choice: a
  test that asserts on an internal detail blocks the next refactor without protecting a
  user.
- **Work in your own git worktree, always.** Create it with
  `git worktree add .ai/worktrees/<task> -b <branch>` and work there, never directly in the
  primary checkout. Two agents that share a working directory overwrite each other's
  uncommitted edits and switch the branch under each other, and neither notices. `.ai/` is
  gitignored, so the worktree stays out of `git status`.
- **Configure one build preset per worktree, and tear the worktree down when you are done.**
  A new worktree adds about 45 MB of source files and shares the existing `.git`. Each
  configured build tree adds another 1 to 4 GB. The build trees are what make an abandoned
  worktree expensive. Use a preset from `CMakePresets.json` rather than a hand-rolled `-B`
  directory, so each build tree has a recognizable `build-<preset>` name. When the task is
  done, run `git worktree remove <path>`, which deletes the build trees with it, then run
  `git worktree prune`.
- Inspect the working tree before editing, and never revert or overwrite a change you did
  not make. If you find edits you did not write, stop and ask instead of reverting them.
- Remove scaffolding before handoff: debug output, commented-out code, and `NOLINT`
  suppressions. A suppression that has to stay names the technical reason in a comment.
- **Report every streamlining, restructuring, simplification, and optimization you notice
  while working, including in existing code.** Say what you would do and why. This is part
  of the task, not a distraction from it: a feature request or a bug fix is the moment
  someone reads the surrounding code closely, and staying quiet wastes that reading.
- Implement it in the same pull request, as its own commit, so a reviewer reads it apart
  from the feature. There is no radius: a fix or a cleanup you found while working ships
  with the work that found it, not in a follow-up pull request that nobody opens.
- A redesign is the exception, because it costs more to review than to write. Say what you
  would change and why, and act once the maintainer agrees.
- Prioritize the architecture and maintainability of the project as a whole.

## Coding

How much code to write. `Writing` governs the prose and `Code Style` governs how the code
looks; this section governs how much of it there is.

### The ladder

Understand the problem before you climb: read the task, read the code it touches, and trace
the real flow end to end. Then stop at the first rung that holds.

1. Does this need to exist? A speculative need is no need. Skip it, and say so in one line.
2. Does _fiction_ already have it? Reuse the trait, the type, or the pattern that sits a few
   headers over. Re-implementing what `include/fiction/` already carries is the most common
   way a change grows.
3. Do the STL, `mockturtle`, or `kitty` do it? Use them. If you add a helper that duplicates
   one of them, say in the pull request description why the existing one does not fit.
4. Does a build or language feature cover it? A CMake option, a compiler flag, a
   `static_assert`, or a concept beats runtime code.
5. Does a dependency already in the tree solve it? Use it. Never add one for what a few
   lines cover; adding one is an ask-first case, see `Boundaries`.
6. Can it be one line? Write one line.
7. Only then: the minimum code that works.

The ladder shortens the solution, never the reading. A small diff in the wrong place is not
a lazy win, it is a second bug.

Fix the root cause, not the symptom. A bug report names a symptom. Grep every caller of the
function you touch and fix the shared function once: one guard there is a smaller diff than
one guard per caller, and it leaves no sibling caller broken.

### Rules

- No abstraction nobody asked for: no template parameter, no configuration option, no
  policy class until a second concrete caller needs it. _fiction_ is header-only and
  template-heavy, so every added template parameter costs compile time and instantiation
  surface in every translation unit that includes the header.
- No scaffolding for a caller that does not exist yet. That caller can scaffold.
- Deletion over addition. Boring over clever. Fewest files. That last one describes the
  shape of the solution; it is not a reason to leave a cleanup you found on the way for
  someone else, which `Working Principles` requires you to ship here.
- Two options of the same size? Take the one that is correct at the edges. The goal is less
  code, not a flimsier algorithm.
- Mark a deliberate simplification that cuts a real corner with a known ceiling — an O(n²)
  scan, a naive heuristic, a bound that holds only for the layouts in `benchmarks/` — in one
  comment naming the ceiling and the upgrade path. Name the ceiling, never the tool or the
  agent that wrote the code.

### Never cut

Cutting one of these is a defect, not minimalism:

- Input validation at a public API boundary, and error handling that prevents a silent wrong
  answer.
- Anything the task explicitly asked for. If the maintainer wants the full version after you
  argue for the small one, build it and stop re-arguing.
- What this repository mandates: the Doxygen block on every symbol, the `docs/changelog.rst`
  entry, the boxes in `.github/pull_request_template.md`.
- The check. Non-trivial logic — a branch, a loop, a lifetime, a bound — leaves behind one
  runnable test that fails if the logic breaks. Match the style of the tests around it and
  add the case, not a suite. Size is not the criterion: a one-line fix to a bound or a
  branch is exactly what a regression test protects. A mechanical or documentation-only
  change needs none.

### Explaining the code

Code first, then at most three lines: what you skipped, and when to add it. If the
explanation outruns the code, delete the explanation — a paragraph defending a
simplification adds back the complexity it removed. A report, walkthrough, or review the
maintainer asked for is not padding; give that one in full.

A subagent inherits none of this. Any subagent that plans, writes, or reviews code gets the
ladder and the `Never cut` list in its prompt, or the absolute path to this file. A plan is
where the abstraction nobody asked for gets designed in, and every later step inherits it.

## Writing

These rules apply to every piece of prose you produce: commit messages, pull request
descriptions, review replies, changelog entries, Doxygen comments and Python docstrings,
code comments, and error messages.

- Cut every word that does not change the meaning. Prefer the short word.
- Use active voice and name the actor: "`hexagonalization` now rejects empty layouts", not
  "empty layouts are now rejected".
- Never use a metaphor, a figure of speech, or a filler opener ("Note that", "It is worth
  mentioning that", "Basically").
- Keep sentences short and direct, and give each sentence one idea. Prefer an explicit noun
  to a pronoun whose referent the reader has to reconstruct — "the layout", not "it".
- Use the established domain term, and use one term per concept. Never paraphrase `SiDB`,
  `defect`, `gate library`, or `operational domain` into everyday words, and never switch
  between synonyms for variety. Take terminology from the repository's own usage and from
  established precedent in field-coupled nanocomputing, logic synthesis, and design
  automation. Where those communities use different words for one concept, explain the
  mapping once and then pick one.
- Prefer everyday English to a jargon term where it costs no precision. Where it does cost
  precision, keep the precise term.
- Preserve the capitalization of project names: _fiction_, `pyfiction`, `nanobind`,
  `mockturtle`, `kitty`, `alice`, `Catch2`, `CMake`, `GitHub`, `SiDB`, `QCA`, `iNML`.
- Write for the final design, not for the history of how you got there. Never narrate
  review rounds, prompts, former names, or the order in which you did the work. Where a
  rejected alternative is worth recording because a reader would otherwise retry it, put
  it in a code comment at the site or in the pull request — not in the changelog.
- Break any of these rules rather than write something unclear or imprecise.

### Pull request, issue, and comment bodies

Default: **one paragraph, three sentences at most** — why the change was needed, what it
does, and the one thing a reviewer needs in order to read the diff. Six lines is the
ceiling. The title carries the _what_; the body carries only what the title and the diff
cannot.

`.github/pull_request_template.md` mandates the sections, not their length: `## Description`
is that one paragraph, and the checklist is ticked, not annotated. The AI-disclosure line
required under `Git and GitHub` sits outside the ceiling.

Go longer only for a reason you can name:

- a decision a reviewer cannot infer from the diff, with its reason;
- a reproduction, a measurement, or a number that makes a claim checkable.

Keep out of a pull request body: a restatement of the issue, a walk through the diff file by
file, a list of tests by name, a log of every design decision considered, a closing summary.
That detail belongs in the commit messages, attached to the code it explains, and in the
issue. Length reads as padding, not thoroughness.

An issue body takes the same shape: the symptom, what you observed, how to reproduce it. A
review reply or a report of review findings gets a line or two per defect, then stops.

Never hard-wrap prose you type into GitHub — pull request and issue bodies, review bodies,
thread comments and replies. One line per paragraph, per list item, and per table row; let
GitHub reflow it. The wrap in this repository belongs to the Markdown and reStructuredText
files in the tree, not to a web textarea.

### Documentation describes the status quo

Documentation, Doxygen comments, and docstrings state what the code does now and why. They
never describe what it used to do. A reader has no access to the earlier state, so a
sentence that compares against it carries no information and goes stale the moment someone
reads it cold.

Never write "this fixes the way it was before", "unlike the previous implementation",
"now correctly handles", "changed to return", or "no longer crashes". Describe the behavior
that exists: "returns `std::nullopt` when the layout is empty".

The same holds for the reason a thing is the way it is. Record it as a present-tense
constraint at the site, not as an account of a past attempt — "`std::unordered_map`
is not usable here because the iteration order feeds the gate ordering", not "switched away
from `std::unordered_map` because it broke the gate ordering".

`docs/changelog.rst` is the one exception, because naming the delta between two releases is
its whole purpose. The word "now" marks the boundary: "`hexagonalization` now rejects empty
layouts" is a good changelog entry, and a bad Doxygen comment on `hexagonalization`, which
should say that the function rejects empty layouts.

### Descriptions match the implementation

For any user-facing surface — an algorithm entry point, a CLI command, a `pyfiction`
binding, a page under `docs/` — keep the summary aligned with what the code actually does:
its scope, its defaults, its limitations, its failure modes, and what it deliberately does
not handle. A description that claims more than the implementation delivers produces bug
reports against behavior that was never promised in code.

When a change adds or alters such a surface, describe the capability the surface provides
and how a user can verify it. Over-explain the user-visible effect; leave the incidental
implementation detail to the code. The changelog entry for the same change is where the
delta against the previous release belongs.

## Project Map

Each subtree below adds rules to this file and never contradicts it. Read the one that
matches what you touch. **If your tool does not load nested instruction files
automatically, open the file yourself** — several do not.

| Touching                  | Read                               | Why it matters                                                      |
| ------------------------- | ---------------------------------- | ------------------------------------------------------------------- |
| `bindings/mnt/pyfiction/` | `bindings/mnt/pyfiction/AGENTS.md` | nanobind wiring; five steps, no compiler reminder                   |
| `test/`                   | `test/AGENTS.md`                   | test file base names must be globally unique                        |
| `docs/`                   | `docs/AGENTS.md`                   | a page missing from a `toctree` builds silently and is unreachable  |
| `cli/`                    | `cli/AGENTS.md`                    | the one subtree with manual source lists, in two places             |
| `experiments/`            | `experiments/AGENTS.md`            | published-paper reproductions; the code may change, the results not |

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
- **Test (Floors)**: `nox -s minimums` (Runs pytest on Python 3.10 against the lowest declared
  dependency versions)
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

`clang-tidy` findings from the `🚨 Lint` check are binding: fix them, or suppress the
specific check with a `// NOLINT(check-name)` comment stating the reason.
CodeRabbit findings are suggestions; disagreeing with a stated reason is a normal outcome,
and LLM reviewers skew conservative. CodeRabbit skips drafts, and marking a draft ready
does not trigger a pass — ask for one with a `@coderabbitai review` comment. Reply to
comments; never resolve them — resolution belongs to the reviewer. Full workflow:
`docs/contributing.rst`, "Code Review".

## Git and GitHub

Prefix every commit subject and PR title with a single plain [gitmoji](https://gitmoji.dev)
emoji character (not the `:shortcode:` text form) matching the change's _dominant_ nature,
e.g. `🐛 Fix off-by-one error in hexagonalization`. A few common ones: `🐛` bug fix, `✨` new
feature, `♻️` refactor, `⚡️` perf, `👷`/`💚` CI, `🔧` config (e.g. `CMakePresets.json`), `📝`
docs, `✅` tests, `🚨` fix warnings, `🔥` remove code. Don't stack multiple emoji by hand —
`⬆️🪝 ...` dependency-bump commits are Renovate's own automated convention, not one to
imitate.

- Add an `Assisted-by: <Model Name> via <Tool Name>` trailer to any commit whose content an
  AI agent authored, for example `Assisted-by: Claude Opus 5 via Claude Code`.
- Disclose AI assistance in **every public text body** an agent wrote or edited: pull
  request descriptions, issues, discussions, review comments, and replies. One line at the
  end is enough, naming what a human actually checked — "Drafted with AI assistance;
  measurements reproduced locally and against CI". Titles are exempt, and a review _by_
  CodeRabbit does not make your pull request AI-assisted.
- You are responsible for everything you post under your name. Review it and understand it
  first; if you cannot explain it, do not submit it.
- Never push, open or merge a pull request, post a comment, open an issue, or otherwise
  change remote state unless the maintainer authorized that specific action. Authorization
  for one task does not carry over to the next. It does carry within one pull request:
  permission to open it covers pushing fixes to it until it is green and replying to its
  review comments.
- Opening a pull request does not finish the task. Watch it: poll `gh pr checks <PR>`, fix
  what fails, push, and poll again. The matrix spans Linux, macOS, and Windows in Debug
  and Release, plus the Python bindings, and takes hours — check back periodically rather
  than blocking on `--watch`. Reviews arrive on their own schedule; answer them as they
  land, as described under "Code Review".
- Report success only once every required check is green and every review comment has an
  answer. Report a check you cannot make pass just as promptly, naming the job and quoting
  the failing lines. Never leave a red pull request unmentioned.

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
  - Describe the status quo, never the previous behavior, and keep the description true to
    what the code does — see "Documentation describes the status quo" and "Descriptions
    match the implementation" under `Writing`.
  - The codebase still carries `// Created by ...` comments. A migration to per-file
    `@file` and `@author` tags, with full name and GitHub handle, is planned; once it
    lands, that convention is enforced and `// Created by ...` is gone.

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
  - Keep plans, notes, analyses, and worktrees in `.ai/` (gitignored). Never write them to
    the repository root and never commit them. `.claude/worktrees/` is a superseded
    location; do not add to it.
  - Remove your worktree and its build trees when the task is done (`git worktree remove`,
    then `git worktree prune`).
- ⚠️ **Ask First**:
  - Before adding a new third-party dependency, whether vendored under `vendors/` or
    fetched from `CMakeLists.txt`.
  - Before changing major build configurations.
- 🚫 **Never**:
  - Modify the contents of an existing `vendors/` source tree. Adding a new one is the
    ask-first case above; editing what is already there is not.
  - Edit generated files by hand, including
    `bindings/mnt/pyfiction/include/pyfiction/pybind11_mkdoc_docstrings.hpp`.
  - Commit secrets or large binary files.
  - Use `using namespace std;`.
