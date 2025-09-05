Getting started
===============

The *fiction* framework provides a stand-alone CLI tool as well as a C++17 header-only library and a Python module which
can be used in external projects. Additionally, we provide an experimentation playground that can be used to quickly
prototype new ideas or script evaluations.

We are continuously testing on Ubuntu, macOS, and Windows with multiple compilers and various Python versions.
See the badges in the README file for more information.

Quick Start
-----------

To help you getting started with *fiction*, pick the interface that best fits your use case:

.. list-table::
   :widths: 40 25 35
   :header-rows: 1

   * - Goal
     - Recommended Path
     - Section
   * - Try the tool immediately
     - 🐳 Docker CLI image
     - :ref:`CLI (Docker) <cli-docker>`
   * - Full-featured local CLI build
     - 💻 Native build
     - :ref:`CLI (Source) <cli-source>`
   * - Integrate into a C++ project
     - 📚 Header-only library
     - :ref:`C++ Library <cpp-library>`
   * - Script / notebooks / rapid prototyping
     - 🐍 Python bindings (PyPI)
     - :ref:`Python Bindings <python-bindings>`

For a full CLI command list or API reference, see the respective documentation sections.

.. _cli:

CLI (Docker)
------------

This is the fastest zero-install path. We release pre-built images of the latest CLI
on `Docker Hub <https://hub.docker.com/r/mawalter/fiction>`_. Make sure you
have `Docker installed <https://docs.docker.com/get-docker/>`_ on your local system.

Pull the latest image:

.. code-block:: console

  $ docker pull mawalter/fiction:latest

Run the interactive CLI session:

.. code-block:: console

  $ docker run --rm -it mawalter/fiction

Internally, the repository lives at ``/app/fiction``.

CLI (Source)
------------

When you want to add your own algorithms or contribute to the project, you should build *fiction* from source.

Compilation requirements
########################

The repository should always be cloned recursively with all submodules:

.. code-block:: console

  $ git clone --recursive https://github.com/cda-tum/fiction.git
  $ cd fiction

Several third-party libraries will be cloned within the ``libs`` folder. The ``cmake`` build process will take care of
them automatically. Should the repository have been cloned before, the commands::

  git submodule update --init --recursive

will fetch the latest version of all external modules used. Additionally, only ``CMake`` and a C++17 compiler are
required for the C++ part. If you want to work with the Python bindings, you need a Python 3.9+ installation.

At the time of writing, for parallel STL algorithms to work when using GCC, the TBB library (``libtbb-dev`` on Ubuntu) is
needed. It is an optional dependency that can be installed for a performance boost in certain scenarios. For your
preferred compiler, see the current implementation state of `P0024R2 <https://en.cppreference.com/w/cpp/compiler_support/17>`_.

On Ubuntu, all required and optional dependencies can be installed via::

  sudo apt-get install build-essential cmake Python3 libreadline-dev libtbb-dev

Building the CLI
################

For auto-completion in the CLI, it is recommended but not required to install the ``libreadline-dev`` package (see above).

Configure and build with CMake:

.. code-block:: console

  $ cmake -S . -B build
  $ cmake --build build -j$(nproc)

Several options can be toggled during the build. For a more interactive interface, please refer to ``ccmake`` for a
full list of supported customizations.

Run the CLI:

.. code-block:: console

  $ build/cli/fiction

Here is an example of running *fiction* to perform a full physical design flow on a QCA circuit layout that can
afterward be simulated in QCADesigner:

.. figure:: /_static/fiction_cli_example.gif
   :alt: CLI example
   :align: center

See :ref:`cli` for a full user guide.

.. _header-only:

C++ Library
-----------

If you want to use *fiction* as a dependency in your project to utilize its header-only library for your own tool.

Add *fiction* as a sub-directory to your CMake project and link against ``libfiction`` (assuming your project is
called *fanfiction*):

.. code-block:: cmake

    add_subdirectory(fiction)
    target_link_libraries(fanfiction PRIVATE libfiction)

.. note::

    The command ``target_link_libraries`` must be called after the respective ``add_executable`` statement that defines
    ``fanfiction``.

    By default *fiction*'s CLI is enabled and will be built, which can be time-consuming. If you do not need it, you can
    disable it by passing ``-DFICTION_CLI=OFF`` to your ``cmake`` call or adding
    ``set(FICTION_CLI OFF CACHE BOOL "" FORCE)`` **before** ``add_subdirectory(fiction/)``.

Then include what you need:

.. code-block:: c++

   #include <fiction/layouts/cell_level_layout.hpp>
   #include <fiction/layouts/clocking_scheme.hpp>
   #include <fiction/technology/qca_one_library.hpp>
   #include <fiction/io/write_qca_layout.hpp>
   #include <fiction/...>

Everything that can safely be used is directly located inside the ``fiction`` namespace.

.. _python-bindings:

Python Bindings
---------------

Ideal for notebooks, exploratory scripts, and integration with Python tooling.

Install the library from PyPI:

.. code-block:: console

  $ pip install mnt.pyfiction

