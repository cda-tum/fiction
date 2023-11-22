Hexagonal Layout
================

Coordinate system that represents a hexagonal grid of compile-time coordinate types. In contrast to Cartesian layouts,
the faces of hexagonal layouts can be arranged in different ways. The most obvious are the distinction between the
orientation of the hexagons that can either be pointy-top up or flat-top up.

.. tabs::
    .. tab:: C+
        **Header:** ``fiction/layouts/hexagonal_layout.hpp``

        .. doxygenstruct:: fiction::pointy_top_hex
        .. doxygenstruct:: fiction::flat_top_hex

        Furthermore, when assuming rectangular-ish layouts, either odd or even rows or columns can be shifted inwards,
        yielding a total of four different orientations.

        .. doxygenstruct:: fiction::odd_row_hex
        .. doxygenstruct:: fiction::even_row_hex
        .. doxygenstruct:: fiction::odd_column_hex
        .. doxygenstruct:: fiction::even_column_hex

        The hexagonal layout supports all of them via template parameters.

        .. doxygenclass:: fiction::hexagonal_layout
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.hexagonal_layout
            :members:
