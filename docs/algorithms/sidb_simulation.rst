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


**Header:** ``fiction/algorithms/simulation/sidb/minimum_energy.hpp``

.. doxygenfunction:: fiction::minimum_energy


**Header:** ``fiction/algorithms/simulation/sidb/is_ground_state.hpp``

.. doxygenfunction:: fiction::is_ground_state


Time-to-Solution (TTS) Statistics
#################################

**Header:** ``fiction/algorithms/simulation/sidb/time_to_solution.hpp``

.. doxygenfunction:: fiction::sim_acc_tts


Random Layout Generator
#################################

**Header:** ``fiction/algorithms/simulation/sidb/random_layout_generator.hpp``

.. doxygenfunction:: fiction::generate_random_layout(const random_layout_params<Lyt>& params, std::ostream& os,
                            const std::vector<Lyt>& all_layouts = {}))
.. doxygenfunction:: fiction::generate_random_layout(const random_layout_params<Lyt>& params, const std::string_view& filename,
                            const std::vector<Lyt>& all_layouts = {})
