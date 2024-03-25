Obstruction Layout
==================

The obstruction layout enables a unified interface for checking coordinates and connections for their obstruction that
can be layered on top of any coordinate layout. An obstruction could be due to a prior placement or due to a fabrication
defect. This interface can be added before construction of a new layout or to an already existing one without altering
it.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/obstruction_layout.hpp``

        .. doxygenclass:: fiction::obstruction_layout
           :members:
        .. doxygenclass:: fiction::obstruction_layout< Lyt, true >
           :members:
        .. doxygenclass:: fiction::obstruction_layout< Lyt, false >
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.cartesian_obstruction_layout
            :members:
        .. autoclass:: mnt.pyfiction.shifted_cartesian_obstruction_layout
            :members:
        .. autoclass:: mnt.pyfiction.hexagonal_obstruction_layout
            :members:
