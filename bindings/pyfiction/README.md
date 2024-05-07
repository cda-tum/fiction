# Python bindings for the *fiction* library

This directory contains Python bindings for the *fiction* library generated
with [pybind11](https://github.com/pybind/pybind11).

## Installation

The bindings can either be built and installed automatically with `pip` or built with `CMake` and installed manually.

### Building the bindings with `pip`

```bash
pip install .
```

If you want to include the functions dependent on the [Z3 Solver](https://github.com/Z3Prover/z3), set `z3=ON` before
running `pip`:

```bash
z3=ON pip install .
```

See [the documentation](https://fiction.readthedocs.io/en/latest/getting_started.html#enabling-dependent-functions) for
more information on dependent functions.

#### Bindings for *ClusterComplete*

On Linux, the bindings can be installed from the PyPI repository which includes all dependent functionality, including
*ClusterComplete*, which depends on ALGLIB. On Windows and macOS, these bindings can be enabled through a local
installation using `alglib=ON`, similar as above. This may be enabled alongside `z3=ON` by separating the definitions
with a space before the `pip` call.

### Building the bindings with CMake

```bash
cmake . -B build -DFICTION_PYTHON_BINDINGS=ON
cd build
cmake --build . -j4
```

### Enabling and running tests

To run tests locally, build the binding with its dependencies using `pip`:

```bash
pip install '.[test]'
```

Alternatively, install the following dependency before the `cmake` call:

```bash
pip install python-dotenv==0.21.1
```

To execute all tests, use the following command inside the package directory:

```bash
 python -m unittest discover --verbose
```

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

Our goal is to expose as much of *fiction*'s library features in some way in `pyfiction` as well. Therefore, whenever a
new feature is added to the C++ library, please also add the respective bindings here. To not duplicate
docstrings and run out of sync, we use the `pybind11_mkdoc` tool to generate the documentation from the C++ docstrings.

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
python3 -m pybind11_mkdoc -o pybind11_mkdoc_docstrings.hpp -D FICTION_Z3_SOLVER -D FICTION_ALGLIB_ENABLED `find ./include/fiction -name "*.hpp" -print`
```

in *fiction*'s base directory.
