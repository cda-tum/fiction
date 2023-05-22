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
        .. autoclass:: fiction.pyfiction.sidb_charge_state
            :members:
        .. autofunction:: fiction.pyfiction.charge_state_to_sign
        .. autofunction:: fiction.pyfiction.sign_to_charge_state
        .. autofunction:: fiction.pyfiction.charge_configuration_to_string


SiDB Charge Distribution Surface
--------------------------------

The charge distribution surface can be layered on top of any SiDB layout to add representation of possible charge
distributions of the SiDBs. Charge distribution surfaces are returned by the SiDB physical simulation algorithms.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/charge_distribution_surface.hpp``

        .. doxygenclass:: fiction::charge_distribution_surface
           :members:
        .. doxygenclass:: fiction::charge_distribution_surface< Lyt, true >
           :members:
        .. doxygenclass:: fiction::charge_distribution_surface< Lyt, false >
           :members:

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.charge_distribution_surface
            :members:
