Physical Simulation of SiDBs
============================

These headers provide auxiliary functionality for the physical simulation of SiDBs layouts.

SiDB Charge States
------------------

**Header:** ``fiction/technology/sidb_charge_state.hpp``

Possible charge states of SiDBs.

.. doxygenenum:: fiction::sidb_charge_state
.. doxygenfunction:: fiction::charge_state_to_sign(const sidb_charge_state& cs) noexcept
.. doxygenfunction:: fiction::sign_to_charge_state(const int8_t sg) noexcept
.. doxygenfunction:: fiction::charge_configuration_to_string(const std::vector<sidb_charge_state>& charge_distribution) noexcept
.. doxygenenum:: fiction::sidb_state_iter_dir
.. doxygenclass:: fiction::sidb_charge_state_iterator
   :members:

SiDB Charge Distribution Surface
--------------------------------

**Header:** ``fiction/technology/charge_distribution_surface.hpp``

The charge distribution surface can be layered on top of any SiDB layout to add representation of possible charge
distributions of the SiDBs. Charge distribution surfaces are returned by the SiDB physical simulation algorithms.

.. doxygenclass:: fiction::charge_distribution_surface
   :members:
.. doxygenclass:: fiction::charge_distribution_surface< Lyt, true >
   :members:
.. doxygenclass:: fiction::charge_distribution_surface< Lyt, false >
   :members:


Physical Constants
------------------

**Header:** ``fiction/technology/physical_constants.hpp``

.. doxygenvariable:: EPSILON
.. doxygenvariable:: ELEMENTARY_CHARGE
.. doxygenvariable:: K_E
.. doxygenvariable:: POP_STABILITY_ERR
.. doxygenvariable:: PI


Euclidean distance between two SiDBs in nanometers
--------------------------------------------------

**Header:** ``fiction/algorithms/simulation/sidb/sidb_nanometer_distance.hpp``

.. doxygenfunction:: fiction::sidb_nanometer_distance


SiDB Cluster Hierarchy
----------------------

**Header:** ``fiction/technology/sidb_cluster_hierarchy.hpp``

A cluster hierarchy can be computed over any SiDB layout, resulting in a useful structure for simulation of possible
charge distributions. This header provides the structures used for :ref:`*Ground State Space* <ground_state_space>`
construction, which enables the :ref:`*ClusterComplete* <clustercomplete>` exact SiDB simulator.

.. doxygenenum:: fiction::sidb_cluster_hierarchy_linkage_method
.. doxygenstruct:: fiction::sidb_binary_cluster_hierarchy_node
   :members:
.. doxygenfunction:: fiction::sidb_cluster_hierarchy
.. doxygenstruct:: fiction::sidb_cluster_receptor_state
   :members:
.. doxygenstruct:: fiction::sidb_cluster_projector_state
.. doxygentypedef:: fiction::sidb_cluster_projector_state_ptr
   :members:
.. doxygenenum:: fiction::bound_direction
.. doxygenfunction:: fiction::potential_bound_top
.. doxygenfunction:: fiction::take_meet_of_potential_bounds
.. doxygenstruct:: fiction::potential_bounds_store
   :members:
.. doxygentypedef:: fiction::partial_potential_bounds_store
.. doxygentypedef:: fiction::complete_potential_bounds_store
.. doxygenstruct:: fiction::sidb_charge_space_composition
   :members:
.. doxygenstruct:: fiction::sidb_clustering_state
   :members:
.. doxygenstruct:: fiction::sidb_cluster_charge_state
   :members:
.. doxygenfunction:: fiction::singleton_multiset_conf_to_charge_state
.. doxygenstruct:: fiction::potential_projection
   :members:
.. doxygenstruct:: fiction::potential_projection_order
   :members:
.. doxygentypedef:: fiction::sidb_cluster_charge_state_space
.. doxygentypedef:: fiction::sidb_clustering
.. doxygenstruct:: fiction::sidb_cluster
   :members:
.. doxygenfunction:: fiction::get_projector_state_compositions
.. doxygenfunction:: fiction::to_unique_sidb_cluster
.. doxygenfunction:: fiction::to_sidb_cluster
