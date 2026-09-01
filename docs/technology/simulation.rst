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

**Header:** ``fiction/technology/sidb/model/physical_constants.hpp``

.. doxygenvariable:: fiction::sidb::model::BOLTZMANN_CONSTANT
.. doxygenvariable:: fiction::sidb::model::EV_TO_JOULE
.. doxygenvariable:: fiction::sidb::model::ELEMENTARY_CHARGE
.. doxygenvariable:: fiction::sidb::model::K_E


Euclidean distance between two SiDBs in nanometers
--------------------------------------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/model/nm_distance.hpp``

        .. doxygenfunction:: fiction::sidb::model::nm_distance

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_nm_distance_100
        .. autoclass:: mnt.pyfiction.sidb_nm_distance_111
