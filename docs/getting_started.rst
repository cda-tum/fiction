Getting started
===============

The *fiction* framework provides a stand-alone CLI tool as well as a C++17 header-only library and a Python module which
can be used in external projects. Additionally, we provide an experimentation playground that can be used to quickly
prototype new ideas or script evaluations.

We are continuously testing on Ubuntu, macOS, and Windows with multiple compilers and various Python versions.
See the badges in the README file for more information.


Compilation requirements
------------------------

The repository should always be cloned recursively with all submodules::

  git clone --recursive https://github.com/cda-tum/fiction.git

Several third-party libraries will be cloned within the ``libs`` folder. The ``cmake`` build process will take care of
them automatically. Should the repository have been cloned before, the commands::

  git submodule update --init --recursive

will fetch the latest version of all external modules used. Additionally, only ``CMake`` and a C++17 compiler are required.

At the time of writing, for parallel STL algorithms to work when using GCC, the TBB library (``libtbb-dev`` on Ubuntu) is
needed. It is an optional dependency that can be installed for a performance boost in certain scenarios. For your
preferred compiler, see the current implementation state of `P0024R2 <https://en.cppreference.com/w/cpp/compiler_support/17>`_.


.. _cli:

Using *fiction* as a stand-alone CLI tool
-----------------------------------------

It is possible to compile *fiction* as a stand-alone CLI tool. For auto-completion in the CLI, it is recommended but not
required to install the ``libreadline-dev`` package.
The build system CMake can be invoked from the command line as follows::

  cmake . -B build
  cd build
  cmake --build . -j4

Several options can be toggled during the build. For a more interactive interface, please refer to ``ccmake`` for a
full list of supported customizations.

The CLI tool can then be run using::

  cli/fiction


Here is an example of running *fiction* to perform a full physical design flow on a QCA circuit layout that can
afterward be simulated in QCADesigner:

.. figure:: /_static/fiction_cli_example.gif
   :alt: CLI example
   :align: center

See :ref:`cli` for a full user guide.


As an alternative to this build workflow, the CLI tool is also available as an image for a
`Docker container <https://www.docker.com/>`_. The respective ``Dockerfile`` is available in the base directory.

.. _header-only:


Using *fiction* as a header-only library
----------------------------------------

All data types and algorithms provided by *fiction* can be used independently to build custom projects. The following
snippets illustrate how *fiction* can be included and linked against your project. Assume, the project
is called *fanfiction*. Clone *fiction* within the project folder ``fanfiction/`` and add the following lines of code
to your ``CMakeLists.txt``::

    add_subdirectory(fiction/)
    target_link_libraries(fanfiction libfiction)

.. note::

    The command ``target_link_libraries`` must be called after the respective ``add_executable`` statement that defines
    ``fanfiction``.

    By default *fiction*'s CLI is enabled and will be built, which can be time-consuming. If you do not need it, you can
    disable it by passing ``-DFICTION_CLI=OFF`` to your ``cmake`` call or adding
    ``set(FICTION_CLI OFF CACHE BOOL "" FORCE)`` **before** ``add_subdirectory(fiction/)``.

Within your code files, you can then call

.. code-block:: c++

   #include <fiction/layouts/cell_level_layout.hpp>
   #include <fiction/layouts/clocking_scheme.hpp>
   #include <fiction/technology/qca_one_library.hpp>
   #include <fiction/io/write_qca_layout.hpp>
   #include <fiction/...>

for each used header file to include *fiction*'s data types and algorithms. Everything that can safely be used is
directly located inside the ``fiction`` namespace.


Python Bindings
---------------

The Python bindings can be installed via ``pip`` from `PyPI <https://pypi.org/project/mnt.pyfiction/>`_ where we publish
wheels for every new release::

  pip install mnt.pyfiction

You can then import the bindings in your Python project:

.. code-block:: python

  from mnt import pyfiction

The Python synopsis is modeled after the C++ API to make it feel as familiar as possible. However, all available Python
bindings are additionally documented together with the C++ code on this site to make it easier to get started. For each
module, you can toggle between the two languages using the tabs.

.. note::

    The *fiction* framework is primarily developed for C++ as a header-only library. The Python bindings are a thin
    wrapper around the C++ code. We try our best to keep the bindings in sync with the C++ code, and to expose most of
    *fiction*'s functionality in both C++ and Python. This is, unfortunately, not always possible. Should you encounter
    features that are not (yet) available in *pyfiction*, please open
    an `issue on GitHub <https://github.com/cda-tum/fiction/issues>`_.


Enabling dependent functions
----------------------------

Some functionalities require the presence of third-party dependencies. In the following, it is discussed how to enable
them.

SMT-based ``exact`` P&R
#######################

