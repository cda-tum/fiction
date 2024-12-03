# Python bindings for the _fiction_ library

This directory contains Python bindings for the _fiction_ library built
with [pybind11](https://github.com/pybind/pybind11).

## Installation

The bindings can either be built and installed automatically with `pip` or built with `CMake` and installed manually.

### Set up a virtual environment

In order to set up a virtual environment on UNIX-like systems, you can use the following commands:

```bash
python3 -m venv venv
source venv/bin/activate
```

If you are using Windows, you can use the following commands instead:

```batch
python3 -m venv venv
venv\Scripts\activate.bat
```

### Set up `nox`

[`nox`](https://nox.thea.codes/en/stable/index.html) can be used to conveniently run many development tasks like
testing the bindings on multiple Python installations.

```bash
(venv) $ pipx install nox
```

If you use macOS, then `nox` is in `brew`, use `brew install nox`.

> [!NOTE]
> If you do not have [`pipx`](https://pypa.github.io/pipx/) (pip for applications) installed, you can install it with:
>
> ```bash
> (venv) $ pip install pipx
> (venv) $ pipx ensurepath
> ```

If you use macOS, then `pipx` is in `brew`, use `brew install pipx`.

### Install `pre-commit`

Install [`pre-commit`](https://pre-commit.com/) to automatically run a set of checks before each commit.

```bash
(venv) $ pipx install pre-commit
(venv) $ pre-commit install
```

If you use macOS, then `pre-commit` is in `brew`, use `brew install pre-commit`.

You can also run the checks manually:

```bash
(venv) $ pre-commit run --all-files
```

### Install Z3

Make sure to have the SMT Solver [`Z3 >= 4.8.0`](https://github.com/Z3Prover/z3) installed. This can be accomplished in
a multitude of ways:

- Under Ubuntu 20.04 and newer: `sudo apt-get install libz3-dev`.
- Under macOS: `brew install z3`.
- Alternatively: `pip install z3-solver` in the virtual environment.
- Download pre-built binaries from https://github.com/Z3Prover/z3/releases and copy the files to the respective system
  directories.
- Build Z3 from source and install it to the system.

### Building the bindings with `pip`

```bash
(venv) $ pip install .
```

### Building the bindings with `uv` (faster)

```bash
(venv) $ pipx install uv
(venv) $ uv pip install .
```

### Building the bindings with CMake

```bash
cmake . -B build -DFICTION_PYTHON_BINDINGS=ON
cd build
cmake --build . -j4
```

### Running Python tests

A `nox` session is provided to conveniently run the Python tests.

```bash
(venv) $ nox -s tests
```

This installs all dependencies for running the tests in an isolated environment, builds the Python package, and then
runs the tests.

## Usage

The bindings are available as a Python module named `pyfiction` in the namespace `mnt`. To use the bindings, simply
import the module in your Python script:

```python
from mnt import pyfiction
```

The bindings are documented using [pybind11's mkdoc utility tool](https://github.com/pybind/pybind11_mkdoc). To see
the documentation for a function, simply call `help` on it:

```python
help(pyfiction.cartesian_layout)
```

## Extending the bindings

We try to keep the bindings as close to the C++ API as possible. However, due to language differences, some
functionality might not be available in Python or differ slightly in its syntax compared to its C++ counterpart.

Our goal is to expose as much of _fiction_'s library features in some way in `pyfiction` as well. Therefore, whenever a
new feature is added to the C++ library, please also add the respective bindings here. To not duplicate
docstrings and run out of sync, we use the `pybind11_mkdoc` tool to automatically generate the documentation from the
C++ docstrings.

A few things must be noted when adding new bindings:

- Do not use `""_a` literals in the bindings. Instead, use `py::arg` to specify the argument names.
- Add new symbols to the `__init__.py` file's `import` and `__all__` statements.
- Do not use `from mnt.pyfiction import *` in the Python code. Instead, use explicit imports like
  `from mnt.pyfiction import cartesian_layout`. This speeds up the import process and helps narrow down the origin of a
  failing test.

### Docstrings

The auto-generated file `include/pybind11_mkdoc_docstrings.hpp` contains all docstrings extracted from the C++ code.
Since it does not contain include guards, we offer the (manually created)
file `include/documentation.hpp`, which acts as a wrapper around the aforementioned docstrings. Should it be necessary
to adjust certain docstrings for Python bindings, this can be done here.

> ⚠️ **Do not directly include the auto-generated file!** Due to the lack of header guards, it leads to compilation
> errors. Instead, only include `documentation.hpp`.

The docstrings are extended and updated automatically by the GitHub
Action [pyfiction-docstring-generator](https://github.com/cda-tum/fiction/actions/workflows/pyfiction-docstring-generator.yml),
which regenerates `pybind11_mkdoc_docstrings.hpp` and overrides the existing one every time changes to
any `*.hpp` file are pushed to a branch.

Alternatively, you can also run `pybind11_mkdoc` locally using the `pybind11_mkdoc` tool (not recommended):

```bash
pip install pybind11_mkdoc clang==14
```

To generate the docstrings call

```bash
python3 -m pybind11_mkdoc -o pybind11_mkdoc_docstrings.hpp -D FICTION_Z3_SOLVER `find ./include/fiction -name "*.hpp" -print`
```

in _fiction_'s base directory.
