# Benchmarks

To objectively evaluate and compare software and design automation tools for the physical design of FCN, diverse benchmark sets with function descriptions are needed.
[MNT Bench](/mntbench/) provides functions from the following benchmark sets in Verilog (``.v``) format:

1. [Trindade16](https://ieeexplore.ieee.org/document/7724048)
2. [Fontes18](https://ieeexplore.ieee.org/document/8351001)
3. [ISCAS85](https://www.researchgate.net/publication/273775783_A_neutral_netlist_of_10_combinational_benchmark_circuits_and_a_targeted_translator_in_FORTRAN)
4. [EPFL](https://www.epfl.ch/labs/lsi/page-102566-en-html/benchmarks/)

For convenience, they are also part of *fiction* to be directly used in experiments or the development of new physical design algorithms, optimizations, and simulators.

# Gate-level Layouts

Gate-level layouts generated with any of the physical design methods implemented in *fiction* can be stored in a human-readable file format (``.fgl``) with the
correponding [write](https://fiction.readthedocs.io/en/latest/io/physical_simulation.html#technology-independent-gate-level-layouts) function.

[MNT Bench](https://www.cda.cit.tum.de/mntbench/) offers gate-level layouts spanning various gate libraries, clocking schemes, physical
design algorithms, and optimizations. These layouts can be [read](https://fiction.readthedocs.io/en/latest/io/input.html#gate-level-layouts) with *fiction* and used for testing new
post-layout optimization algorithms, creating cell-level layouts, or any other use case.
