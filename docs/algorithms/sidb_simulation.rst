SiDB Electrostatic Ground State Simulation
------------------------------------------

These headers provide functions for physically simulating the *ground state* of an SiDB layout. Ground state simulations
are a crucial step in the physical design flow of SiDB layouts, as they are used to validate their functionality.


Physical Parameters
###################

**Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp``

.. doxygenstruct:: fiction::sidb_simulation_parameters
   :members:


Simulation Result
#################

**Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp``

.. doxygenstruct:: fiction::sidb_simulation_result
   :members:


Heuristic Ground State Simulation
#################################

.. _quicksim:

**Header:** ``fiction/algorithms/simulation/sidb/quicksim.hpp``

.. doxygenstruct:: fiction::quicksim_params
   :members:

.. doxygenfunction:: fiction::quicksim


Exhaustive Ground State Simulation
##################################

**Header:** ``fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp``

.. doxygenfunction:: fiction::exhaustive_ground_state_simulation


Energy Calculation
##################

**Header:** ``fiction/algorithms/simulation/sidb/energy_distribution.hpp``

.. doxygenfunction:: fiction::energy_distribution
.. doxygentypedef:: fiction::sidb_energy_distribution
    :project: fiction


**Header:** ``fiction/algorithms/simulation/sidb/minimum_energy.hpp``

.. doxygenfunction:: fiction::minimum_energy


**Header:** ``fiction/algorithms/simulation/sidb/is_ground_state.hpp``

.. doxygenfunction:: fiction::is_ground_state


Temperature Behavior
####################

**Header:** ``fiction/algorithms/simulation/sidb/critical_temperature.hpp``

.. doxygenfunction:: fiction::critical_temperature

**Header:** ``fiction/algorithms/simulation/sidb/occupation_probability_excited_states.hpp``

.. doxygenfunction:: fiction::occupation_probability_gate_based
.. doxygenfunction:: fiction::occupation_probability_non_gate_based

**Header:** ``fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp``

.. doxygenfunction:: fiction::calculate_energy_and_state_type
.. doxygentypedef:: fiction::sidb_energy_and_state_type
    :project: fiction


Time-to-Solution (TTS) Statistics
#################################

**Header:** ``fiction/algorithms/simulation/sidb/time_to_solution.hpp``

.. doxygenfunction:: fiction::sim_acc_tts
