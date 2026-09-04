# AGENTS.md — documentation

`docs/` is MyST Markdown built by Sphinx, plus Doxygen for the C++ API. Run
`uvx nox -s docs` for a local preview with live reload, or add `--non-interactive`
to build and exit. Read the Docs uses the same session for pull request previews.

## Adding a page

- A new page needs a `.md` file **and** a line in the matching `{toctree}` in
  `docs/index.md`. A page that is not in a `toctree` builds without an error and is
  unreachable.
- API documentation comes from the C++ sources through Doxygen directives —
  `{doxygenfunction}`, `{doxygenclass}`, `{doxygenstruct}`, `{doxygenenum}`.
  Do not retype a signature or a parameter description into the `.md` file; fix the
  Doxygen comment in the header instead.
- Where a feature exists in both C++ and Python, use `{tab-set}` with
  `:sync-group: language` and `{tab-item}` with `:sync: cpp` or `:sync: python`,
  following `technology/sidb/generators/design_gates.md`.

## Prose

The `Writing` rules in the root `AGENTS.md` apply here in full. In addition:

- Write _fiction_ in italics when referring to the project.
- Use single backticks for code literals and fenced MyST directives for Sphinx features.
- Preserve explicit labels and use `{ref}` and `{doc}` roles for internal links.
- Put Python autodoc directives in `{eval-rst}` fences: Sphinx emits their content
  as reStructuredText. Keep prose and C++ directives in MyST.
- Use rumdl's MyST-aware checks and run `prek run -a` after editing a page.

## The changelog

`docs/changelog.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Every user-facing change adds a bullet to the `Unreleased` section under `Added`,
`Changed`, `Fixed`, or `Removed`, matching the existing category order and bullet style.

### Length

**One or two sentences per entry.** Aim for 90 characters and treat 350 as the ceiling —
that is the profile of the `v0.6.12` and earlier releases, and it is the target.

The changelog says _what changed and what a user must do about it_. It is not a commit
message, a design document, or a work log. In particular, do not write:

- per-file or per-symbol enumerations of a refactor — "modernized the module for C++20"
  covers what a reader needs, and `git log` covers the rest;
- approaches that were tried and reverted, or the reason CI rejected them;
- lists of individual linter findings that were fixed along the way;
- narration of the order in which work happened ("started with…", "then extended to…",
  "continued into…").

Rationale that a future maintainer genuinely needs belongs in a code comment at the site
it applies to, or in the pull request. Both outlive the changelog entry and sit closer to
the code they explain.

Every entry is grouped under a category bullet — `Algorithms`, `Build system`, `CLI`,
`Code quality`, `Continuous integration`, `Data structures`, `Dependencies`,
`Documentation`, `Experiments`, `Gate libraries`, `I/O`, `Python bindings`, `Tooling` —
with the actual entries as nested sub-bullets. Do not add flat top-level bullets. Order
the categories alphabetically within each of `Added`, `Changed`, `Fixed`, and `Removed`,
as listed above.
