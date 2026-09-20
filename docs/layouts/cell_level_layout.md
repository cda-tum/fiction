# Cell-level Layout

`cell_level_layout<Technology, CoordinateLayout>` stores FCN cells on a coordinate layout.
The {ref}`cell technology <fcn-cell-technologies>` supplies cell types and modes.
The layout owns clocking, synchronization, and persistent obstruction data. Dedicated
`sidb::layout` represents SiDB designs independently of this hierarchy.

Cell-level layouts merely represent structural information of the FCN circuit but do not inherently possess any
knowledge of their implemented functionality.

A cell-level layout can be obtained from a `gate_level_layout` by the {ref}`application <apply-gate-library>` of a
{ref}`gate library <fcn-gate-libraries>`.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/cell_level_layout.hpp`

```{doxygenclass} fiction::layouts::cell_level_layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.qca_layout
   :members:

.. autoclass:: mnt.pyfiction.inml_layout
   :members:

.. autoclass:: mnt.pyfiction.mol_qca_layout
   :members:
```

:::

::::

## Clock and synchronization coordinates

`get_clock_number(c)` takes a cell position and queries the clock zone at
`(c.x / tile_size_x, c.y / tile_size_y, c.z)`. `assign_clock_number(cz, number)` takes
the unscaled clock-zone coordinate. For a tile size of 2 by 2, assigning zone `(1, 1)`
changes the clock returned for cells `(2, 2)` through `(3, 3)`.
Clock-zone dimensions must be positive. The constructor and tile-size setters reject zero;
a rejected setter call preserves the stored dimensions. Python exposes these dimensions
through `get_tile_size_x`, `get_tile_size_y`, `set_tile_size_x`, and `set_tile_size_y`.

Synchronization assignment and lookup use the supplied coordinate directly, without
tile-size conversion. `num_se()` counts stored nonzero synchronization entries.
Manual obstructions use cell positions. An occupied cell remains obstructed after clearing
manual obstructions. Ordinary C++ copies share storage; `clone()` produces independent state.
