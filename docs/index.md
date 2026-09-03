# Let there be a _fiction_

_fiction_ provides a C++20 header-only library, the `mnt.pyfiction` Python package,
and a command-line interface for Field-coupled Nanocomputing design automation.
Design and inspect QCA, SiDB, and iNML layouts with algorithms for logic synthesis,
placement, routing, clocking, and verification.

## Get started

- {ref}`Use the C++ library <header-only>` to develop algorithms and integrate _fiction_ into a project.
- {ref}`Install the Python bindings <python-bindings>` for scripting and interactive experiments.
- {ref}`Run the CLI with Docker <cli-docker>` to try the tools without a local build.

## Explore the library

Start with {doc}`layouts <layouts/layouts>` and {doc}`networks <networks/networks>`,
then explore {doc}`synthesis <synthesis/synthesis>`,
{doc}`physical design <physical_design/physical_design>`, and
{doc}`verification <verification/verification>`.
The {doc}`technology reference <technology/technology>` covers FCN, QCA, iNML, and SiDB.
API pages provide C++ and Python tabs where both interfaces exist.

```{toctree}
:hidden:
:maxdepth: 1
:caption: User Guide

about
getting_started
cli
publications
changelog
acknowledgments
```

```{toctree}
:hidden:
:maxdepth: 1
:caption: API Reference

layouts/layouts
networks/networks
synthesis/synthesis
physical_design/physical_design
verification/verification
technology/technology
utils/utils
```

```{toctree}
:hidden:
:maxdepth: 1
:caption: Development

contributing
support
```
