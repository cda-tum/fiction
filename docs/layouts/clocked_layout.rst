.. _clocked layout:

Clocked Layout
==============

Clocking is one of the most important differences between conventional CMOS technologies and the FCN domain. Clocking
directs information flow and provides a notion of timing for both combinational and sequential circuits alike. Usually,
clocking information is assigned to the coordinates of a layout by the means of a clocking scheme.

The clocked layout can be layered on top of any coordinate (in this context called a clock zone) layout type at compile
time to extend its functionality by a notion of FCN clocking. Various :ref:`pre-defined clocking schemes<clocking schemes>`
can be utilized or the clock numbers can be assigned manually.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/clocked_layout.hpp``

        .. doxygenclass:: fiction::clocked_layout
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.clocked_cartesian_layout
            :members:
        .. autoclass:: mnt.pyfiction.clocked_hexagonal_layout
            :members:
