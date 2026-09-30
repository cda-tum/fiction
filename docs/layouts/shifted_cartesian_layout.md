# Shifted Cartesian Layout

Coordinate system that represents a shifted Cartesian grid of compile-time coordinate types. The faces of a shifted
Cartesian layout are arranged by shifting either odd or even rows (horizontal shift) or columns (vertical shift)
inwards. The arrangement is a value that the layout receives at construction. The layout shares its members with
`hexagonal_layout`, and {doc}`hexagonal_layout` documents the `arrangement` type.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/shifted_cartesian_layout.hpp`

```{doxygenclass} fiction::layouts::shifted_cartesian_layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.layouts.shifted_cartesian_layout
   :members:
```

:::

::::
