What is this about?
===================

This code base provides a framework for **fi**\ eld-**c**\ oupled **t**\ echnology-**i**\ ndependent **o**\ pen **n**\ anocomputing
in C++17 using the `EPFL Logic Synthesis Libraries <https://github.com/lsils/lstools-showcase>`_. Thereby, *fiction* focuses on the
logic synthesis, placement, routing, clocking, and verification of emerging nanotechnologies. As a promising class of post-CMOS technologies,
`Field-coupled Nanocomputing (FCN) <https://www.springer.com/de/book/9783662437216>`_ devices like Quantum-dot Cellular
Automata (QCA) in manifold forms (e.g. atomic or molecular), Nanomagnet Logic (NML) devices, Silicon Dangling Bonds (SiDBs),
and many more, allow for high computing performance with tremendously low power consumption without the flow of electric current.

With ongoing research in the field, it is unclear, which technology will eventually be competing with CMOS.
To be as generic as possible, *fiction* is able to perform physical design tasks for FCN circuit layouts on data
structures that abstract from particular technology or cell design. Using an extensible set of gate libraries,
technologies, and cell types, these can easily be compiled down to any desired FCN technology for physical simulation.

The *fiction* framework is academic software and aims at researchers and developers in the FCN domain who want to obtain
cell-accurate circuit layouts from logical specifications or who want to implement their own physical design algorithms.

For these use cases, *fiction* provides a :ref:`header-only library <header-only>` that provides data types and algorithms
for recurring tasks, e.g., logic network and layout types on different abstraction levels, clocking schemes, gate libraries,
placement, routing, clocking, and verification algorithms, etc. Additionally, *fiction* comes with an ABC-like
:ref:`CLI tool <cli>` that allows quick access to its core functionality.

If you have any questions, comments, or suggestions, please do not hesitate to get in touch.
