# Coordinate Systems

**Header:** `fiction/layouts/coordinates.hpp`

Coordinate types identify positions in a coordinate system, e.g., a Cartesian or hexagonal grid. This file provides implementations for various types of coordinates.

## Offset coordinates

An offset coordinate is a coordinate that defines a location via an offset from a fixed point (origin). Cartesian coordinates are offset coordinates.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```{doxygenstruct} fiction::layouts::coords::offset

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.offset_coordinate
```

:::

::::

## Cube coordinates

Cube coordinates are used as a way to identify faces in a hexagonal grid. A wonderful resource on the topic is: <https://www.redblobgames.com/grids/hexagons/#coordinates-cube>
At the same time, they can be used to address 3-dimensional grids.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```{doxygenstruct} fiction::layouts::coords::cube

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.cube_coordinate
```

:::

::::

## Coordinate iterator

An iterator type that allows to enumerate coordinates in order within a boundary.

```{doxygenclass} fiction::layouts::coords::coordinate_iterator

```

## Utility functions

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

```{doxygenfunction} fiction::layouts::coords::area_of(const CoordinateType& coord) noexcept

```

```{doxygenfunction} fiction::layouts::coords::volume_of(const CoordinateType& coord) noexcept

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.offset_area

.. autofunction:: mnt.pyfiction.cube_area

.. autofunction:: mnt.pyfiction.offset_volume

.. autofunction:: mnt.pyfiction.cube_volume
```

:::

::::
