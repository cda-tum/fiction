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
        .. doxygenfunction:: fiction::charge_state_to_sign
        .. doxygenfunction:: fiction::sign_to_charge_state
        .. doxygenfunction:: fiction::charge_configuration_to_string

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_charge_state
            :members:
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
