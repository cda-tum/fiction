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

will fetch the latest version of all external modules used.

Afterwards, *fiction* is ready to be built.


Using *fiction* as a stand-alone CLI tool
-----------------------------------------

It is possible to compile *fiction* as a stand-alone CLI tool.
The build system CMake can be invoked from the command line as follows::

  mkdir build
  cd build
  cmake ..
  make

Several options can be toggled during the build. For a more interactive interface, please refer to ``ccmake`` for a full list of supported customizations.

The CLI tool can then be run using::

  cli/fiction

See :ref:`cli` for a full user guide.

.. _header-only:

Using *fiction* as a header-only library
----------------------------------------

All data types and algorithms provided by *fiction* can be used independently by simply including its ``include/`` directory into a source build tree and using, e.g.,

.. code-block:: c++

   #include <fiction/layouts/cell_level_layout.hpp>
   #include <fiction/layouts/clocking_scheme.hpp>
   #include <fiction/technology/qca_one_library.hpp>
   #include <fiction/io/write_qca_layout.hpp>
   #include <fiction/...>

for each used file. Everything that can safely be used is located inside the ``fiction`` namespace.

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
