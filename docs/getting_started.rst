Getting started
===============

The *fiction* framework provides stand-alone CLI tool as well as a header-only library that can be used in external projects.
Both are written in C++17 and are continuously tested on ubuntu, macOS, and Windows with multiple compilers. See the build badges in the README file for more information.


Compilation requirements
------------------------

The repository should always be cloned recursively with all submodules::

  git clone --recursive

Several third-party libraries will be cloned within the ``libs`` folder. The ``cmake`` build process will take care of
them automatically. Should the repository have been cloned before, the commands::

  git submodule update --init --recursive

will fetch the latest version of all external modules used. Additionally, only ``CMake`` and a C++17 compiler are required.

At the time of writing, for parallel STL algorithms to work when using GCC, the TBB library (``libtbb-dev`` on ubuntu) is
needed. It is an optional dependency that can be installed for a performance boost in certain scenarios. For your
preferred compiler, see the current implementation state of `P0024R2 <https://en.cppreference.com/w/cpp/compiler_support/17>`_.


Using *fiction* as a stand-alone CLI tool
-----------------------------------------

It is possible to compile *fiction* as a stand-alone CLI tool. For auto-completion in the CLI, it is recommended but not
required to install the ``libreadline-dev`` package.
The build system CMake can be invoked from the command line as follows::

  mkdir build
  cd build
  cmake ..
  make

Several options can be toggled during the build. For a more interactive interface, please refer to ``ccmake`` for a
full list of supported customizations.

The CLI tool can then be run using::

  cli/fiction

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

Note that ``target_link_libraries`` must be called after the respective ``add_executable`` statement that defines
``fanfiction``. Within your code files, you can then call

.. code-block:: c++

   #include <fiction/layouts/cell_level_layout.hpp>
   #include <fiction/layouts/clocking_scheme.hpp>
   #include <fiction/technology/qca_one_library.hpp>
   #include <fiction/io/write_qca_layout.hpp>
   #include <fiction/...>

for each used header file to include *fiction*'s data types and algorithms. Everything that can safely be used is
directly located inside the ``fiction`` namespace.


Enabling dependent functions
----------------------------

Some functionalities require the presence of third-party dependencies. In the following, it is discussed how to enable
them.

SMT-based ``exact`` P&R
#######################

The :ref:`exact placement and routing algorithm <exact>` utilizes the `SMT solver Z3 <https://github.com/Z3Prover/z3>`_.
Follow the `installation instructions <https://github.com/Z3Prover/z3/blob/master/README-CMake.md>`_ and make sure to call
``sudo make install`` to install headers, scripts, and the binary.

Finally, before building *fiction*, pass ``-DFICTION_Z3=ON`` to the ``cmake`` call. It should be able to find
Z3's include path and link against the binary automatically if installed correctly. Otherwise, you can use
``-DFICTION_Z3_SEARCH_PATHS=<path_to_z3>`` to set a list of locations that are to be searched for the installed solver.

SAT-based ``onepass`` synthesis
###############################

The :ref:`one-pass synthesis algorithm <onepass>` is embedded via the Python3 script
`Mugen <https://github.com/whaaswijk/mugen>`_ by Winston Haaswijk using `pybind11 <https://github.com/pybind/pybind11>`_.
It has some further Python dependencies that can be installed via ``pip3``::

    pip3 install python-sat==0.1.6.dev6 wrapt_timeout_decorator graphviz

The Python3 integration is experimental and may cause issues on some systems. It is currently not available on Windows
and some macOS versions due to issues with ``python-sat``. Mugen requires at least Python 3.7!

Finally, before building *fiction*, pass ``-DFICTION_ENABLE_MUGEN=ON`` to the ``cmake`` call.


Building tests
--------------

Unit tests can be built with CMake via a respective flag on the command line and executed via ``ctest``::

  mkdir build
  cd build
  cmake -DFICTION_TEST=ON ..
  make
  ctest


Building experiments
--------------------

The ``experiments`` folder provides a playground for quickly scripting some ideas by plugging algorithms together.
A ``fictionlib_demo.cpp`` demonstrates the usage. Any ``*.cpp`` file that is placed in that folder is automatically
linked against *fiction* and compiled as a stand-alone binary using the following commands::

  mkdir build
  cd build
  cmake -DFICTION_EXPERIMENTS=ON ..
  make


Uninstall
---------

Since all tools were built locally, simply delete the git folder cloned initially to uninstall this project.
