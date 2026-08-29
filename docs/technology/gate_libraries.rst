.. _fcn-gate-libraries:

FCN Gate Libraries
==================

A gate library is a set of cell-accurate implementations of gate functions. It breaks down each gate into a tile of
cells. Thereby, via ``apply_gate_library``, a ``gate_level_layout`` can be compiled down to a ``cell_level_layout`` of a
concrete technology.

The implementation of a gate library should be non-instantiatable plus ``static`` and ``constexpr`` wherever possible.
The abstract class ``fiction::fcn::gate_library`` provides utility functions that can be used to implement custom
gate libraries by extending it.

Abstract Gate Library
---------------------

**Header:** ``fiction/technology/fcn/gate_library.hpp``

.. doxygenclass:: fiction::fcn::gate_library
   :members:

.. doxygenclass:: fiction::fcn::unsupported_gate_type_exception
    :members:
.. doxygenclass:: fiction::fcn::unsupported_gate_orientation_exception
    :members:

**Header:** ``fiction/technology/fcn/cell_ports.hpp``

.. doxygenstruct:: fiction::fcn::port_position
   :members:
.. doxygenstruct:: fiction::fcn::port_direction
   :members:
.. doxygenstruct:: fiction::fcn::port_list
   :members:

QCA ONE Library
---------------

**Header:** ``fiction/technology/qca/qca_one_library.hpp``

.. doxygenclass:: fiction::qca::qca_one_library
   :members:


.. _molQCA:

Molecular QCA Library
---------------------

**Header:** ``fiction/technology/qca/sim7_mol_library.hpp``

.. doxygenclass:: fiction::qca::sim7_mol_library
   :members:


iNML ToPoliNano Library
-----------------------

**Header:** ``fiction/technology/inml/topolinano_library.hpp``

.. doxygenclass:: fiction::inml::topolinano_library
   :members:


.. _bestagon:

SiDB Bestagon Library
---------------------

**Header:** ``fiction/technology/sidb/bestagon_library.hpp``

.. doxygenclass:: fiction::sidb::bestagon_library
   :members:

Parameterized SiDB Library
--------------------------

**Header:** ``fiction/technology/sidb/on_the_fly_gate_library.hpp``

.. doxygenstruct:: fiction::sidb::on_the_fly_gate_library_params
   :members:
.. doxygenclass:: fiction::sidb::on_the_fly_gate_library
   :members:

.. doxygenclass:: fiction::sidb::gate_design_exception
   :members:
