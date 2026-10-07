# QCA Layout

A QCA layout places QCA cells on a Cartesian grid with a ground layer and a crossing layer. Each cell has a type, a
mode as known from QCADesigner (regular, rotated, via, or crossover), and, for inputs and outputs, a name. Clock zones
are tiles of cells, and each clock zone can be a synchronization element that extends its Hold phase. See
{doc}`../../layouts/cell_grid` for the cell storage and the clock zones that QCA shares with other technologies.

A QCA layout can be obtained from a gate-level layout by applying the {ref}`QCA ONE library <apply-gate-library>`.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/qca/layout.hpp`

```{doxygenenum} fiction::qca::cell_type
```

```{doxygenenum} fiction::qca::cell_mode
```

```{doxygenfunction} fiction::qca::is_constant
```

```{doxygenclass} fiction::qca::layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.qca.qca_cell_type
   :members:

.. autoclass:: mnt.pyfiction.qca.qca_cell_mode
   :members:

.. autoclass:: mnt.pyfiction.qca.qca_layout
   :members:
```

:::

::::
