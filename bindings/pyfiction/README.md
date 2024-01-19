# Python bindings for the *fiction* library

This directory contains Python bindings for the *fiction* library generated
with [pybind11](https://github.com/pybind/pybind11).

## Installation

The bindings can either be build and installed automatically with `pip` or build with `CMake` and manually installed.

### Building the bindings with pip

```bash
pip install .
```

If you want to include the functions dependent on the Z3 solver, set `z3=ON` before running `pip`:

```bash
z3=ON pip install .
```

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

Alternatively, install the following dependency before the `CMake` call:

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
new feature is added to the C++ library, please also add the respective bindings here. In order to not duplicate
docstrings and run out of sync, we use the `pybind11_mkdoc` tool to generate the documentation from the C++ docstrings.

### Docstrings

There is an auto-generated file called `include/pybind11_mkdoc_docstrings.hpp`. It contains all docstrings extracted
from the C++ code. Since it does not have include guards, we offer the (manually created)
file `include/documentation.hpp`, which acts as a wrapper around the aforementioned docstrings.

> ⚠️ **Do not directly include the auto-generated file!** Due to the lack of header guards, it leads to compilation
> errors. Instead, only include `documentation.hpp`.

To extend the docstrings, regenerate `pybind11_mkdoc_docstrings.hpp` and override the existing one. The easiest way to
do this is to use the GitHub Action `pyfiction-docstring-generator`. Alternatively, you can also run `pybind11_mkdoc`
locally.

#### Use the GitHub Action (preferred)

Go to
the [pyfiction-docstring-generator](https://github.com/cda-tum/fiction/actions/workflows/pyfiction-docstring-generator.yml)
workflow on GitHub and run it. Upon completion, it will make the `pybind11_mkdoc_docstrings.hpp` file available as an
artifact. Download the file and replace the existing one.

#### Run locally

Setup the `pybind11_mkdoc` tool:

```bash
pip install pybind11_mkdoc clang==14
```

To generate the docstrings call

```bash
python3 -m pybind11_mkdoc -o pybind11_mkdoc_docstrings.hpp -D FICTION_Z3_SOLVER `find ./include/fiction -name "*.hpp" -print`
```

in *fiction*'s base directory.