The :ref:`exact placement and routing algorithm <exact>` utilizes the `SMT solver Z3 <https://github.com/Z3Prover/z3>`_.
Follow the `installation instructions <https://github.com/Z3Prover/z3/blob/master/README-CMake.md>`_ and call
``sudo make install`` to install headers, scripts, and the binary.

.. note::
   Be sure to compile Z3 in **release mode** to avoid performance issues when running *fiction*'s dependent functions!
   This can be achieved by passing ``-DCMAKE_BUILD_TYPE=Release`` to Z3's ``cmake`` call.

Finally, before building *fiction*, pass ``-DFICTION_Z3=ON`` to the ``cmake`` call. It should be able to find
Z3's include path and link against the binary automatically if installed correctly. Otherwise, you can use
``-DZ3_ROOT=<path_to_z3_root>`` to set Z3's root directory that is to be searched for the installed solver.

SAT-based ``onepass`` synthesis
###############################

The :ref:`one-pass synthesis algorithm <onepass>` is embedded via the Python3 script
`Mugen <https://github.com/whaaswijk/mugen>`_ by Winston Haaswijk using `pybind11 <https://github.com/pybind/pybind11>`_.
It has some further Python dependencies that can be installed via ``pip3``::

    pip install -r libs/mugen/requirements.txt

The Python integration is experimental and may cause issues on some systems. It is currently not available on Windows
and some macOS versions due to issues with ``python-sat``. Mugen requires at least Python 3.7!

Finally, before building *fiction*, pass ``-DFICTION_ENABLE_MUGEN=ON`` to the ``cmake`` call.


Building experiments
--------------------

The ``experiments`` folder provides a playground for quickly scripting some ideas by plugging algorithms together.
A ``fictionlib_demo.cpp`` demonstrates the usage. Any ``*.cpp`` file that is placed in on of its sub-folders is
automatically linked against ``libfiction`` and compiled as a stand-alone binary. Simply add a ``main`` function and
include the desired header files to get started:

.. code-block:: c++

   #include <fiction/layouts/cell_level_layout.hpp>
   #include <fiction/layouts/clocking_scheme.hpp>
   #include <fiction/technology/qca_one_library.hpp>
   #include <fiction/io/write_qca_layout.hpp>
   #include <fiction/...>

   int main(int argc, char* argv[])
   {
     // your code goes here
   }


Each file can be built individually via CMake::

  cmake . -B build -DFICTION_EXPERIMENTS=ON
  cd build
  cmake --build . -j4


Building tests
--------------

Unit tests can be built with CMake via a respective flag on the command line and executed via ``ctest``::

  cmake . -B build -DFICTION_TEST=ON
  cd build
  cmake --build . -j4
  ctest


Building experiments
--------------------

The ``experiments`` folder provides a playground for quickly scripting some ideas by plugging algorithms together.
A ``fictionlib_demo.cpp`` demonstrates the usage. Any ``*.cpp`` file that is placed in that folder is automatically
linked against *fiction* and compiled as a stand-alone binary using the following commands::

  cmake . -B build -DFICTION_EXPERIMENTS=ON
  cd build
  cmake --build . -j4


Building code benchmarks
------------------------

Using ``Catch2``'s micro-benchmarking feature, you can compile and run code tests that evaluate the performance of
certain code constructs. The ``test/benchmark`` folder provides a selection of benchmarks we were running to evaluate
the performance of our code during development. Any ``*.cpp`` file that is placed in that folder is automatically
linked against *fiction* and compiled as a stand-alone binary using the following commands::

  cmake . -B build -DFICTION_BENCHMARK=ON
  cd build
  cmake --build . -j4


Noteworthy CMake options
------------------------

The following CMake options are available which have a potential positive impact on the build process, debugging
attempts, or performance of the resulting binaries:

* ``-DFICTION_ENABLE_IPO=ON``: Enable IPO/LTO to improve performance of resulting binaries on some systems.
* ``-DFICTION_ENABLE_PCH=ON``: Enable precompiled headers (PCH) to speed up compilation.
* ``-DFICTION_ENABLE_UNITY_BUILD=ON``: Enable unity builds to speed up compilation.
* ``-DFICTION_ENABLE_SANITIZER_ADDRESS=ON``: Enable the address sanitizer to detect memory issues.
* ``-DFICTION_ENABLE_SANITIZER_LEAK=ON``: Enable the leak sanitizer to detect memory leaks.
* ``-DFICTION_ENABLE_SANITIZER_UNDEFINED=ON``: Enable the undefined behavior sanitizer to detect undefined behavior.
* ``-DFICTION_ENABLE_SANITIZER_THREAD=ON``: Enable the thread sanitizer to detect multithreading-related problems.
* ``-DFICTION_ENABLE_SANITIZER_MEMORY=ON``: Enable the memory sanitizer to detect uninitialized reads.

Uninstall
---------

Since all tools were built locally, simply delete the git folder cloned initially to uninstall this project.
