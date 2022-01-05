Coordinate Systems
==================

**Header:** ``fiction/layouts/coordinates.hpp``

Coordinate types identify positions in a coordinate system, e.g., a Cartesian or hexagonal grid. This file provides implementations for various types of coordinates.

Offset coordinates
------------------

An offset coordinate is a coordinate that defines a location via an offset from a fixed point (origin). Cartesian coordinates are offset coordinates.

.. doxygenstruct:: fiction::offset::ucoord_t

.. doxygenclass:: fiction::offset::coord_iterator

Cube coordinates
----------------

Cube coordinates are used as a way to identify faces in a hexagonal grid. A wonderful resource on the topic is: https://www.redblobgames.com/grids/hexagons/#coordinates-cube
At the same time, they can be used to address 3-dimensional grids.

.. doxygenstruct:: fiction::cube::coord_t

Utility functions
-----------------

.. doxygenfunction:: template<typename CoordinateType> uint64_t fiction::area(const CoordinateType &coord) noexcept
.. doxygenfunction:: template <typename CoordinateType> uint64_t fiction::volume(const CoordinateType& coord) noexcept
