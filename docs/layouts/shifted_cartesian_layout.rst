Shifted Cartesian Layout
========================

Coordinate system that represents a shifted Cartesian grid of compile-time coordinate types. In contrast to Cartesian layouts,
the faces of shifted Cartesian layouts can be arranged in different ways. The most obvious are the distinction between the
orientation of the shift that can be horizontal and vertical.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/shifted_cartesian_layout.hpp``

        .. doxygenstruct:: fiction::horizontal_shift_cartesian
        .. doxygenstruct:: fiction::vertical_shift_cartesian

        Furthermore, when assuming rectangular-ish layouts, either odd or even rows or columns can be shifted inwards, yielding
        a total of four different orientations.

        .. doxygenstruct:: fiction::odd_row_cartesian
        .. doxygenstruct:: fiction::even_row_cartesian
        .. doxygenstruct:: fiction::odd_column_cartesian
        .. doxygenstruct:: fiction::even_column_cartesian

        The shifted Cartesian layout supports all of them via template parameters. It shares its members with ``hexagonal_layout``.

        .. doxygenclass:: fiction::shifted_cartesian_layout
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.shifted_cartesian_layout
            :members:
