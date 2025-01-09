Physical Simulation of SiDBs
============================

These headers provide auxiliary functionality for the physical simulation of SiDBs layouts.

SiDB Charge States
------------------

Possible charge states of SiDBs.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb_charge_state.hpp``

        .. doxygenenum:: fiction::sidb_charge_state
        .. doxygenvariable:: fiction::SIDB_CHARGE_STATES_BASE_2
        .. doxygenvariable:: fiction::SIDB_CHARGE_STATES_BASE_3
        .. doxygenfunction:: fiction::sidb_charge_states_for_base
        .. doxygenfunction:: fiction::charge_state_to_sign
        .. doxygenfunction:: fiction::sign_to_charge_state
        .. doxygenfunction:: fiction::charge_configuration_to_string

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_charge_state
            :members:
        .. autofunction:: mnt.pyfiction.sidb_charge_states_for_base_number
        .. autofunction:: mnt.pyfiction.charge_state_to_sign
        .. autofunction:: mnt.pyfiction.sign_to_charge_state
        .. autofunction:: mnt.pyfiction.charge_configuration_to_string


SiDB Charge Distribution Surface
--------------------------------

The charge distribution surface can be layered on top of any SiDB layout to add representation of possible charge
distributions of the SiDBs. Charge distribution surfaces are returned by the SiDB physical simulation algorithms.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/charge_distribution_surface.hpp``

        .. doxygenenum:: fiction::dependent_cell_mode
        .. doxygenenum:: fiction::energy_calculation
        .. doxygenenum:: fiction::charge_distribution_history
        .. doxygenenum:: fiction::charge_index_recomputation
        .. doxygenenum:: fiction::charge_distribution_mode
        .. doxygenenum:: fiction::charge_index_mode
        .. doxygenclass:: fiction::charge_distribution_surface
           :members:
        .. doxygenclass:: fiction::charge_distribution_surface< Lyt, true >
           :members:
        .. doxygenclass:: fiction::charge_distribution_surface< Lyt, false >
           :members:

    .. tab:: Python

        .. autoclass:: mnt.pyfiction.dependent_cell_mode
            :members:
        .. autoclass:: mnt.pyfiction.energy_calculation
            :members:
        .. autoclass:: mnt.pyfiction.charge_index_mode
            :members:
        .. autoclass:: mnt.pyfiction.charge_index_mode
            :members:
        .. autoclass:: mnt.pyfiction.charge_distribution_history
            :members:
        .. autoclass:: mnt.pyfiction.charge_distribution_surface_100
            :members:
        .. autoclass:: mnt.pyfiction.charge_distribution_surface_111
            :members:


Is SiDB gate design deemed impossible
-------------------------------------

**Header:** ``fiction/technology/is_sidb_gate_design_impossible.hpp``

.. doxygenstruct:: fiction::is_sidb_gate_design_impossible_params
   :members:
.. doxygenfunction:: fiction::is_sidb_gate_design_impossible


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

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb_nm_distance.hpp``

        .. doxygenfunction:: fiction::sidb_nm_distance

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_nm_distance_100
        .. autoclass:: mnt.pyfiction.sidb_nm_distance_111


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
