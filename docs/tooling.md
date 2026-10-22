# Tooling

This page lists the tools used to develop _fiction_. For build requirements and
presets, see {doc}`getting_started`. For contribution and review commands, see
{doc}`contributing`.

## C++

| Tool | Purpose | Configuration and use |
| --- | --- | --- |
| [CMake](https://cmake.org/) and [Ninja](https://ninja-build.org/) | Configure and build the library's tests, experiments, and Python bindings. | `CMakeLists.txt` and `CMakePresets.json`; see {ref}`cmake-presets`. |
| [Catch2](https://github.com/catchorg/Catch2) and [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) | Write and run C++ tests and micro-benchmarks. | `test/`; see {ref}`building-tests`. |
| [clang-format](https://clang.llvm.org/docs/ClangFormat.html) | Format C++ code. | `.clang-format`; runs through prek. |
| [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) | Analyze C++ code. | `.clang-tidy`; `uvx nox -s cpp_lint -- --all` runs the CI configuration. |
| [ccache](https://ccache.dev/) | Cache compilations. | `FICTION_ENABLE_CACHE`; see {ref}`cmake-presets` for the PCH setting. |

## Python bindings and packaging

| Tool | Purpose | Configuration and use |
| --- | --- | --- |
| [nanobind](https://nanobind.readthedocs.io/) | Expose C++ APIs to Python and generate type stubs. | `bindings/` and `python/mnt/pyfiction/`; see {ref}`bindings-architecture`. `uvx nox -s stubs` regenerates stubs. |
| [scikit-build-core](https://scikit-build-core.readthedocs.io/) | Build Python packages with CMake. | `pyproject.toml`. |
| [cibuildwheel](https://cibuildwheel.pypa.io/) | Build and test platform wheels. | `pyproject.toml` and `.github/workflows/cd.yml`. |
| [uv](https://docs.astral.sh/uv/) | Manage Python dependencies and isolated environments. | `pyproject.toml` and `uv.lock`. |
| [nox](https://nox.thea.codes/) | Run tests, lint, documentation, and stub-generation sessions. | `noxfile.py`; `uvx nox -l` lists sessions. |
| [pytest](https://docs.pytest.org/) | Run Python tests. | `test/python/` and `pyproject.toml`; `uvx nox -s tests` tests supported Python versions, and `uvx nox -s minimums` checks dependency floors. |
| [Ruff](https://docs.astral.sh/ruff/) and [ty](https://docs.astral.sh/ty/) | Format, lint, and type-check Python code. | `pyproject.toml`; run through prek. |

## Documentation

| Tool | Purpose | Configuration and use |
| --- | --- | --- |
| [Sphinx](https://www.sphinx-doc.org/), [MyST](https://myst-parser.readthedocs.io/), and [Furo](https://pradyunsg.me/furo/) | Build and style the documentation from Markdown. | `docs/conf.py`; see {ref}`building-documentation` for previews, HTML builds, and link checks. |
| [Doxygen](https://www.doxygen.nl/) and [Breathe](https://breathe.readthedocs.io/) | Render C++ API comments in Sphinx. | `docs/Doxyfile`; the docs session generates XML for Breathe. |
| [sphinxcontrib-bibtex](https://sphinxcontrib-bibtex.readthedocs.io/) and [bibtex-tidy](https://flamingtempura.github.io/bibtex-tidy/) | Render and format publication references. | `docs/references.bib`; see {ref}`citing-publications`. |
| [rumdl](https://rumdl.dev/) | Format and lint MyST Markdown. | `.rumdl.toml` and `.pre-commit-config.yaml`; run through prek. |

## CI and quality checks

| Tool | Purpose | Configuration and use |
| --- | --- | --- |
| [prek](https://prek.j178.dev/) | Run formatting and validation hooks. | `.pre-commit-config.yaml`; `prek install` installs the commit hook, and `prek run -a` checks all files. |
| [license-tools](https://github.com/emzeat/mz-lictools) | Maintain copyright headers. | `.license-tools-config.json`; run through prek instead of editing headers by hand. |
| [GitHub Actions](https://github.com/cda-tum/fiction/actions) | Run builds, tests, packaging, and static analysis. | `.github/workflows/`; see {doc}`contributing` for check names and review rules. |
| [Codecov](https://app.codecov.io/gh/cda-tum/fiction) | Report C++ and Python test coverage. | `.codecov.yml`. |
| [CodeRabbit](https://www.coderabbit.ai/) | Provide automated pull request reviews. | `.coderabbit.yaml`; see {ref}`code-review`. |
| [pre-commit.ci](https://pre-commit.ci/) | Run hooks and submit formatting fixes. | The `ci` section in `.pre-commit-config.yaml` skips ty; GitHub Actions runs the type check. |
| [Renovate](https://docs.renovatebot.com/) | Open dependency-update pull requests. | `.github/renovate.json5`. |
