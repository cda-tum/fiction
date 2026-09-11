# SiDB

```{toctree}
:maxdepth: 1

generators/generators
io/io
layout
model/model
simulation/simulation
```

## Silicon Dangling Bonds (SiDB)

**Header:** `fiction/technology/sidb/technology.hpp`

`sidb_technology` is the technology tag of the Cartesian cell-level layout interface, which physical
design produces before [conversion to an `sidb::layout`](layout.md#conversion-from-cell-level-layouts).
It has no Python counterpart; Python works on [`sidb_layout`](layout.md#layout) and its
`sidb_dot_tag` values directly.

```{doxygenstruct} fiction::sidb::sidb_technology
:members:
```

(bestagon)=

## SiDB Bestagon Library

**Header:** `fiction/technology/sidb/bestagon_library.hpp`

```{doxygenclass} fiction::sidb::bestagon_library
:members:
```

## Parameterized SiDB Library

**Header:** `fiction/technology/sidb/on_the_fly_gate_library.hpp`

```{doxygenstruct} fiction::sidb::on_the_fly_gate_library_params
:members:
```

```{doxygenclass} fiction::sidb::on_the_fly_gate_library
:members:
```

```{doxygenclass} fiction::sidb::gate_design_exception
:members:
```

## Skeleton Bestagon Library

**Header:** `fiction/technology/sidb/skeleton_bestagon_library.hpp`

```{doxygenclass} fiction::sidb::skeleton_bestagon_library

```