Import it in your script:

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

Virtual Environment Setup
##########################

In order to set up a virtual environment on UNIX-like systems, you can use the following commands:

.. code-block:: console

    $ python3 -m venv venv
    $ source venv/bin/activate

If you are using Windows, you can use the following commands instead:

.. code-block:: console

    $ python3 -m venv venv
    $ venv\Scripts\activate.bat

---

Advanced Configuration
======================

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
   Be sure to compile Z3 in **Release mode** to avoid performance issues when running *fiction*'s dependent functions!
   This can be achieved by passing ``-DCMAKE_BUILD_TYPE=Release`` to Z3's ``cmake`` call.

Finally, before building *fiction*, pass ``-DFICTION_Z3=ON`` to the ``cmake`` call. It should be able to find
Z3's include path and link against the binary automatically if installed correctly. Otherwise, you can use
``-DZ3_ROOT=<path_to_z3_root>`` to set Z3's root directory that is to be searched for the installed solver.

SAT-based ``onepass`` synthesis
###############################

The :ref:`one-pass synthesis algorithm <onepass>` is embedded via the Python3 script
`Mugen <https://github.com/whaaswijk/mugen>`_ by Winston Haaswijk using `pybind11 <https://github.com/pybind/pybind11>`_.
It has some further Python dependencies that can be installed via ``pip3``:

.. code-block:: console

    (venv) $ pip install -r libs/mugen/requirements.txt

The Python integration is experimental and may cause issues on some systems. It is currently not available on Windows
and some macOS versions due to issues with ``python-sat``. Mugen requires at least Python 3.7!

Finally, before building *fiction*, pass ``-DFICTION_ENABLE_MUGEN=ON`` to the ``cmake`` call.

.. _abc-cmake:

ABC callback
############

`ABC <https://github.com/berkeley-abc/abc/>`_ by Alan Mishchenko can be used as a callback for logic synthesis and
optimization from within the *fiction* CLI. It must be compiled and installed manually and can be enabled by passing
``-DFICTION_ABC=ON`` to the ``cmake`` call. If ABC is not in your ``PATH``, you can specify the path to the folder
where the ``abc`` binary is located by passing ``-DABC_ROOT=<path_to_abc_root>`` to the ``cmake`` call. On the other
hand, if you installed ABC in a default location on UNIX-like operating systems (e.g., ``/usr/bin/``
or ``/usr/local/bin/``), it should be detected automatically without the need to pass the root directory.

.. note::
   Be sure to compile ABC in **Release mode** to avoid performance issues during synthesis and optimization!
   This can be achieved by passing ``-DCMAKE_BUILD_TYPE=Release`` to ABC's ``cmake`` call.

For information on usage, see the :ref:`ABC callback <abc-cli>` section in the CLI documentation.

ALGLIB-dependent ``ClusterComplete`` exact SiDB simulation
##########################################################

The :ref:`ClusterComplete <clustercomplete>` exact SiDB simulation algorithm relies on functionality offered by
`ALGLIB <https://www.alglib.net>`_ by the ALGLIB Project.
When enabled, it will be downloaded automatically and linked against *fiction*.

To enable it, before building *fiction*, pass ``-DFICTION_ALGLIB=ON`` to the ``cmake`` call.


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


Each file can be built individually via CMake:

.. code-block:: console

  $ cmake -S . -B build -DFICTION_EXPERIMENTS=ON
  $ cmake --build build -j$(nproc)


Building tests
--------------

Unit tests can be built with CMake via a respective flag on the command line and executed via ``ctest``:

.. code-block:: console

  $ cmake -S . -B build -DFICTION_TEST=ON
  $ cmake --build build -j$(nproc)
  $ ctest



Building code benchmarks
------------------------

Using ``Catch2``'s micro-benchmarking feature, you can compile and run code tests that evaluate the performance of
certain code constructs. The ``test/benchmark`` folder provides a selection of benchmarks we were running to evaluate
the performance of our code during development. Any ``*.cpp`` file that is placed in that folder is automatically
linked against *fiction* and compiled as a stand-alone binary using the following commands:

.. code-block:: console

  $ cmake -S . -B build -DFICTION_BENCHMARK=ON
  $ cmake --build build -j$(nproc)


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
* ``-DFICTION_ENABLE_JEMALLOC=ON``: Enable the usage of jemalloc by Jason Evans to speed up ``malloc`` in parallelized processes.

Usage of jemalloc
#################

While enabling jemalloc through the above CMake is not beneficial to every application (and may add runtime due to overhead), it can bring significant runtime improvements to some applications. In particular, it is recommended to use jemalloc for parallelized applications in which allocations are predominantly non-ephemeral.

.. note::
   Windows users need to install jemalloc manually. It can be done by following `these steps <https://github.com/jemalloc/jemalloc/blob/dev/INSTALL.md#building-for-windows>`_.

.. note::
   Usage of jemalloc in *fiction* is not tested on macOS with the g++ compiler.

Uninstall
---------

Since all tools were built locally, simply delete the git folder cloned initially to uninstall this project.
