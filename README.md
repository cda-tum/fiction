# Let there be a *fiction*

[![Ubuntu CI](https://github.com/marcelwa/fiction/actions/workflows/ubuntu.yml/badge.svg?branch=main)](https://github.com/marcelwa/fiction/actions/workflows/ubuntu.yml)
[![macOS CI](https://github.com/marcelwa/fiction/actions/workflows/macos.yml/badge.svg?branch=main)](https://github.com/marcelwa/fiction/actions/workflows/macos.yml)
[![Windows CI](https://github.com/marcelwa/fiction/actions/workflows/windows.yml/badge.svg?branch=main)](https://github.com/marcelwa/fiction/actions/workflows/windows.yml)
[![codecov](https://codecov.io/gh/marcelwa/fiction/branch/main/graph/badge.svg?token=SUPC5N6KFU)](https://codecov.io/gh/marcelwa/fiction)
[![Documentation Status](https://img.shields.io/readthedocs/fiction?logo=readthedocs)](https://fiction.readthedocs.io/)
[![Release](https://img.shields.io/github/v/tag/marcelwa/fiction?label=fiction&logo=github&logoColor=darkgray)](https://github.com/marcelwa/fiction/releases)
[![License](https://img.shields.io/github/license/marcelwa/fiction)](https://github.com/marcelwa/fiction/blob/main/LICENSE.txt)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/marcelwa/fiction.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/marcelwa/fiction/context:cpp)

This code base provides a framework for **fi**eld-**c**oupled **t**echnology-**i**ndependent **o**pen **n**anocomputing
in C++17 using the [EPFL Logic Synthesis Libraries](https://github.com/lsils/lstools-showcase). Thereby, *fiction*
focuses on the logic synthesis, placement, routing, clocking, and verification of emerging nanotechnologies. As a
promising class of post-CMOS technologies,
[Field-coupled Nanocomputing (FCN)](https://www.springer.com/de/book/9783662437216) devices like Quantum-dot Cellular
Automata (QCA) in manifold forms (e.g. atomic or molecular), Nanomagnet Logic (NML) devices, Silicon Dangling Bonds
(SiDBs), and many more, allow for high computing performance with tremendously low power consumption without the flow of
electric current.

With ongoing research in the field, it is unclear, which technology will eventually be competing with CMOS. To be as
generic as possible, *fiction* is able to perform physical design tasks for FCN circuit layouts on data structures that
abstract from particular technology or cell design. Using an extensible set of gate libraries, technologies, and cell
types, these can easily be compiled down to any desired FCN technology for physical simulation.

The *fiction* framework is academic software and aims at researchers and developers in the FCN domain who want to obtain
cell-accurate circuit layouts from logical specifications or who want to implement their own physical design algorithms.

For these use cases, *fiction* provides
a [header-only library](https://fiction.readthedocs.io/en/latest/getting_started.html#using-fiction-as-a-header-only-library)
that provides data types and algorithms for recurring tasks, e.g., logic network and layout types on different
abstraction levels, clocking schemes, gate libraries, placement, routing, clocking, and verification algorithms, etc.
Additionally, *fiction* comes with an
ABC-like [CLI tool](https://fiction.readthedocs.io/en/latest/getting_started.html#using-fiction-as-a-stand-alone-cli-tool)
that allows quick access to its core functionality.

Learn more by referring to the [full documentation](https://fiction.readthedocs.io/).

If you have any questions, comments, or suggestions, please do not hesitate to get in touch.

## Implemented Physical Design Algorithms

For automatic FCN layout obtainment, *fiction* provides implementations of state-of-the-art
[physical design algorithms](https://fiction.readthedocs.io/en/latest/algorithms/algorithms.html#physical-design).

Among these are

- SMT-based [exact placement and routing](https://ieeexplore.ieee.org/document/8342060)
- OGD-based [scalable placement and routing](https://dl.acm.org/citation.cfm?id=3287705)
- SAT-based [one-pass synthesis](https://ieeexplore.ieee.org/document/9371573)

Furthermore, layout correctness can
be [validated](https://fiction.readthedocs.io/en/latest/algorithms/algorithms.html#verification) using

- SAT-based [formal verification](https://ieeexplore.ieee.org/document/9218641)

For logic synthesis, *fiction* relies on the [mockturtle library](https://github.com/lsils/mockturtle) that offers logic
network types and optimization algorithms. Optimized logic networks can then be passed as specifications to physical
design algorithms. Alternatively, logic synthesis can be performed in external tools and resulting
Verilog/AIGER/BLIF/... files can be parsed by *fiction*.

## Supported Technologies

Physical design in *fiction* can be performed technology-independent. Only if resulted layouts are to be physically,
simulated, a specific technology implementation is required. To this end, *fiction* supports various potential FCN
implementations together with gate libraries to compile gate-level layout abstractions down to the cell level.
Additionally, output formats for state-of-the-art physical simulator engines are supported.

### Quantum-dot Cellular Automata (QCA)

Gate libraries:

- [QCA ONE](https://ieeexplore.ieee.org/document/7538997/)

File formats:

- `*.qca` for [QCADesigner](https://waluslab.ece.ubc.ca/qcadesigner/)
- `*.qll` for [MagCAD](https://topolinano.polito.it/) and [SCERPA](https://ieeexplore.ieee.org/document/8935211)
- `*.fqca` for [QCA-STACK](https://github.com/wlambooy/QCA-STACK)
- `*.svg` for visual representation

Many thanks to Frank Sill Torres for his support with the QCADesigner format, to Willem Lambooy for his support with the
QCA-STACK format, and to Sophia Kuhn for implementing the SVG writer!

### in-plane Nanomagnet Logic (iNML)

Gate libraries:

- [ToPoliNano](https://topolinano.polito.it/supported-technologies/)

File formats:

- `*.qcc` for [ToPoliNano](https://topolinano.polito.it/)
- `*.qll` for [ToPoliNano & MagCAD](https://topolinano.polito.it/)

Many thanks to Umberto Garlando, Fabrizio Riente, and Giuliana Beretta for their support!

### Silicon Dangling Bonds (SiDBs)

Gate libraries:

- [Bestagon](https://dl.acm.org/doi/10.1145/3489517.3530525)

File formats:

- `*.sqd` for [SiQAD](https://github.com/siqad/siqad)

Many thanks to Samuel Sze Hang Ng for his support!

## Clocking Schemes

There are highly regular clocking schemes proposed for FCN technologies which can be used as a floor plan for physical
design. However, sometimes it can make sense to have more degree of freedom and assign clock numbers on the fly. That is
why *fiction*
supports [regular and irregular clocking schemes](https://fiction.readthedocs.io/en/latest/layouts/clocking_scheme.html)
with variable amounts of clock numbers as QCA for instance uses four clock phases but iNML needs only three.

Built-in schemes are

|      [Columnar](https://ieeexplore.ieee.org/document/573740)       |    [Row](https://ieeexplore.ieee.org/document/573740)    |     [2DDWave](https://ieeexplore.ieee.org/document/1717097)      |
|:------------------------------------------------------------------:|:--------------------------------------------------------:|:----------------------------------------------------------------:|
| <img src="docs/_static/columnar.png" alt="Columnar" height="200"/> | <img src="docs/_static/row.png" alt="Row" height="200"/> | <img src="docs/_static/2ddwave.png" alt="2DDWave" height="200"/> |

|   [USE](https://ieeexplore.ieee.org/document/7219390)    | [RES](https://www.tandfonline.com/doi/abs/10.1080/21681724.2019.1570551) | [ESR](https://link.springer.com/content/pdf/10.1007/s10470-020-01760-4.pdf) |
|:--------------------------------------------------------:|:------------------------------------------------------------------------:|:---------------------------------------------------------------------------:|
| <img src="docs/_static/use.png" alt="USE" height="200"/> |         <img src="docs/_static/res.png" alt="RES" height="200"/>         |          <img src="docs/_static/esr.png" alt="ESR" height="200"/>           |

|   [CFE](https://ietresearch.onlinelibrary.wiley.com/doi/10.1049/iet-cds.2019.0096)   |    [BANCS](https://ieeexplore.ieee.org/document/8533251)     |
|:------------------------------------------------------------------------------------:|:------------------------------------------------------------:|
|               <img src="docs/_static/cfe.png" alt="CFE" height="200"/>               | <img src="docs/_static/bancs.png" alt="BANCS" height="300"/> |

plus the mentioned irregular open clocking that works via a clock map instead of a regular extrapolated cutout.

## Wire Crossings

<img src="docs/_static/cross.png" alt="Second layer crossing" align="left" width="200"/>

With many FCN technologies considered planar, wire crossings should be minimized if possible. However, there are some
options in QCA where, using a second layer, crossings over short distances and co-planar rotated cells become possible.
As both are just technical implementations of the same concept, *fiction* supports crossings as wires in a second grid
layer in its data structures. They will also be represented as such in corresponding SVG and QCADesigner output.
However, note that it is to be interpreted as the concept of crossings and could also be realized co-planar.

Wires are only allowed to cross other wires! Wires crossing gates is considered to lead to unstable signals.

## Gate Pins vs. Designated I/Os

<img src="docs/_static/io.png" alt="Input pin and cell output" align="right" width="200"/>

In the literature, both is seen: having input cells (pins) directly located in the gate structure or using designated
I/O elements which are located outside of gates. This distinction only makes sense on the gate-level and *fiction*
supports both approaches and offers usage in the implemented physical design algorithms.

## Multi Wires

<img src="docs/_static/multi.png" alt="Multi wires" align="left" width="200"/>

Gate-level abstraction has its limits. Often, chip area is wasted when only using a single wire per tile. In *fiction*,
cell-level layouts allow for precise control over cell placement and can, thus, also create multiple wire segments per
clock zone. Physical simulation can give an indication whether the built structures are implementing the intended
functionality.

## Synchronization Elements

<img src="docs/_static/se.png" alt="Synchronization element" align="right" width="150"/>

A technology extension proposes to utilize the external clock signal generator in an unconventional way: by creating
further asymmetric clock signals with extended *Hold* phases that are assigned to specific wire
tiles, [synchronization elements](https://ieeexplore.ieee.org/document/8626294) can be created that stall signals over
multiple clock cycles. These artificial latches are able to feed information to any other clock number, but their usage
reduces the overall throughput of the layout. In return, long wire detours for signal synchronization can be prevented.

## Cost Metrics

Designed layouts can be evaluated with regard to several cost functions. The following metrics are currently
implemented:

Gate-level layouts:

- Circuit dimension in tiles
- Number of gate tiles
- Number of wire tiles
- Number of wire crossings
- Number of [synchronization elements](#synchronization-elements)
- Critical path
- Throughput
- Bounding box
- Energy dissipation based on a [physical model](https://ieeexplore.ieee.org/document/8246526) (QCA only)

Cell-level layouts:

- Circuit dimension in cells
- Number of cells
- Bounding box
- Area usage in nm²
