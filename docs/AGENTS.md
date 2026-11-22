# AGENTS.md — documentation

`docs/` is MyST Markdown built by Sphinx, plus Doxygen for the C++ API. Run
`uvx nox -s docs` for a live preview or `uvx nox --non-interactive -s docs` for a
build that exits. Read the Docs uses the same session for pull request previews.

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

- Write _fiction_ in italics when referring to the project.
- Use single backticks for code literals and fenced MyST directives for Sphinx features.
- Preserve explicit labels and use `{ref}` and `{doc}` roles for internal links.
- Put Python autodoc directives in `{eval-rst}` fences: Sphinx emits their content
  as reStructuredText. Keep prose and C++ directives in MyST.
- Use rumdl's MyST-aware checks.

## Publications

- Keep publication metadata in `docs/references.bib`. Preserve existing citation keys.
- Cite entries with `{cite:p}` or `{cite:t}` roles. `docs/publications.md` renders the
  bibliography and provides its BibTeX download.
- Keep algorithm and experiment links beside the corresponding citations. Related
  publications do not imply that their methods are implemented in the library.
- Run `prek run bibtex-tidy --all-files` after changing the bibliography.

## The changelog

`docs/changelog.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Every user-facing change adds a bullet to the `Unreleased` section under `Added`,
`Changed`, `Fixed`, or `Removed`, matching the existing category order and bullet style.

Use one or two sentences. Aim for 90 characters and keep each entry at 350 or
fewer. Say what changed and what users need to do. Keep implementation rationale
in the code or pull request. A typical entry reads "`hexagonalization` now rejects
empty layouts."

Every entry is grouped under a category bullet — `Algorithms`, `Build system`, `CLI`,
`Code quality`, `Continuous integration`, `Data structures`, `Dependencies`,
`Documentation`, `Experiments`, `Gate libraries`, `I/O`, `Python bindings`, `Tooling` —
with the actual entries as nested sub-bullets. Do not add flat top-level bullets. Order
the categories alphabetically within each of `Added`, `Changed`, `Fixed`, and `Removed`,
as listed above.
