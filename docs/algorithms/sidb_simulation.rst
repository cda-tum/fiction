Physical Simulation of Silicon Dangling Bond Logic
--------------------------------------------------

These headers provide functions for physically simulating an SiDB layout, which is a crucial step in the physical design flow of SiDB layouts, as they are used to validate their functionality.


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

.. _quickexact:

**Header:** ``fiction/algorithms/simulation/sidb/quickexact.hpp``

.. doxygenenum:: fiction::required_simulation_base_number
.. doxygenstruct:: fiction::quickexact_params
   :members:
.. doxygenfunction:: fiction::quickexact

**Header:** ``fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp``

.. doxygenfunction:: fiction::exhaustive_ground_state_simulation


Engine Selectors
################

**Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp``

.. doxygenenum:: fiction::sidb_simulation_engine
.. doxygenenum:: fiction::exhaustive_sidb_simulation_engine


Energy Calculation
##################

**Header:** ``fiction/algorithms/simulation/sidb/energy_distribution.hpp``

.. doxygentypedef:: fiction::sidb_energy_distribution
.. doxygenfunction:: fiction::energy_distribution


**Header:** ``fiction/algorithms/simulation/sidb/minimum_energy.hpp``

.. doxygenfunction:: fiction::minimum_energy
.. doxygenfunction:: fiction::minimum_energy_distribution


**Header:** ``fiction/algorithms/simulation/sidb/is_ground_state.hpp``

.. doxygenfunction:: fiction::is_ground_state


**Header:** ``fiction/algorithms/simulation/sidb/check_simulation_results_for_equivalence.hpp``

.. doxygenfunction:: fiction::check_simulation_results_for_equivalence


**Header:** ``fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp``

.. doxygenfunction:: fiction::determine_groundstate_from_simulation_results


Temperature Behavior
####################

.. _critical_temperature:

**Header:** ``fiction/algorithms/simulation/sidb/critical_temperature.hpp``

.. doxygenstruct:: fiction::critical_temperature_params
   :members:
.. doxygenfunction:: fiction::critical_temperature_gate_based
.. doxygenfunction:: fiction::critical_temperature_non_gate_based

**Header:** ``fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp``

.. doxygenfunction:: fiction::occupation_probability_gate_based
.. doxygenfunction:: fiction::occupation_probability_non_gate_based

**Header:** ``fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp``

.. doxygentypedef:: fiction::sidb_energy_and_state_type
.. doxygenfunction:: fiction::calculate_energy_and_state_type


Maximum Defect Influence Distance
#################################

**Header:** ``fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp``

.. doxygenstruct:: fiction::maximum_defect_influence_distance_params
   :members:
.. doxygenfunction:: fiction::maximum_defect_influence_position_and_distance


Time-to-Solution (TTS) Statistics
#################################

**Header:** ``fiction/algorithms/simulation/sidb/time_to_solution.hpp``

.. doxygenstruct:: fiction::time_to_solution_params
   :members:
.. doxygenstruct:: fiction::time_to_solution_stats
   :members:
.. doxygenfunction:: fiction::time_to_solution


Random SiDB Layout Generator
############################

**Header:** ``fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp``

.. doxygenstruct:: fiction::generate_random_sidb_layout_params
   :members:
.. doxygenfunction:: fiction::generate_random_sidb_layout
.. doxygenfunction:: fiction::generate_multiple_random_sidb_layouts


Operational Domain Computation
##############################

.. _opdom:

**Header:** ``fiction/algorithms/simulation/sidb/is_operational.hpp``

.. doxygenenum:: fiction::operational_status
.. doxygenstruct:: fiction::is_operational_params
   :members:
.. doxygenfunction:: fiction::is_operational
.. doxygenfunction:: fiction::operational_input_patterns

**Header:** ``fiction/algorithms/simulation/sidb/operational_domain.hpp``

.. doxygenstruct:: fiction::operational_domain
   :members:

.. doxygenstruct:: fiction::operational_domain_params
   :members:
.. doxygenstruct:: fiction::operational_domain_stats
   :members:

.. doxygenfunction:: fiction::operational_domain_grid_search
.. doxygenfunction:: fiction::operational_domain_random_sampling
.. doxygenfunction:: fiction::operational_domain_flood_fill
.. doxygenfunction:: fiction::operational_domain_contour_tracing


Utility Functions
#################


Binary-dot Logic (BDL) Pair Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Header:** ``fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp``

.. doxygenstruct:: fiction::bdl_pair
   :members:
.. doxygenstruct:: fiction::detect_bdl_pairs_params
   :members:
.. doxygenfunction:: fiction::detect_bdl_pairs


Assess Population Stability
^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Header:** ``fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp``

.. doxygenenum:: fiction::transition_type
.. doxygenstruct:: fiction::population_stability_information
   :members:
.. doxygenstruct:: fiction::assess_physical_population_stability_params
   :members:
.. doxygenfunction:: fiction::assess_physical_population_stability


Positive Charge Detection
^^^^^^^^^^^^^^^^^^^^^^^^^

**Header:** ``fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp``

.. doxygenfunction:: fiction::can_positive_charges_occur


Convert Potential to Distance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Header:** ``fiction/algorithms/simulation/sidb/convert_potential_to_distance.hpp``

.. doxygenfunction:: fiction::convert_potential_to_distance
