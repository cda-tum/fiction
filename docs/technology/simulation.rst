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
