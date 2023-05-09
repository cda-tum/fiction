Coordinate Systems
==================

**Header:** ``fiction/layouts/coordinates.hpp``

Coordinate types identify positions in a coordinate system, e.g., a Cartesian or hexagonal grid. This file provides implementations for various types of coordinates.

Offset coordinates
------------------

An offset coordinate is a coordinate that defines a location via an offset from a fixed point (origin). Cartesian coordinates are offset coordinates.

.. tabs::
    .. tab:: C++
        .. doxygenstruct:: fiction::offset::ucoord_t
        .. doxygenclass:: fiction::offset::coord_iterator

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.coordinate
            :members:

Cube coordinates
----------------

Cube coordinates are used as a way to identify faces in a hexagonal grid. A wonderful resource on the topic is: https://www.redblobgames.com/grids/hexagons/#coordinates-cube
At the same time, they can be used to address 3-dimensional grids.

.. tabs::
    .. tab:: C++
        .. doxygenstruct:: fiction::cube::coord_t

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.cube_coordinate
            :members:

SiQAD coordinates
-----------------

SiQAD coordinates are used to describe locations of Silicon Dangling Bonds on the H-Si(100) 2x1 surface were dimer columns and rows are identified by x and y values, respecitvely,
while the z value (0,1) points to the top or bottom Si atom in the dimer. The coordinates are originally used in the SiQAD simulator (https://github.com/siqad).

.. tabs::
    .. tab:: C++
        .. doxygenstruct:: fiction::siqad::coord_t

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.siqad_coordinate
            :members:

Utility functions
-----------------

.. tabs::
    .. tab:: C++
        .. doxygenfunction:: fiction::area(const CoordinateType& coord) noexcept
        .. doxygenfunction:: fiction::volume(const CoordinateType& coord) noexcept
        .. doxygenfunction:: fiction::siqad::to_siqad_coord(const CoordinateType& coord) noexcept
        .. doxygenfunction:: fiction::siqad::to_fiction_coord(const coord_t& coord) noexcept
    .. tab:: Python
        .. autofunction:: fiction.pyfiction.to_fiction_coord
        .. autofunction:: fiction.pyfiction.to_siqad_coord
