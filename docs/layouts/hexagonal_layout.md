# Hexagonal Layout

Coordinate system that represents a hexagonal grid of compile-time coordinate types. The faces of a hexagonal layout
are arranged by shifting either odd or even rows (pointy-top hexagons) or columns (flat-top hexagons) inwards. The
arrangement is a value that the layout receives at construction.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/arrangement.hpp`

```{doxygenenum} fiction::layouts::arrangement
```

```{doxygenfunction} fiction::layouts::is_row_arrangement
```

```{doxygenfunction} fiction::layouts::is_odd_arrangement
```

```{doxygenfunction} fiction::layouts::to_string(const arrangement a)
```

**Header:** `fiction/layouts/hexagonal_layout.hpp`

```{doxygenclass} fiction::layouts::hexagonal_layout
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.layouts.arrangement
   :members:
   :undoc-members:

.. autoclass:: mnt.pyfiction.layouts.hexagonal_layout
   :members:
```

:::

::::
