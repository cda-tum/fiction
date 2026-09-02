SiDB Surfaces
=============

SiDB Defect Surface
-------------------

**Header:** ``fiction/technology/sidb/surfaces/defect_surface.hpp``

A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
fabrication defects on the H-Si(100) 2x1 surface.

.. doxygenclass:: fiction::sidb::surfaces::defect_surface
   :members:
.. doxygenclass:: fiction::sidb::surfaces::defect_surface< Lyt, true >
   :members:
.. doxygenclass:: fiction::sidb::surfaces::defect_surface< Lyt, false >
   :members:

SiDB Charge Distribution Surface
--------------------------------

The charge distribution surface can be layered on top of any SiDB layout to add representation of possible charge
distributions of the SiDBs. Charge distribution surfaces are returned by the SiDB physical simulation algorithms.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/surfaces/charge_distribution_surface.hpp``

        .. doxygenenum:: fiction::sidb::surfaces::dependent_cell_mode
        .. doxygenenum:: fiction::sidb::surfaces::energy_calculation
        .. doxygenenum:: fiction::sidb::surfaces::charge_distribution_history
        .. doxygenenum:: fiction::sidb::surfaces::charge_index_recomputation
        .. doxygenenum:: fiction::sidb::surfaces::charge_distribution_mode
        .. doxygenenum:: fiction::sidb::surfaces::charge_index_mode
        .. doxygenenum:: fiction::sidb::surfaces::charge_transition_threshold_bounds
        .. doxygenclass:: fiction::sidb::surfaces::charge_distribution_surface
           :members:
        .. doxygenclass:: fiction::sidb::surfaces::charge_distribution_surface< Lyt, true >
           :members:
        .. doxygenclass:: fiction::sidb::surfaces::charge_distribution_surface< Lyt, false >
           :members:

    .. tab:: Python

        .. autoclass:: mnt.pyfiction.dependent_cell_mode
            :members:
        .. autoclass:: mnt.pyfiction.energy_calculation
            :members:
        .. autoclass:: mnt.pyfiction.charge_index_mode
            :members:
        .. autoclass:: mnt.pyfiction.charge_distribution_history
            :members:
        .. autoclass:: mnt.pyfiction.charge_distribution_surface_100
            :members:
        .. autoclass:: mnt.pyfiction.charge_distribution_surface_111
            :members:

.. toctree::
   :maxdepth: 1

   hsi_lattices
