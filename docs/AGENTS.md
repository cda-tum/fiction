# AGENTS.md — documentation

`docs/` is reStructuredText built by Sphinx, plus Doxygen for the C++ API. There is no
local docs session in `noxfile.py`; Read the Docs builds a preview on every pull request.

## Adding a page

- A new page needs a `.rst` file **and** a line in the matching `toctree` in
  `docs/index.rst`. A page that is not in a `toctree` builds without an error and is
  unreachable.
- API documentation comes from the C++ sources through Doxygen directives —
  `.. doxygenfunction::`, `.. doxygenclass::`, `.. doxygenstruct::`, `.. doxygenenum::`.
  Do not retype a signature or a parameter description into the `.rst` file; fix the
  Doxygen comment in the header instead.
- Where a feature exists in both C++ and Python, document both under a `.. tabs::` block,
  following `algorithms/design_sidb_gates.rst`.

## Prose

The `Writing` rules in the root `AGENTS.md` apply here in full. In addition:

- Write _fiction_ in italics when referring to the project.
- Code literals in reStructuredText take **two** backticks, not one. A single backtick is
  interpreted-text markup and renders as italics, not code — the `rst-backticks` prek hook
  catches this.
- The three `rst-*` prek hooks catch the usual reStructuredText mistakes (single
  backticks, missing directive colons, inline markup touching text). Run `prek run -a`
  after editing a page.

## The changelog

`docs/changelog.rst` follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Every user-facing change adds a bullet to the `Unreleased` section under `Added`,
`Changed`, `Fixed`, or `Removed`, matching the existing category order and bullet style.

This file is the **one exception** to the root rule "write for the final design, not the
history of how you got there":

- Entries here are deliberately long and explain the reasoning, not just the outcome.
- Record an approach that was tried and reverted, together with the reason it failed,
  whenever a future reader would otherwise retry it. The `std::jthread` revert
  (Apple's libc++ gates `<stop_token>`) is the model to follow.
- Do not record the review process, prompts, or intermediate commits. The distinction is
  whether the information saves the next person from a dead end.
