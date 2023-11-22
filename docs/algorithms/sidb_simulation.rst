Physical Simulation of Silicon Dangling Bond Logic
--------------------------------------------------

These headers provide functions for physically simulating an SiDB layout, which is a crucial step in the physical design flow of SiDB layouts, as they are used to validate their functionality.


Physical Parameters
###################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp``

        .. doxygenstruct:: fiction::sidb_simulation_parameters
           :members:

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.sidb_simulation_parameters
            :members:


Simulation Result
#################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp``

        .. doxygenstruct:: fiction::sidb_simulation_result
           :members:

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.sidb_simulation_result
            :members:


Heuristic Ground State Simulation
#################################

.. _quicksim:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/quicksim.hpp``

        .. doxygenstruct:: fiction::quicksim_params
           :members:

        .. doxygenfunction:: fiction::quicksim

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.quicksim_params
            :members:

        .. autofunction:: fiction.pyfiction.quicksim


Exhaustive Ground State Simulation
##################################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/quickexact.hpp``

        .. doxygenstruct:: fiction::quickexact_params
           :members:
        .. doxygenfunction:: fiction::quickexact

        **Header:** ``fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp``

        .. doxygenfunction:: fiction::exhaustive_ground_state_simulation

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.quickexact_params
            :members:
        .. autofunction:: fiction.pyfiction.quickexact

        .. autofunction:: fiction.pyfiction.exhaustive_ground_state_simulation


Engine Selectors
################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp``

        .. doxygenenum:: fiction::sidb_simulation_engine
        .. doxygenenum:: fiction::exhaustive_sidb_simulation_engine

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.sidb_simulation_engine
            :members:
        .. autoclass:: fiction.pyfiction.exhaustive_sidb_simulation_engine
            :members:


Energy Calculation
##################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/energy_distribution.hpp``

        .. doxygentypedef:: fiction::sidb_energy_distribution
        .. doxygenfunction:: fiction::energy_distribution


        **Header:** ``fiction/algorithms/simulation/sidb/minimum_energy.hpp``

        .. doxygenfunction:: fiction::minimum_energy


        **Header:** ``fiction/algorithms/simulation/sidb/is_ground_state.hpp``

        .. doxygenfunction:: fiction::is_ground_state

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.energy_distribution

        .. autofunction:: fiction.pyfiction.minimum_energy

        .. autofunction:: fiction.pyfiction.is_ground_state


Temperature Behavior
####################

.. _critical_temperature:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/critical_temperature.hpp``

        .. doxygenstruct:: fiction::critical_temperature_params
           :members:
        .. doxygenfunction:: fiction::critical_temperature_gate_based
        .. doxygenfunction:: fiction::critical_temperature_non_gate_based

        **Header:** ``fiction/algorithms/simulation/sidb/occupation_probability_excited_states.hpp``

        .. doxygenfunction:: fiction::occupation_probability_gate_based
        .. doxygenfunction:: fiction::occupation_probability_non_gate_based

        **Header:** ``fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp``

        .. doxygentypedef:: fiction::sidb_energy_and_state_type
        .. doxygenfunction:: fiction::calculate_energy_and_state_type

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.critical_temperature_mode
            :members:
        .. autoclass:: fiction.pyfiction.simulation_engine
            :members:
        .. autoclass:: fiction.pyfiction.critical_temperature_params
            :members:
        .. autofunction:: fiction.pyfiction.critical_temperature_gate_based
        .. autofunction:: fiction.pyfiction.critical_temperature_non_gate_based

        .. autofunction:: fiction.pyfiction.occupation_probability_gate_based
        .. autofunction:: fiction.pyfiction.occupation_probability_non_gate_based

        .. autofunction:: fiction.pyfiction.calculate_energy_and_state_type


Maximum Defect Influence Distance
#################################

**Header:** ``fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp``

.. doxygenstruct:: fiction::maximum_defect_influence_distance_params
   :members:
.. doxygenfunction:: fiction::maximum_defect_influence_position_and_distance


Time-to-Solution (TTS) Statistics
#################################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/time_to_solution.hpp``

        .. doxygenstruct:: fiction::time_to_solution_params
           :members:
        .. doxygenstruct:: fiction::time_to_solution_stats
           :members:
        .. doxygenfunction:: fiction::time_to_solution

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.time_to_solution_params
            :members:
        .. autoclass:: fiction.pyfiction.time_to_solution_stats
            :members:
        .. autofunction:: fiction.pyfiction.time_to_solution


Random SiDB Layout Generator
############################

**Header:** ``fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp``

.. doxygenstruct:: fiction::generate_random_sidb_layout_params
.. doxygenfunction:: fiction::generate_random_sidb_layout
.. doxygenfunction:: fiction::generate_multiple_random_sidb_layouts


Operational Domain Computation
##############################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/is_operational.hpp``

        .. doxygenenum:: fiction::operational_status
        .. doxygenstruct:: fiction::is_operational_params
           :members:
        .. doxygenfunction:: fiction::is_operational

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

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.operational_status
            :members:
        .. autoclass:: fiction.pyfiction.is_operational_params
            :members:
        .. autofunction:: fiction.pyfiction.is_operational

        .. autoclass:: fiction.pyfiction.sweep_parameter
            :members:
        .. autoclass:: fiction.pyfiction.parameter_point
            :members:
        .. autoclass:: fiction.pyfiction.operational_domain
            :members:
        .. autoclass:: fiction.pyfiction.operational_domain_params
            :members:
        .. autoclass:: fiction.pyfiction.operational_domain_stats
            :members:

        .. autofunction:: fiction.pyfiction.operational_domain_grid_search
        .. autofunction:: fiction.pyfiction.operational_domain_random_sampling
        .. autofunction:: fiction.pyfiction.operational_domain_flood_fill
        .. autofunction:: fiction.pyfiction.operational_domain_contour_tracing


Utility Functions
#################

Charge Detection
^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp``

        .. doxygenfunction:: fiction::can_positive_charges_occur

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.can_positive_charges_occur

Binary-dot Logic (BDL) Pair Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp``

        .. doxygenstruct:: fiction::bdl_pair
           :members:
        .. doxygenstruct:: fiction::detect_bdl_pairs_params
           :members:
        .. doxygenfunction:: fiction::detect_bdl_pairs

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.bdl_pair
            :members:
        .. autoclass:: fiction.pyfiction.detect_bdl_pairs_params
            :members:
        .. autofunction:: fiction.pyfiction.detect_bdl_pairs


Assess Population Stability
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp``

        .. doxygenenum:: fiction::transition_type
        .. doxygenstruct:: fiction::population_stability_information
           :members:
        .. doxygenstruct:: fiction::assess_physical_population_stability_params
           :members:
        .. doxygenfunction:: fiction::assess_physical_population_stability

    .. tab:: Python
        .. autoclass:: fiction.pyfiction.transition_type
            :members:
        .. autoclass:: fiction.pyfiction.population_stability_information
            :members:
        .. autoclass:: fiction.pyfiction.assess_physical_population_stability_params
            :members:
        .. autofunction:: fiction.pyfiction.assess_physical_population_stability


Convert Potential to Distance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/convert_potential_to_distance.hpp``

        .. doxygenfunction:: fiction::convert_potential_to_distance

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.convert_potential_to_distance
