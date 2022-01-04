Getting started
===============

This code base provides a framework for **fi**eld-**c**oupled **t**echnology-**i**ndependent **o**pen **n**anocomputing
in C++17 using the `EPFL Logic Synthesis Libraries <https://github.com/lsils/lstools-showcase>`_. Thereby, *fiction* focuses on the
logic synthesis, placement, routing, clocking, and verification of emerging nanotechnologies. As a promising class of post-CMOS technologies,
`Field-coupled Nanocomputing (FCN) <https://www.springer.com/de/book/9783662437216>`_ devices like Quantum-dot Cellular
Automata (QCA) in manifold forms (e.g. atomic or molecular), Nanomagnet Logic (NML) devices, Silicon Dangling Bonds (SiDBs), and many more, allow for high computing performance with tremendously low power consumption without the flow of electric current.

With ongoing research in the field, it is unclear, which technology will eventually be competing with CMOS.
To be as generic as possible, *fiction* is able to perform physical design tasks for FCN circuit layouts on a data
structure that abstracts from a particular technology or cell design. Using an extensible set of gate libraries,
technologies, and cell types, these can easily be compiled down to any desired FCN technology.

Compilation requirements
------------------------

The *fiction* framework provides stand-alone CLI tool as well as a header-only library that can be used in external projects.
Both are written in C++17 and are continuously tested on ubuntu, macOS, and Windows with multiple compilers. See the build badges in the README file for more information.

The repository should always be cloned recursively with all submodules::

  git clone --recursive

Several third-party libraries will be cloned within the ``libs`` folder. The ``cmake`` build process will take care of
them automatically. Should the repository have been cloned before,::

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

Using *fiction* as a header-only library
----------------------------------------

All data types and algorithms provided by *fiction* can be used independently by simply including its ``include/`` directory into a source build tree and using

.. code-block:: c++

  #include <fiction/...>

for each used file. Everything that can safely be used is located inside the ``fiction`` namespace.

Building tests
--------------

Unit tests can be built with CMake via a respective flag on the command line and executed via ``ctest``.::

  mkdir build
  cd build
  cmake -DFICTION_TEST=ON ..
  make
  ctest
