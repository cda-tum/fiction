# Coordinates

**Header:** `fiction/layouts/layout_base.hpp`

Every layout (Cartesian, shifted Cartesian, and hexagonal) derives from `layout_base`, which defines the one coordinate
type that all of them share. A coordinate is an offset from a fixed point (origin) with three signed 32-bit axes. The
default-constructed coordinate is the origin. Every signed 32-bit value is representable, including
`-2147483648`. Neighbor operations return `std::optional<coordinate>` in C++ and a coordinate or
`None` in Python. Absence means that the neighbor lies outside the frame or cannot be represented.

Each layout exposes the coordinate type as `coordinate`, and gate-level layouts also as `tile`.
The frame is an `extent{width, height, layers}` with nonnegative counts. It contains coordinates
whose axes lie in `[0, width)`, `[0, height)`, and `[0, layers)`. Two axes describe one layer;
the default extent is empty. Width and height can reach $2^{31}$, so the final included coordinate
still fits a signed 32-bit axis. Layouts accept at most two layers: the ground layer at `z = 0`
and the crossing layer at `z = 1`. Construction and resize reject valid extents with larger layer counts with
`std::out_of_range` in C++ and `IndexError` in Python. A rejected resize preserves the dimensions.
`width()`, `height()`, and `layers()` return counts;
`dimensions()` returns the extent and `last()` returns the optional final coordinate.

Gate-level layouts store object identity separately from placement coordinates. An editing object
may lie outside the frame; physical design-rule checks validate frame membership.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```{doxygenclass} fiction::layouts::layout_base

```

```{doxygenstruct} fiction::layouts::layout_base::coordinate

```

```{doxygenstruct} fiction::layouts::layout_base::extent

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.layouts.coordinate
   :members:

.. autoclass:: mnt.pyfiction.layouts.Extent
   :members:
```

:::

::::

## Coordinate iterator

An iterator type that allows to enumerate coordinates in order within a boundary.

```{doxygenclass} fiction::layouts::layout_base::coordinate_iterator

```

## Utility functions

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```{doxygenfunction} fiction::layouts::area_of

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.area
```

:::

::::
