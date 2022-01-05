Hexagonal Layout
================

**Header:** ``fiction/layouts/hexagonal_layout.hpp``

Coordinate system that represents a hexagonal grid. In contrast to Cartesian layouts, the faces of hexagonal layouts can
be arranged in different ways. The most obvious are the distinction between the orientation of the hexagons that can
either be pointy-top up or flat-top up.

.. doxygenstruct:: fiction::pointy_top
.. doxygenstruct:: fiction::flat_top

Furthermore, when assuming rectangular-ish layouts, either odd or even rows or columns can be shifted inwards, yielding
a total of four different orientations.

.. doxygenstruct:: fiction::odd_row
.. doxygenstruct:: fiction::even_row
.. doxygenstruct:: fiction::odd_column
.. doxygenstruct:: fiction::even_column

The hexagonal layout supports all of them via template parameters.

.. doxygenclass:: fiction::hexagonal_layout
   :members:
