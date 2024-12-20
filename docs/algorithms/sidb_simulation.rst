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
        .. autoclass:: mnt.pyfiction.sidb_simulation_parameters
            :members:


Simulation Result
#################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp``

        .. doxygenstruct:: fiction::sidb_simulation_result
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_simulation_result_100
            :members:
        .. autoclass:: mnt.pyfiction.sidb_simulation_result_111
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
        .. autoclass:: mnt.pyfiction.quicksim_params
            :members:

        .. autofunction:: mnt.pyfiction.quicksim


Exhaustive Ground State Simulation
##################################

.. _quickexact:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/quickexact.hpp``

        .. doxygenstruct:: fiction::quickexact_params
           :members:
        .. doxygenfunction:: fiction::quickexact

        **Header:** ``fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp``

        .. doxygenfunction:: fiction::exhaustive_ground_state_simulation

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.quickexact_params
            :members:
        .. autofunction:: mnt.pyfiction.quickexact
        .. autofunction:: mnt.pyfiction.exhaustive_ground_state_simulation


Engine Selectors
################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp``

        .. doxygenenum:: fiction::sidb_simulation_engine
        .. doxygenenum:: fiction::exact_sidb_simulation_engine

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_simulation_engine
            :members:
        .. autoclass:: mnt.pyfiction.exact_sidb_simulation_engine
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
        .. doxygenfunction:: fiction::minimum_energy_distribution


        **Header:** ``fiction/algorithms/simulation/sidb/is_ground_state.hpp``

        .. doxygenfunction:: fiction::is_ground_state

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.energy_distribution

        .. autofunction:: mnt.pyfiction.minimum_energy

        .. autofunction:: mnt.pyfiction.is_ground_state


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

        **Header:** ``fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp``

        .. doxygenfunction:: fiction::occupation_probability_gate_based
        .. doxygenfunction:: fiction::occupation_probability_non_gate_based

        **Header:** ``fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp``

        .. doxygentypedef:: fiction::sidb_energy_and_state_type
        .. doxygenfunction:: fiction::calculate_energy_and_state_type_with_kinks_accepted
        .. doxygenfunction:: fiction::calculate_energy_and_state_type_with_kinks_rejected

    .. tab:: Python

        .. autoclass:: mnt.pyfiction.critical_temperature_stats
            :members:
        .. autoclass:: mnt.pyfiction.critical_temperature_params
            :members:
        .. autofunction:: mnt.pyfiction.critical_temperature_gate_based
        .. autofunction:: mnt.pyfiction.critical_temperature_non_gate_based

        .. autofunction:: mnt.pyfiction.occupation_probability_gate_based
        .. autofunction:: mnt.pyfiction.occupation_probability_non_gate_based

        .. autofunction:: mnt.pyfiction.calculate_energy_and_state_type_with_kinks_accepted
        .. autofunction:: mnt.pyfiction.calculate_energy_and_state_type_with_kinks_rejected


Fabrication Defects
###################

A collection of tools to model defects that can occur during the fabrication process of FCN technologies.

SiDB Defect Types
-----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb_defects.hpp``

        .. doxygenenum:: fiction::sidb_defect_type

        .. doxygenstruct:: fiction::sidb_defect
           :members:

        .. doxygenfunction:: fiction::is_charged_defect_type
        .. doxygenfunction:: fiction::is_neutral_defect_type

        .. doxygenfunction:: fiction::is_positively_charged_defect
        .. doxygenfunction:: fiction::is_negatively_charged_defect
        .. doxygenfunction:: fiction::is_neutrally_charged_defect

        .. doxygenvariable:: fiction::SIDB_CHARGED_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::SIDB_CHARGED_DEFECT_VERTICAL_SPACING
        .. doxygenvariable:: fiction::SIDB_NEUTRAL_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::SIDB_NEUTRAL_DEFECT_VERTICAL_SPACING

        .. doxygenfunction:: fiction::defect_extent

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_defect_type
            :members:

        .. autoclass:: mnt.pyfiction.sidb_defect
            :members:

        .. autofunction:: mnt.pyfiction.is_charged_defect_type
        .. autofunction:: mnt.pyfiction.is_neutral_defect_type

        .. autofunction:: mnt.pyfiction.is_positively_charged_defect
        .. autofunction:: mnt.pyfiction.is_negatively_charged_defect
        .. autofunction:: mnt.pyfiction.is_neutrally_charged_defect

        .. autofunction:: mnt.pyfiction.defect_extent


SiDB Defect Surface
-------------------

**Header:** ``fiction/technology/sidb_defect_surface.hpp``

A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
fabrication defects on the H-Si(100) 2x1 surface.

.. doxygenclass:: fiction::sidb_defect_surface
   :members:
.. doxygenclass:: fiction::sidb_defect_surface< Lyt, true >
   :members:
.. doxygenclass:: fiction::sidb_defect_surface< Lyt, false >
   :members:


SiDB Defect Analysis
--------------------

**Header:** ``fiction/technology/sidb_surface_analysis.hpp``

.. doxygenfunction:: fiction::sidb_surface_analysis


**Header:** ``fiction/algorithms/simulation/sidb/defect_influence.hpp``

.. doxygenstruct:: fiction::defect_influence_params
   :members:
.. doxygenenum:: fiction::defect_influence_status
.. doxygenstruct:: fiction::defect_influence_domain
   :members:

.. doxygenstruct:: fiction::defect_influence_stats
.. doxygenfunction:: fiction::defect_influence_grid_search(const Lyt& lyt, const std::vector<TT>& spec,
                             const defect_influence_params<cell<Lyt>>& params = {}, const std::size_t step_size = 1,
                             defect_influence_stats* stats = nullptr)
.. doxygenfunction:: fiction::defect_influence_grid_search(const Lyt& lyt, const defect_influence_params<cell<Lyt>>& params = {},
                             const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)
.. doxygenfunction:: fiction::defect_influence_random_sampling(const Lyt& lyt, const std::vector<TT>& spec, std::size_t samples,
                                 const defect_influence_params<cell<Lyt>>& params = {},
                                 defect_influence_stats*                   stats  = nullptr)
.. doxygenfunction:: fiction::defect_influence_random_sampling(const Lyt& lyt, std::size_t samples,
                                 const defect_influence_params<cell<Lyt>>& params = {},
                                 defect_influence_stats*                   stats  = nullptr)
.. doxygenfunction:: fiction::defect_influence_quicktrace(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples,
                            const defect_influence_params<cell<Lyt>>& params = {},
                            defect_influence_stats*                   stats  = nullptr)
.. doxygenfunction:: fiction::defect_influence_quicktrace(const Lyt& lyt, const std::size_t samples,
                            const defect_influence_params<cell<Lyt>>& params = {},
                            defect_influence_stats*                   stats  = nullptr)

**Header:** ``fiction/algorithms/simulation/sidb/calculate_defect_clearance.hpp``

.. doxygenstruct:: fiction::defect_clearance_result
   :members:
.. doxygenfunction:: fiction::calculate_defect_clearance


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
        .. doxygenfunction:: fiction::time_to_solution_for_given_simulation_results

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.time_to_solution_params
            :members:
        .. autoclass:: mnt.pyfiction.time_to_solution_stats
            :members:
        .. autofunction:: mnt.pyfiction.time_to_solution
        .. autofunction:: mnt.pyfiction.time_to_solution_for_given_simulation_results


Random SiDB Layout Generator
############################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp``

        .. doxygenstruct:: fiction::generate_random_sidb_layout_params
           :members:
        .. doxygenfunction:: fiction::generate_random_sidb_layout
        .. doxygenfunction:: fiction::generate_multiple_random_sidb_layouts

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.generate_random_sidb_layout_params
            :members:
        .. autofunction:: mnt.pyfiction.generate_random_sidb_layout
        .. autofunction:: mnt.pyfiction.generate_multiple_random_sidb_layouts



Operational Domain Computation
##############################

.. _opdom:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/is_operational.hpp``

        .. doxygenenum:: fiction::operational_status
        .. doxygenenum:: fiction::operational_condition
        .. doxygenstruct:: fiction::is_operational_params
           :members:
        .. doxygenfunction:: fiction::is_operational
        .. doxygenfunction:: fiction::operational_input_patterns
        .. doxygenfunction:: fiction::is_kink_induced_non_operational
        .. doxygenfunction:: fiction::kink_induced_non_operational_input_patterns

        **Header:** ``fiction/algorithms/simulation/sidb/operational_domain.hpp``

        .. doxygenstruct:: fiction::parameter_point
           :members:
        .. doxygenenum:: fiction::sweep_parameter
        .. doxygenstruct:: fiction::operational_domain
           :members:
        .. doxygenstruct:: fiction::operational_domain_value_range
           :members:
        .. doxygenstruct:: fiction::operational_domain_params
           :members:
        .. doxygenstruct:: fiction::operational_domain_stats
           :members:

        .. doxygenfunction:: fiction::operational_domain_grid_search
        .. doxygenfunction:: fiction::operational_domain_random_sampling
        .. doxygenfunction:: fiction::operational_domain_flood_fill
        .. doxygenfunction:: fiction::operational_domain_contour_tracing

        **Header:** ``fiction/algorithms/simulation/sidb/compute_operational_ratio.hpp``

        .. doxygenstruct:: fiction::compute_operational_ratio_params
           :members:
        .. doxygenfunction:: fiction::compute_operational_ratio

        **Header:** ``fiction/algorithms/simulation/sidb/verify_logic_match.hpp``

        .. doxygenfunction:: fiction::verify_logic_match

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.operational_status
            :members:
        .. autoclass:: mnt.pyfiction.operational_condition
            :members:
        .. autoclass:: mnt.pyfiction.is_operational_params
            :members:
        .. autofunction:: mnt.pyfiction.is_operational
        .. autofunction:: mnt.pyfiction.operational_input_patterns
        .. autofunction:: mnt.pyfiction.is_kink_induced_non_operational
        .. autofunction:: mnt.pyfiction.kink_induced_non_operational_input_patterns

        .. autoclass:: mnt.pyfiction.sweep_parameter
            :members:
        .. autoclass:: mnt.pyfiction.parameter_point
            :members:
        .. autoclass:: mnt.pyfiction.operational_domain
            :members:
        .. autoclass:: mnt.pyfiction.operational_domain_value_range
            :members:
        .. autoclass:: mnt.pyfiction.operational_domain_params
            :members:
        .. autoclass:: mnt.pyfiction.operational_domain_stats
            :members:

        .. autofunction:: mnt.pyfiction.operational_domain_grid_search
        .. autofunction:: mnt.pyfiction.operational_domain_random_sampling
        .. autofunction:: mnt.pyfiction.operational_domain_flood_fill
        .. autofunction:: mnt.pyfiction.operational_domain_contour_tracing

        .. autoclass:: mnt.pyfiction.compute_operational_ratio_params
                :members:

        .. autofunction:: mnt.pyfiction.compute_operational_ratio


Determine Physically Valid Parameters
#####################################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/determine_physically_valid_parameters.hpp``

        .. doxygenfunction:: fiction::determine_physically_valid_parameters

   .. tab:: Python
        .. autoclass:: mnt.pyfiction.determine_physically_valid_parameters


Displacement Robustness Domain
##############################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/determine_displacement_robustness.hpp``

        .. doxygenstruct:: fiction::displacement_robustness_domain
           :members:
        .. doxygenstruct:: fiction::displacement_robustness_domain_params
           :members:
        .. doxygenstruct:: fiction::displacement_robustness_domain_stats
           :members:
        .. doxygenstruct:: fiction::displacement_robustness_domain
           :members:
        .. doxygenfunction:: fiction::determine_displacement_robustness_domain
        .. doxygenfunction:: fiction::probability_of_fabricating_operational_gate

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.dimer_displacement_policy
            :members:
        .. autoclass:: mnt.pyfiction.displacement_analysis_mode
            :members:
        .. autofunction:: mnt.pyfiction.displacement_robustness_domain_params
        .. autofunction:: mnt.pyfiction.displacement_robustness_domain_stats
        .. autofunction:: mnt.pyfiction.displacement_robustness_domain_100
        .. autofunction:: mnt.pyfiction.displacement_robustness_domain_111
        .. autofunction:: mnt.pyfiction.determine_displacement_robustness_domain_100
        .. autofunction:: mnt.pyfiction.determine_displacement_robustness_domain_111

Utility Functions
#################

Simulation Equivalence Checking
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
       **Header:** ``fiction/algorithms/simulation/sidb/check_simulation_results_for_equivalence.hpp``

        .. doxygenfunction:: fiction::check_simulation_results_for_equivalence

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.check_simulation_results_for_equivalence


Determine the Ground State from Simulation Results
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/groundstate_from_simulation_results.hpp``

        .. doxygenfunction:: fiction::groundstate_from_simulation_results

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.groundstate_from_simulation_results


Charge Detection
^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/can_positive_charge_occur.hpp``

        .. doxygenfunction:: fiction::can_positive_charge_occur

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.can_positive_charge_occur


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
        .. autoclass:: mnt.pyfiction.bdl_pair
            :members:
        .. autoclass:: mnt.pyfiction.detect_bdl_pairs_params
            :members:
        .. autofunction:: mnt.pyfiction.detect_bdl_pairs


Binary-dot Logic (BDL) Wire Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp``

        .. doxygenenum:: fiction::bdl_wire_selection
        .. doxygenstruct:: fiction::detect_bdl_wires_params
           :members:
        .. doxygenstruct:: fiction::bdl_wire
           :members:
        .. doxygenfunction:: fiction::detect_bdl_wires

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.bdl_wire_selection
            :members:
        .. autoclass:: mnt.pyfiction.detect_bdl_wires_params
            :members:
        .. autoclass:: mnt.pyfiction.bdl_wire_100
            :members:
        .. autoclass:: mnt.pyfiction.bdl_wire_111
            :members:
        .. autofunction:: mnt.pyfiction.detect_bdl_wires_100
        .. autofunction:: mnt.pyfiction.detect_bdl_wires_111


Assess Population Stability
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/physical_population_stability.hpp``

        .. doxygenenum:: fiction::transition_type
        .. doxygenstruct:: fiction::population_stability_information
           :members:
        .. doxygenstruct:: fiction::physical_population_stability_params
           :members:
        .. doxygenfunction:: fiction::physical_population_stability

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.transition_type
            :members:
        .. autoclass:: mnt.pyfiction.population_stability_information_100
            :members:
        .. autoclass:: mnt.pyfiction.population_stability_information_111
            :members:
        .. autoclass:: mnt.pyfiction.physical_population_stability_params
            :members:
        .. autofunction:: mnt.pyfiction.assess_physical_population_stability_100
        .. autofunction:: mnt.pyfiction.assess_physical_population_stability_111


Band-Bending Resilience
^^^^^^^^^^^^^^^^^^^^^^^

**Header:** ``fiction/algorithms/simulation/sidb/physical_population_stability.hpp``

.. doxygenstruct:: fiction::band_bending_resilience_params
   :members:
.. doxygenstruct:: fiction::band_bending_resilience


Convert Potential to Distance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/simulation/sidb/convert_potential_to_distance.hpp``

        .. doxygenfunction:: fiction::convert_potential_to_distance

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.convert_potential_to_distance
