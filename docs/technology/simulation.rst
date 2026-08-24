Physical Simulation of SiDBs
============================

These headers provide auxiliary functionality for the physical simulation of SiDBs layouts.

SiDB Charge States
------------------

Possible charge states of SiDBs.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/model/charge_state.hpp``

        .. doxygenenum:: fiction::sidb::model::charge_state
        .. doxygenvariable:: fiction::sidb::model::SIDB_CHARGE_STATES_BASE_2
        .. doxygenvariable:: fiction::sidb::model::SIDB_CHARGE_STATES_BASE_3
        .. doxygenfunction:: fiction::sidb::model::charge_states_for_base_number
        .. doxygenfunction:: fiction::sidb::model::charge_state_to_sign
        .. doxygenfunction:: fiction::sidb::model::sign_to_charge_state
        .. doxygenfunction:: fiction::sidb::model::charge_configuration_to_string

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
        **Header:** ``fiction/technology/sidb/primitives/charge_distribution_surface.hpp``

        .. doxygenenum:: fiction::sidb::primitives::dependent_cell_mode
        .. doxygenenum:: fiction::sidb::primitives::energy_calculation
        .. doxygenenum:: fiction::sidb::primitives::charge_distribution_history
        .. doxygenenum:: fiction::sidb::primitives::charge_index_recomputation
        .. doxygenenum:: fiction::sidb::primitives::charge_distribution_mode
        .. doxygenenum:: fiction::sidb::primitives::charge_index_mode
        .. doxygenenum:: fiction::sidb::primitives::charge_transition_threshold_bounds
        .. doxygenclass:: fiction::sidb::primitives::charge_distribution_surface
           :members:
        .. doxygenclass:: fiction::sidb::primitives::charge_distribution_surface< Lyt, true >
           :members:
        .. doxygenclass:: fiction::sidb::primitives::charge_distribution_surface< Lyt, false >
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

**Header:** ``fiction/technology/sidb/generators/is_gate_design_impossible.hpp``

.. doxygenstruct:: fiction::sidb::generators::is_gate_design_impossible_params
   :members:
.. doxygenfunction:: fiction::sidb::generators::is_gate_design_impossible


Physical Constants
------------------

**Header:** ``fiction/technology/fcn/constants.hpp``

.. doxygenvariable:: fiction::fcn::constants::physical::BOLTZMANN_CONSTANT
.. doxygenvariable:: fiction::fcn::constants::physical::EV_TO_JOULE
.. doxygenvariable:: fiction::fcn::constants::physical::EPSILON
.. doxygenvariable:: fiction::fcn::constants::physical::ELEMENTARY_CHARGE
.. doxygenvariable:: fiction::fcn::constants::physical::K_E
.. doxygenvariable:: fiction::fcn::constants::ERROR_MARGIN
.. doxygenvariable:: fiction::fcn::constants::PI


Euclidean distance between two SiDBs in nanometers
--------------------------------------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/model/nm_distance.hpp``

        .. doxygenfunction:: fiction::sidb::model::nm_distance

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_nm_distance_100
        .. autoclass:: mnt.pyfiction.sidb_nm_distance_111


SiDB Cluster Hierarchy
----------------------

**Header:** ``fiction/technology/sidb/model/cluster_hierarchy.hpp``

A cluster hierarchy can be computed over any SiDB layout, resulting in a useful structure for simulation of possible
charge distributions. This header provides the structures used for :ref:`*Ground State Space* <ground_state_space>`
construction, which enables the :ref:`*ClusterComplete* <clustercomplete>` exact SiDB simulator.

.. doxygenenum:: fiction::sidb::model::cluster_hierarchy_linkage_method
.. doxygenstruct:: fiction::sidb::model::binary_cluster_hierarchy_node
   :members:
.. doxygenfunction:: fiction::sidb::model::cluster_hierarchy
.. doxygenstruct:: fiction::sidb::model::cluster_receptor_state
   :members:
.. doxygenstruct:: fiction::sidb::model::cluster_projector_state
.. doxygentypedef:: fiction::sidb::model::cluster_projector_state_ptr
.. doxygenenum:: fiction::sidb::model::bound_direction
.. doxygenfunction:: fiction::sidb::model::potential_bound_top
.. doxygenfunction:: fiction::sidb::model::take_meet_of_potential_bounds
.. doxygenstruct:: fiction::sidb::model::potential_bounds_store
   :members:
.. doxygentypedef:: fiction::sidb::model::partial_potential_bounds_store
.. doxygentypedef:: fiction::sidb::model::complete_potential_bounds_store
.. doxygenstruct:: fiction::sidb::model::charge_space_composition
   :members:
.. doxygenstruct:: fiction::sidb::model::clustering_state
   :members:
.. doxygenstruct:: fiction::sidb::model::cluster_charge_state
   :members:
.. doxygenfunction:: fiction::sidb::model::singleton_multiset_conf_to_charge_state
.. doxygenstruct:: fiction::sidb::model::potential_projection
   :members:
.. doxygenstruct:: fiction::sidb::model::potential_projection_order
   :members:
.. doxygentypedef:: fiction::sidb::model::cluster_charge_state_space
.. doxygentypedef:: fiction::sidb::model::clustering
.. doxygenstruct:: fiction::sidb::model::cluster
   :members:
.. doxygenfunction:: fiction::sidb::model::get_projector_state_compositions
.. doxygenfunction:: fiction::sidb::model::to_unique_sidb_cluster
.. doxygenfunction:: fiction::sidb::model::to_sidb_cluster
