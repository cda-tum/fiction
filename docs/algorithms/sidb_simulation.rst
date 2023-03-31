SiDB Electrostatic Ground State Simulation
------------------------------------------

These headers provide functions for physically simulating the *ground state* of an SiDB layout. Ground state simulations
are a crucial step in the physical design flow of SiDB layouts, as they are used to validate their functionality.


Physical Parameters
###################


**Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp``

.. doxygenstruct:: fiction::sidb_simulation_parameters
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


**Header:** ``fiction/algorithms/simulation/sidb/minimum_energy.hpp``

.. doxygenfunction:: fiction::minimum_energy


**Header:** ``fiction/algorithms/simulation/sidb/is_ground_state.hpp``

.. doxygenfunction:: fiction::is_ground_state


Time-to-Solution (TTS) Statistics
#################################

**Header:** ``fiction/algorithms/simulation/sidb/time_to_solution.hpp``

.. doxygenfunction:: fiction::sim_acc_tts

**Header:** ``fiction/algorithms/simulation/sidb/critical_temperature.hpp``

.. doxygenfunction:: fiction::critical_temperature(const Lyt &lyt, const std::string &gate, const std::string &input_bits, const sidb_simulation_parameters &params = sidb_simulation_parameters{}, critical_temperature_stats<Lyt> *pst = nullptr, const double confidence_level = 0.99, const uint64_t max_temperature = 400) noexcept

**Header:** ``fiction/algorithms/simulation/sidb/occupation_function_erroneous.hpp``

.. doxygenfunction:: fiction::occupation_probability(const std::vector<std::pair<double, bool>> &energy_distribution_transparent_erroneous, const double &temperature) noexcept