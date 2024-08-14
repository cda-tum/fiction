.. _fcn-gate-libraries:

FCN Gate Libraries
==================

A gate library is a set of cell-accurate implementations of gate functions. It breaks down each gate into a tile of
cells. Thereby, via ``apply_gate_library``, a ``gate_level_layout`` can be compiled down to a ``cell_level_layout`` of a
concrete technology.

The implementation of a gate library should be non-instantiatable plus ``static`` and ``constexpr`` wherever possible.
The abstract class ``fcn_gate_library`` provides utility functions that can be used to implement custom gate libraries
by extending it.

Abstract Gate Library
---------------------

**Header:** ``fiction/technology/fcn_gate_library.hpp``

.. doxygenclass:: fiction::fcn_gate_library
   :members:

.. doxygenclass:: fiction::unsupported_gate_type_exception
    :members:
.. doxygenclass:: fiction::unsupported_gate_orientation_exception
    :members:

**Header:** ``fiction/technology/cell_ports.hpp``

.. doxygenstruct:: fiction::port_position
   :members:
.. doxygenstruct:: fiction::port_direction
   :members:
.. doxygenstruct:: fiction::port_list
   :members:

QCA ONE Library
---------------

**Header:** ``fiction/technology/qca_one_library.hpp``

.. doxygenclass:: fiction::qca_one_library
   :members:


iNML ToPoliNano Library
-----------------------

**Header:** ``fiction/technology/inml_topolinano_library.hpp``

.. doxygenclass:: fiction::inml_topolinano_library
   :members:


.. _bestagon:

SiDB Bestagon Library
---------------------

**Header:** ``fiction/technology/sidb_bestagon_library.hpp``

.. doxygenclass:: fiction::sidb_bestagon_library
   :members:

Parameterized SiDB Library
--------------------------

**Header:** ``fiction/technology/parameterized_gate_library.hpp``

.. doxygenstruct:: fiction::parameterized_gate_library_params
   :members:
.. doxygenclass:: fiction::parameterized_gate_library
   :members:

.. doxygenclass:: fiction::gate_design_exception
   :members:
