# SiDB

```{toctree}
:maxdepth: 1

generators/generators
io/io
layout
model/model
simulation/simulation
surfaces/surfaces
```

## Silicon Dangling Bonds (SiDB)

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/technology.hpp`

```{doxygenstruct} fiction::sidb::sidb_technology
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_technology
   :members:
```

:::

::::

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
