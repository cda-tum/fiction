Gate-level Layout
=================

The gate-level layout can be layered on top of any clocked layout type at compile time to extend its functionality by
a notion of gates that can be assigned to its clock zones. Each gate is identified as a Boolean function. Thereby, this
layout type is technology-independent and does not have any concrete implementation, structures, or shapes assigned to
its gates. Even the sizes of the clock zones (tiles) are not determined. Consequently, this layout type is best suited
for logic-level placement and routing and can be considered *fiction*\ 's most important core data type.

The gate-level layout is implemented in accordance with ``mockturtle``\ 's
`network interface API <https://mockturtle.readthedocs.io/en/latest/network.html>`_. Thus, it is itself a mockturtle
network that can be passed to various of mockturtle's algorithms. However, since each logic node (gate) in the layout
has to have a concrete position assigned, mockturtle cannot be used to generate valid layouts.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/gate_level_layout.hpp``

        .. doxygenclass:: fiction::gate_level_layout
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.cartesian_gate_layout
            :members:
        .. autoclass:: mnt.pyfiction.hexagonal_gate_layout
            :members:
