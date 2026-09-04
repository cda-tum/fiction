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
  : We can always use more tests to ensure that the code base is robust and stable. If you want to help out, you can start by looking through the [CodeCov report](https://app.codecov.io/gh/cda-tum/fiction) to find out where we missed covering lines.
- 📝 Write Documentation
  : _fiction_ could always use some more [documentation](https://fiction.readthedocs.io/en/latest/), and we appreciate any help with that.

## First Contributions

Ready to contribute? Check out the [documentation](https://fiction.readthedocs.io/en/latest/) to set up _fiction_ for local development and learn about the style guidelines and conventions used throughout the project.

We value contributions from people with all levels of experience.
In particular if this is your first PR not everything has to be perfect.
We will guide you through the PR process.
Nevertheless, please try to follow the guidelines below as well as you can to help make the PR process quick and smooth.

## Core Guidelines

- ["Commit early and push often"](https://www.worklytics.co/blog/commit-early-push-often).
- Write meaningful commit messages. Prefix each subject with a single [gitmoji](https://gitmoji.dev) character matching the change's dominant nature; `AGENTS.md` documents the convention in full.
- Focus on a single feature/bug at a time and only touch relevant files. Split multiple features into multiple contributions.
- If you added a new feature, you should add tests that ensure it works as intended. Furthermore, the new feature should be documented appropriately.
- If you fixed a bug, you should add tests that demonstrate that the bug has been fixed.
- Document your code thoroughly and write readable code.
- Keep your code clean. Remove any debug statements, left-over comments, or code unrelated to your contribution.
- Follow the style and conventions used throughout the project.
- Run `clang-format` and `clang-tidy` to check your code for style and linting errors before committing.
- We recommend installing [prek](https://prek.j178.dev/) and running `prek install` once so that formatting and linting checks run automatically before every commit.

## Pull Request Workflow

- Create PRs early. It is ok to create work-in-progress PRs. You may mark these as draft PRs on GitHub.

- Describe your PR. Start with a descriptive title, reference any related issues by including the issue number in the PR description, and add a comprehensive description of the changes. We provide a PR template that you can (and should) follow to create a PR.

- Whenever a PR is created or updated, the `CI` workflow runs. Its `🔍 Change` job classifies your diff and skips the jobs your changes cannot affect, so a documentation-only PR does not pay for the C++ matrix. `🚦 Check` aggregates every job and is the one status that has to be green. Here are some tips for finding the cause of certain failures:

  - If any of the `🐧 Test`, `🍎 Test`, `🪟 Test`, `🧪 Experiments`, or `🐳 Docker` checks fail, this most likely indicates build errors or test failures in the C++ part of the code base. Look through the respective logs on GitHub for any error or failure messages.
  - If the `☂️ Coverage` check fails, this means that your changes are not appropriately covered by tests or that the overall project coverage decreased too much. Ensure that you include tests for all your changes in the PR.
  - If a `🐍 Packaging` check fails, this indicates an error in the Python part of the code base. `cibuildwheel` runs the `pyfiction` test suite against every wheel it builds, so a failing test shows up here. Look through the respective logs on GitHub for any error or failure messages.
  - If the `📝 CodeQL` check fails, this indicates a security vulnerability in the code base. Look through the respective logs on GitHub for any error or failure messages.
  - If `🚨 Lint` comments on your PR with a list of suggestions/warnings, `clang-tidy` raised them when checking the C++ part of your changes for warnings or style guideline violations. The individual messages frequently provide helpful suggestions on how to fix the warnings.
  - If the `🐍 Docstrings` check fails, the generated `pybind11_mkdoc_docstrings.hpp` no longer matches the Doxygen comments in `include/fiction/`. Download the `pyfiction-docstrings` artifact from that run, put it in place of `bindings/mnt/pyfiction/include/pyfiction/pybind11_mkdoc_docstrings.hpp`, and commit it. Never edit that file by hand; the job's summary carries the same instructions.
  - If `🚦 Check` fails while every job below it looks green, a job was skipped for a reason change detection did not predict — usually because something it depends on failed first.

- Once your PR is ready, change it from a draft PR to a regular PR and request a review from one of the project maintainers.

- If your PR gets a "Changes requested" review, you will need to address the feedback and update your PR by pushing to the same branch. You don't need to close the PR and open a new one. Respond to review comments on the PR (e.g., with "done 👍"). Be sure to re-request review once you have made changes after a code review so that maintainers know that the requests have been addressed.

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

## AI-Assisted Contributions

Contributions written with the help of an AI agent are welcome, under two conditions.

- **Disclose it.** Add an `Assisted-by: <Model Name> via <Tool Name>` trailer to any commit whose content an agent authored, and add a line disclosing AI assistance to every public text body an agent wrote or edited: pull request descriptions, issues, discussions, review comments, and replies. One line at the end is enough, and it should name what you actually checked. Titles are exempt, and having your PR reviewed by CodeRabbit does not make it an AI-assisted contribution.
- **Take responsibility for it.** Review and understand the content before you post it. If you cannot explain it, do not submit it. A contribution should be worth more than the effort it costs to review.

Agent instructions live in `AGENTS.md` at the repository root, with additional files in the subdirectories they apply to. If your tool expects a different file name, create a local symlink — for example `ln -s AGENTS.md CLAUDE.md`. Those names are gitignored, so the repository itself stays tool-neutral.

```{raw} html
<hr>
```

This document was inspired by and partially adapted from

- <https://matplotlib.org/stable/devel/coding_guide.html>
- <https://opensource.creativecommons.org/contributing-code/pr-guidelines/>
- <https://yeoman.io/contributing/pull-request.html>
- <https://github.com/scikit-build/scikit-build>

## Building the Documentation

Install [uv](https://docs.astral.sh/uv/), Doxygen, and the C++ build prerequisites
listed in {doc}`getting_started`. The documentation session uses Python 3.12 and
builds the checkout's Python bindings. Install Z3 and make its headers and library
available to CMake, as required by the `pyfiction` preset.

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

### Citing Publications

Add publication metadata to `docs/references.bib` and preserve existing citation keys.
Use MyST citation roles such as `` {cite:p}`fiction` `` in documentation pages.
The {doc}`publications` page renders the bibliography and provides a BibTeX download;
keep algorithm and experiment links beside their citations. Run
`prek run bibtex-tidy --all-files` to format entries and fields consistently.

HTML pages include OpenGraph titles, descriptions, canonical URLs, and the project logo.
Write a clear opening paragraph for each page's generated description. Keep the default
image in `docs/_static/mnt_social.png` aligned with the project logo.
