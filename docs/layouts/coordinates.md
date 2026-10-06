# Coordinates

**Header:** `fiction/layouts/layout_base.hpp`

Every layout (Cartesian, shifted Cartesian, and hexagonal) derives from `layout_base`, which defines the one coordinate
type that all of them share. A coordinate is an offset from a fixed point (origin) with three signed 32-bit axes. The
default-constructed coordinate is invalid, and so is every coordinate with an axis equal to `-2147483648`. Layouts
return an invalid coordinate for neighbors that lie outside of them, and gate-level layouts return it for the tile of a
node that is not placed.
Each layout exposes the type as `coordinate`, gate-level layouts in C++ also as `tile`, and its aspect ratio as
`aspect_ratio`. An aspect ratio is the highest coordinate that still belongs to a layout, not a size.

Gate-level layouts identify tiles by a 64-bit signal. It holds x and y as 31-bit signed values and z as one bit, so the
x and y extents of gate-level layouts are limited to $2^{30} - 1$ and z to 1. Every other layout limits each extent to
$2^{30} - 1$, so that coordinate arithmetic stays within 32 bits.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```{doxygenclass} fiction::layouts::layout_base

```

```{doxygenstruct} fiction::layouts::layout_base::coordinate

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.layouts.coordinate
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

```{doxygenfunction} fiction::layouts::area_of(const CoordinateType& coord) noexcept

```

```{doxygenfunction} fiction::layouts::volume_of(const CoordinateType& coord) noexcept

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.area

.. autofunction:: mnt.pyfiction.layouts.volume
```

:::

::::
