# Contributing

Thank you for your interest in contributing to this project.
We value contributions from people with all levels of experience.
In particular if this is your first pull request, not everything has to be perfect.
We will guide you through the process.

We use GitHub to [host code](https://github.com/cda-tum/fiction), to [track issues and feature requests](https://github.com/cda-tum/fiction/issues), as well as accept [pull requests](https://github.com/cda-tum/fiction/pulls).
See <https://docs.github.com/en/get-started/quickstart> for a general introduction to working with GitHub and contributing to projects.

## Types of Contributions

You can contribute in several ways:

- 🐛 Report Bugs
  : Report bugs at <https://github.com/cda-tum/fiction/issues> using the _🐛 Bug report_ issue template. Please make sure to fill out all relevant information in the respective issue form.
- 🐛 Fix Bugs
  : Look through the [GitHub Issues](https://github.com/cda-tum/fiction/issues) for bugs. Anything tagged with "bug" is open to whoever wants to try and fix it.
- ✨ Propose New Features
  : Propose new features at <https://github.com/cda-tum/fiction/issues> using the _✨ Feature request_ issue template. Please make sure to fill out all relevant information in the respective issue form.
- ✨ Implement New Features
  : Look through the [GitHub Issues](https://github.com/cda-tum/fiction/issues) for features. Anything tagged with "enhancement" is open to whoever wants to implement it. We highly appreciate external contributions to the project.
- ✔️ Write Tests
  : Add tests for uncovered contracts and simplify redundant checks. The [CodeCov report](https://app.codecov.io/gh/cda-tum/fiction) helps locate unexercised code; inspect its requirements and callers before choosing a test.
- 📝 Write Documentation
  : _fiction_ could always use some more [documentation](https://fiction.readthedocs.io/en/latest/), and we appreciate any help with that.

## First Contributions

Start with {doc}`getting_started` to set up _fiction_ for local development.
See {doc}`tooling` for the tools and configuration files used in development.

Follow the guidelines below when preparing a pull request.

## Core Guidelines

- ["Commit early and push often"](https://www.worklytics.co/blog/commit-early-push-often).
- Write meaningful commit messages. Prefix each subject with a single [gitmoji](https://gitmoji.dev) character matching the change's dominant nature; `AGENTS.md` documents the convention in full.
- Focus on one feature or bug. Include fixes and cleanups found during the work as separate commits; discuss redesigns first. Keep dependency updates in separate pull requests.
- If you added a new feature, you should add tests that ensure it works as intended. Furthermore, the new feature should be documented appropriately.
- If you fixed a bug, you should add tests that demonstrate that the bug has been fixed.
- Document your code thoroughly and write readable code.
- Keep your code clean. Remove any debug statements, left-over comments, or code unrelated to your contribution.
- Follow the style and conventions used throughout the project.
- Enable EditorConfig support in your editor to apply the repository's indentation, line endings, and final-newline settings. The formatter hooks check the full style.
- Run `clang-format` and `clang-tidy` to check your code for style and linting errors before committing.
- We recommend installing [prek](https://prek.j178.dev/) and running `prek install` once so that formatting and linting checks run automatically before every commit.

## C++ Static Analysis

The C++ lint session requires [uv](https://docs.astral.sh/uv/) and a C++ compiler. It installs
CMake, Ninja, Clang-Tidy, and the Python build dependencies in an isolated environment, then
generates the same compilation database and runs the same Clang-Tidy configuration as CI.

```console
$ uvx nox -s cpp_lint
$ uvx nox -s cpp_lint -- --diff-base origin/main
$ uvx nox -s cpp_lint -- --all
```

The first command checks staged C++ files when the index contains changes; otherwise, it checks
the last commit. The second checks files changed from the given Git revision. The third checks
every eligible C++ file.

## Pull Request Workflow

- Create PRs early. It is ok to create work-in-progress PRs. You may mark these as draft PRs on GitHub.

- Follow the PR template. Use a descriptive title and a short paragraph explaining the need, resulting behavior, and any decision the diff cannot explain. Reference related issues and complete the checklist.

- Whenever a PR is created or updated, the `CI` workflow runs. Its `🔍 Change` job classifies your diff and skips the jobs your changes cannot affect, so a documentation-only PR does not pay for the C++ matrix. `🚦 Check` aggregates every job and is the one status that has to be green. Here are some tips for finding the cause of certain failures:

  - If any of the `🐧 Test`, `🍎 Test`, `🪟 Test`, `🧪 Experiments`, or `🐳 Docker` checks fail, this most likely indicates build errors or test failures in the C++ part of the code base. Look through the respective logs on GitHub for any error or failure messages.
  - If `☂️ Coverage` or `🐍 Coverage` fails, inspect the job log for a build, test, report, or upload error. Codecov's separate project and patch statuses enforce coverage targets; add tests when those statuses report insufficient coverage.
  - If a `🐍 Packaging` check fails, this indicates an error in the Python part of the code base. `cibuildwheel` runs the `pyfiction` test suite against every wheel it builds, so a failing test shows up here. Look through the respective logs on GitHub for any error or failure messages.
  - If the `📝 CodeQL` check fails, this indicates a security vulnerability in the code base. Look through the respective logs on GitHub for any error or failure messages.
  - If `🚨 Lint` comments on your PR with a list of suggestions/warnings, `clang-tidy` raised them when checking the C++ part of your changes for warnings or style guideline violations. The individual messages frequently provide helpful suggestions on how to fix the warnings.
  - If the `🐍 Docstrings` check fails, the generated `pybind11_mkdoc_docstrings.hpp` no longer matches the Doxygen comments in `include/fiction/`. Download the `pyfiction-docstrings` artifact from that run, put it in place of `bindings/include/pyfiction/pybind11_mkdoc_docstrings.hpp`, and commit it. Never edit that file by hand; the job's summary carries the same instructions.
  - If `🚦 Check` fails while every job below it looks green, a job was skipped for a reason change detection did not predict — usually because something it depends on failed first.

- Once your PR is ready, change it from a draft PR to a regular PR and request a review from one of the project maintainers.

- If your PR gets a "Changes requested" review, you will need to address the feedback and update your PR by pushing to the same branch. You don't need to close the PR and open a new one. Respond to review comments on the PR (e.g., with "done 👍"). Be sure to re-request review once you have made changes after a code review so that maintainers know that the requests have been addressed.

## Selecting Python Tests

Tests inherit markers from their directories. Run `pytest -m simulation` to select
SiDB simulation tests, or `uvx nox -s tests-3.12 -- -m simulation` to build the bindings
and run that selection in an isolated environment. Use `pytest --markers` to list
the available groups.

Markers include `layouts`, `networks`, `physical_design`, `synthesis`, `verification`,
`fcn`, `inml`, `sidb`, `simulation`, `io`, and `generators`. Nested tests inherit every
matching ancestor marker: `pytest -m "sidb and io"` selects SiDB I/O tests, including
simulation I/O. New files in those directories need no marker annotations. Running
without `-m` executes the full suite.

## Python Coverage

Run the test suite with coverage on one supported Python version:

```sh
uvx nox -s tests-3.12 -- --cov --cov-config=pyproject.toml --cov-report=term-missing --cov-report=xml
```

Coverage measures the Python source in `mnt.pyfiction`, including Python subprocesses.
The report excludes the embedded test package and typing-only statements. Compiled C++
binding code is not measured by `pytest-cov`.

CI combines reports from the supported Python versions and platforms under Codecov's
`python` flag. The `cpp` flag measures C++ headers. Each flag has its own project and
patch status; Python project coverage is compared with the measured base revision.

(code-review)=

## Code Review

Two automated reviewers comment on every pull request, and they carry different weight.

- The `🚨 Lint` check posts `clang-tidy` findings. Treat these as **binding**: fix them, or suppress the specific check with a `// NOLINT(check-name)` comment that states the reason.
- [CodeRabbit](https://coderabbit.ai) performs the first substantive review pass, covering design, contracts, tests, and documentation. Treat its findings as **suggestions**.

When working with CodeRabbit:

- **Review the review.** Language models skew conservative and will sometimes push toward overcomplicated code. Verify each finding against the current code, and disagree in writing when it is wrong — that is a normal outcome, not a problem.
- **Respond to comments rather than silently resolving them.** CodeRabbit learns from replies, and a resolved-but-unanswered thread hides the reasoning from human reviewers. Leave resolution to the reviewer.
- **Re-request a pass** with `@coderabbitai review` after a substantial push, or `@coderabbitai full review` after a rebase. When you need a CodeRabbit response in a thread, mention `@coderabbitai`.
- **Do not enable a second AI reviewer** on the same pull request. CodeRabbit performs noticeably worse when another review bot is active on the same diff.
- **Apply reviewer code suggestions through GitHub's batch flow** ("Add suggestion to batch" → "Commit suggestions"). Each suggestion's author is recorded as a co-author of the resulting commit, and the person committing the batch is the committer and also a co-author.
- **Avoid force-pushing or squashing locally while a review is open.** It detaches existing comments from their lines. Maintainers squash on merge.

`.coderabbit.yaml` in the repository root configures which paths are reviewed and what CodeRabbit should not comment on. Update it rather than repeating the same correction by hand.

## Publishing to PyPI

The `CD` workflow publishes `mnt.pyfiction` when a GitHub release is published.
The `publish-to-pypi` job downloads the source distribution and wheels from the build jobs
and authenticates through [PyPI trusted publishing](https://docs.pypi.org/trusted-publishers/using-a-publisher/).
The job uses the `pypi` GitHub environment and requires `id-token: write` permission.
A manual workflow run builds the distributions but skips publishing.

The GitHub trusted publisher on the
[project's Publishing page](https://pypi.org/manage/project/mnt.pyfiction/settings/publishing/)
uses these values:

| Field | Value |
| --- | --- |
| Owner | `cda-tum` |
| Repository name | `fiction` |
| Workflow name | `cd.yml` |
| Environment name | `pypi` |

The workflow name is the filename, without `.github/workflows/`.
Configure any required reviewers or release-tag restrictions on the `pypi` GitHub environment.
After the next release, verify that `Deploy to PyPI` succeeds and that the release files
and their attestations appear on PyPI. Then revoke the old deployment token on PyPI and
remove the `PYPI_DEPLOY_TOKEN` GitHub secret if no other workflow uses that token.

## AI-Assisted Contributions

Contributions written with the help of an AI agent are welcome, under two conditions.

- **Disclose it.** Add an `Assisted-by: <Model Name> via <Tool Name>` trailer to any commit whose content an agent authored, and add a line disclosing AI assistance to every public text body an agent wrote or edited: pull request descriptions, issues, discussions, review comments, and replies. One line at the end is enough, and it should name what you actually checked. Titles are exempt, and having your PR reviewed by CodeRabbit does not make it an AI-assisted contribution.
- **Take responsibility for it.** Review and understand the content before you post it. If you cannot explain it, do not submit it. A contribution should be worth more than the effort it costs to review.

Agent instructions live in `AGENTS.md` at the repository root, with additional files in the subdirectories they apply to. If your tool expects a different file name, create a local symlink — for example `ln -s AGENTS.md CLAUDE.md`. Those names are gitignored, so the repository itself stays tool-neutral.

## Writing Catch2 tests

Test observable contracts with the smallest inputs that distinguish correct behavior from a plausible defect.
Keep boundary, invalid-input, ownership, and regression cases even when their setup looks similar.
Repeated checks before and after an operation can protect different state transitions.
Before deleting a test, identify the surviving oracle and explain which inputs and failures it covers.
Equal line coverage or one detected mutation does not establish redundancy; use the contract audit below.

### Choose the test structure

- Use `TEST_CASE` for one concrete behavior and `TEMPLATE_TEST_CASE` when the same contract applies to several
  types. Every type must represent a useful implementation or compatibility path.
- Use `SECTION` for independent operations with common setup. Catch2 re-enters the test case for each section
  path, so local setup is fresh. Put an assertion inside each leaf section: a check after the section does not
  satisfy `--warn NoAssertions`. Keep sequential operations together when their interaction is the contract.
- Use `TEST_CASE_METHOD` or `TEMPLATE_TEST_CASE_METHOD` when several cases need the same nontrivial setup or
  resource lifetime. Prefer an existing blueprint or a local setup expression for simple construction.
  Fixtures own per-test state; avoid shared mutable state and configurable fixture hierarchies.
  Catch2's typed fixture macros expand to C arrays flagged by clang-tidy; use `TEMPLATE_TEST_CASE` with
  a local fixture object for typed setup.
- Use `GENERATE` and table generators for independent input/expected-output rows. Keep related values in one
  row: separate generators create a Cartesian product. Place generators inside the section that needs them.
  Keep loops for state transitions and bounded failure sweeps whose next step depends on the previous result.
- Name cases and sections after behavior. Use `DYNAMIC_SECTION` for readable generated case names, `CAPTURE`
  for input values, and `INFO` for context that an assertion cannot show. Diagnostics must identify a failing
  row without requiring successful-assertion output.

See Catch2's [sections](https://catch2-temp.readthedocs.io/en/latest/test-cases-and-sections.html),
[fixtures](https://catch2-temp.readthedocs.io/en/latest/test-fixtures.html),
[generators](https://catch2-temp.readthedocs.io/en/latest/generators.html), and
[diagnostics](https://catch2-temp.readthedocs.io/en/latest/logging.html).

### Choose the assertion

- Use `REQUIRE` before dereferencing, indexing, or querying an object whose existence is a prerequisite.
  Use `CHECK` for independent observations so one failure does not hide other useful diagnostics.
- Use direct expressions and `CHECK_FALSE`/`REQUIRE_FALSE` for negative predicates. Separate independent
  conditions instead of hiding them inside a parenthesized `&&` expression.
- Use `CHECK_THROWS_AS` or `REQUIRE_THROWS_AS` for an exception-type contract. Check the state after a rejected
  mutation. Match exception text only when callers rely on that text.
- Compare complete collections. `RangeEquals` checks order; `UnorderedRangeEquals` ignores order but preserves
  multiplicity. Converting observations to a set hides duplicate visits; membership-only callbacks can miss
  omissions. For larger comparable collections, sort observations and expectations and use `RangeEquals` to
  avoid quadratic permutation matching. Keep direct equality when it already expresses the contract clearly.
- Use `STATIC_REQUIRE`, `STATIC_REQUIRE_FALSE`, or `static_assert` for compile-time contracts. Runtime cases
  should still exercise observable behavior. Do not weaken an exact result into approximate equality.
- Keep assertions, diagnostics, fixture construction, and generators outside active allocation-failure
  injection. Reset the budget before checking results, including on unexpected exceptions. Preserve the
  existing platform-specific `SKIP` and test the operation's rollback and recovery.

See Catch2's [assertions](https://catch2-temp.readthedocs.io/en/latest/assertions.html) and
[generic range matchers](https://catch2-temp.readthedocs.io/en/latest/matchers.html#generic-range-matchers).

### Examples and verification

The [gate-layout tests](https://github.com/cda-tum/fiction/blob/main/test/layouts/test_gate_layout.cpp)
demonstrate these choices using real contracts:

| Technique | Example |
| --- | --- |
| Typed local setup and independent sections | Object identity; copies isolating geometry, terminals, and connectivity |
| Named table rows and diagnostics | Elementary truth tables; clocked neighbor degrees |
| Complete unordered collections | Iteration over clocking zones; five-input objects and their sinks |
| Ordered collections | Five-input source ports; sorted observations of sparse object iteration |
| Compile-time assertions | Canonical layout aliases; borrowed clocking references; move exception specifications |
| Fatal prerequisites and exception checks | Geometry clone and assignment failures |
| Bounded fault sweeps and platform skips | Failed creation and name assignment |

Run the affected executable with `--order rand --rng-seed 17 --warn NoAssertions`, repeat with other recorded
seeds, and confirm that the intended cases ran. Use CTest for normal integration. Compare runtime and compilation
cost when adding type/value combinations, fixtures, or matchers. More assertions are not a quality target.
See Catch2's [execution guidance](https://catch2-temp.readthedocs.io/en/latest/usage-tips.html).

These examples do not cover every testing domain. Numerical tests need justified absolute, relative, or ULP
tolerances using [floating-point matchers](https://catch2-temp.readthedocs.io/en/latest/comparing-floating-point-numbers.html).
Parser tests need independent valid and malformed inputs; constructing every input with the matching writer
can conceal shared errors. Adopt these techniques through a separate contract audit of the relevant module.

## Contract Audits

Use the [SpecAudit method](https://github.com/cda-tum/fiction/blob/main/.agent/AUDITS.md)
to check whether tests protect intended behavior or constrain incidental choices.
Start with one concrete concern, inspect the contract and consumers, and support
behavioral claims with reproducible experiments. Preserve regression coverage,
numerical accuracy, input validation, and compatibility. Keep optional audit
records in ignored `.ai/audits/`; apply fixes and publish only within the scope
the maintainer authorized. The method includes a SiDB text-printer example request.

```{raw} html
<hr>
```

This document was inspired by and partially adapted from

- <https://matplotlib.org/stable/devel/coding_guide.html>
- <https://opensource.creativecommons.org/contributing-code/pr-guidelines/>
- <https://yeoman.io/contributing/pull-request.html>
- <https://github.com/scikit-build/scikit-build>

(building-documentation)=

## Building the Documentation

Install [uv](https://docs.astral.sh/uv/), Doxygen, and the C++ build prerequisites
listed in {doc}`getting_started`. The documentation session uses Python 3.12 and
builds the checkout's Python bindings. The session installs the Python build dependencies,
including `z3-solver`; CMake discovers Z3 from that environment.

Run these commands from the repository root:

```console
$ uvx nox -s docs
$ uvx nox --non-interactive -s docs
$ uvx nox -s docs -- -b linkcheck
```

The first command serves an HTML preview with live reload. The second writes
`docs/_build/html/index.html` and exits. The third checks external links and
writes its report to `docs/_build/linkcheck`. Additional arguments after `--`
are passed to Sphinx. Read the Docs uses the same non-interactive build.

The docs dependency group requires Python 3.12 or newer. `uv.lock` records the
versions used by local and hosted builds. HTML builds also produce `llms.txt`,
`llms-full.txt`, and a Markdown copy of each page alongside its HTML file.

(citing-publications)=

### Citing Publications

Add publication metadata to `docs/references.bib` and preserve existing citation keys.
Use MyST citation roles such as `` {cite:p}`fiction` `` in documentation pages.
The {doc}`publications` page renders the bibliography and provides a BibTeX download;
keep algorithm and experiment links beside their citations. Run
`prek run bibtex-tidy --all-files` to format entries and fields consistently.

HTML pages include OpenGraph titles, descriptions, canonical URLs, and the project logo.
Write a clear opening paragraph for each page's generated description. Keep the default
image in `docs/_static/mnt_social.png` aligned with the project logo.
