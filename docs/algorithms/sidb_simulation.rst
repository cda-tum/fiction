Physical Simulation of Silicon Dangling Bond Logic
--------------------------------------------------

These headers provide functions for physically simulating an SiDB layout, which is a crucial step in the physical design flow of SiDB layouts, as they are used to validate their functionality.


Physical Parameters
###################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/model/simulation_parameters.hpp``

        .. doxygenstruct:: fiction::sidb::model::simulation_parameters
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_simulation_parameters
            :members:


Simulation Result
#################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/result.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::result
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
        **Header:** ``fiction/technology/sidb/simulation/engines/quicksim.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::engines::quicksim_params
           :members:

        .. doxygenfunction:: fiction::sidb::simulation::engines::quicksim

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.quicksim_params
            :members:

        .. autofunction:: mnt.pyfiction.quicksim


Exhaustive Ground State Simulation
##################################

.. tabs::
    .. tab:: C++

        .. _quickexact:

        **Header:** ``fiction/technology/sidb/simulation/engines/quickexact.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::engines::quickexact_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::engines::quickexact

        .. _clustercomplete:

        **Header:** ``fiction/technology/sidb/simulation/engines/clustercomplete.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::engines::clustercomplete_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::engines::clustercomplete

        **Header:** ``fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::engines::exhaustive_ground_state_simulation

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.quickexact_params
            :members:
        .. autofunction:: mnt.pyfiction.quickexact
        .. autoclass:: mnt.pyfiction.clustercomplete_params
            :members:
        .. autofunction:: mnt.pyfiction.clustercomplete
        .. autofunction:: mnt.pyfiction.exhaustive_ground_state_simulation


Engine Selectors
################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/engine.hpp``

        .. doxygenenum:: fiction::sidb::simulation::engine
        .. doxygenenum:: fiction::sidb::simulation::exact_engine
        .. doxygenenum:: fiction::sidb::simulation::heuristic_engine
        .. doxygenfunction:: fiction::sidb::simulation::engine_name

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_simulation_engine
            :members:
        .. autoclass:: mnt.pyfiction.exact_sidb_simulation_engine
            :members:
        .. autoclass:: mnt.pyfiction.heuristic_sidb_simulation_engine
            :members:
        .. autofunction:: mnt.pyfiction.sidb_simulation_engine_name

Energy Calculation
##################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/analysis/energy_distribution.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::analysis::energy_state
           :members:
        .. doxygenclass:: fiction::sidb::simulation::analysis::energy_distribution
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::analysis::calculate_energy_distribution


        **Header:** ``fiction/technology/sidb/simulation/analysis/minimum_energy.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::analysis::minimum_energy
        .. doxygenfunction:: fiction::sidb::simulation::analysis::minimum_energy_distribution


        **Header:** ``fiction/technology/sidb/simulation/is_ground_state.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::is_ground_state

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.calculate_energy_distribution

        .. autofunction:: mnt.pyfiction.minimum_energy

        .. autofunction:: mnt.pyfiction.is_ground_state


Temperature Behavior
####################

.. _critical_temperature:

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/analysis/critical_temperature.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::analysis::critical_temperature_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::analysis::critical_temperature_gate_based(const Lyt& lyt, const std::vector<TT>& spec, const critical_temperature_params& params = {}, critical_temperature_stats* pst = nullptr)
        .. doxygenfunction:: fiction::sidb::simulation::analysis::critical_temperature_gate_based(const std::vector<Lyt>& input_pattern_layouts, const std::vector<TT>& spec, const critical_temperature_params& params, const std::vector<bdl_pair<cell<Lyt>>>& output_bdl_pairs, const std::vector<bdl_wire<Lyt>>& input_bdl_wires, const std::vector<bdl_wire<Lyt>>& output_bdl_wires, critical_temperature_stats* pst = nullptr)
        .. doxygenfunction:: fiction::sidb::simulation::analysis::critical_temperature_non_gate_based

        **Header:** ``fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::analysis::calculate_boltzmann_factor
        .. doxygenfunction:: fiction::sidb::simulation::analysis::occupation_probability_gate_based
        .. doxygenfunction:: fiction::sidb::simulation::analysis::occupation_probability_non_gate_based

        **Header:** ``fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp``

        .. doxygenenum:: fiction::sidb::simulation::analysis::state_type
        .. doxygentypedef:: fiction::sidb::simulation::analysis::energy_and_state_type
        .. doxygenfunction:: fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_accepted
        .. doxygenfunction:: fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_rejected

    .. tab:: Python

        .. autoclass:: mnt.pyfiction.critical_temperature_stats
            :members:
        .. autoclass:: mnt.pyfiction.critical_temperature_params
            :members:
        .. autofunction:: mnt.pyfiction.critical_temperature_gate_based
        .. autofunction:: mnt.pyfiction.critical_temperature_non_gate_based

        .. autofunction:: mnt.pyfiction.occupation_probability_gate_based
        .. autofunction:: mnt.pyfiction.occupation_probability_non_gate_based

        .. autoclass:: mnt.pyfiction.state_type
            :members:
        .. autofunction:: mnt.pyfiction.calculate_energy_and_state_type_with_kinks_accepted
        .. autofunction:: mnt.pyfiction.calculate_energy_and_state_type_with_kinks_rejected

Ground State Space Construction
###############################

.. _ground_state_space:

**Header:** ``fiction/technology/sidb/simulation/engines/ground_state_space.hpp``

.. doxygenstruct:: fiction::sidb::simulation::engines::ground_state_space_results
   :members:
.. doxygenfunction:: fiction::sidb::simulation::engines::ground_state_space


Time-to-Solution (TTS) Statistics
#################################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/analysis/time_to_solution.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::analysis::time_to_solution_params
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::analysis::time_to_solution_stats
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::analysis::time_to_solution
        .. doxygenfunction:: fiction::sidb::simulation::analysis::time_to_solution_for_given_simulation_results

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
        **Header:** ``fiction/technology/sidb/generators/random_layout_generator.hpp``

        .. doxygenstruct:: fiction::sidb::generators::generate_random_layout_params
           :members:
        .. doxygenfunction:: fiction::sidb::generators::generate_random_layout
        .. doxygenfunction:: fiction::sidb::generators::generate_multiple_random_layouts

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
        **Header:** ``fiction/technology/sidb/simulation/logic/is_operational.hpp``

        .. doxygenenum:: fiction::sidb::simulation::logic::operational_status
        .. doxygenstruct:: fiction::sidb::simulation::logic::is_operational_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::logic::is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})
        .. doxygenfunction:: fiction::sidb::simulation::logic::is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)
        .. doxygenfunction:: fiction::sidb::simulation::logic::is_operational(const std::vector<Lyt>& input_pattern_layouts, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)
        .. doxygenfunction:: fiction::sidb::simulation::logic::operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})
        .. doxygenfunction:: fiction::sidb::simulation::logic::operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)
        .. doxygenfunction:: fiction::sidb::simulation::logic::is_kink_induced_non_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})
        .. doxygenfunction:: fiction::sidb::simulation::logic::is_kink_induced_non_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)
        .. doxygenfunction:: fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {})
        .. doxygenfunction:: fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)

        **Header:** ``fiction/technology/sidb/simulation/logic/operational_domain.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::logic::parameter_point
           :members:
        .. doxygenenum:: fiction::sidb::simulation::logic::sweep_parameter
        .. doxygenclass:: fiction::sidb::simulation::logic::operational_domain
           :members:
        .. doxygenclass:: fiction::sidb::simulation::logic::critical_temperature_domain
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::logic::operational_domain_value_range
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::logic::operational_domain_params
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::logic::operational_domain_stats
           :members:

        Four strategies sample the parameter space. Grid search and random sampling place their samples
        independently and accept any number of sweep dimensions. Flood fill and contour tracing follow the
        shape of the operational region and therefore need a neighborhood to expand over, which requires at
        least two dimensions. In two dimensions, contour tracing walks the boundary as a closed curve; in
        three or more, where the boundary is a surface, it collects the boundary instead.

        Setting ``strategy_to_analyze_operational_status`` to ``FILTER_ONLY`` computes the *operational
        domain sketch*: each parameter point is classified by filtering alone, without physical simulation.
        This is dramatically faster and never rejects a point that is operational, but it does report some
        non-operational points as operational. The sketch requires ``REJECT_KINKS`` and a layout with
        ``LOGIC`` cells, since the filtering steps enumerate the charge configurations of the canvas those
        cells define; without either, it is rejected with ``std::invalid_argument``.

        The sketch combines with any of the four strategies, but pairs best with the exhaustive ones. Grid
        search and random sampling place their samples independently of the result, so the sketch simply makes
        each of them cheaper. Flood fill and contour tracing instead follow the boundary of the *sketch*
        region rather than of the operational region. That is sound, since the former contains the latter, but
        it is only useful when the initial random sampling lands inside the region: over a three-dimensional
        range, a sample count that suffices in two dimensions frequently misses the region altogether and
        returns almost nothing. Where the sketch region does fill the swept range, the frontier never stops
        and the flood fill visits every point anyway, at a higher per-point cost than a grid search. Prefer
        grid search with the sketch, and raise the sample count substantially before combining the sketch with
        flood fill or contour tracing in three dimensions.

        .. doxygenfunction:: fiction::sidb::simulation::logic::operational_domain_grid_search
        .. doxygenfunction:: fiction::sidb::simulation::logic::operational_domain_random_sampling
        .. doxygenfunction:: fiction::sidb::simulation::logic::operational_domain_flood_fill
        .. doxygenfunction:: fiction::sidb::simulation::logic::operational_domain_contour_tracing
        .. doxygenfunction:: fiction::sidb::simulation::logic::critical_temperature_domain_grid_search
        .. doxygenfunction:: fiction::sidb::simulation::logic::critical_temperature_domain_random_sampling
        .. doxygenfunction:: fiction::sidb::simulation::logic::critical_temperature_domain_flood_fill
        .. doxygenfunction:: fiction::sidb::simulation::logic::critical_temperature_domain_contour_tracing

        **Header:** ``fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::logic::operational_domain_ratio_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::logic::operational_domain_ratio

        **Header:** ``fiction/technology/sidb/simulation/logic/verify_logic_match.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::logic::verify_logic_match

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.operational_status
            :members:
        .. autoclass:: mnt.pyfiction.operational_condition
            :members:
        .. autoclass:: mnt.pyfiction.operational_analysis_strategy
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
        .. autoclass:: mnt.pyfiction.critical_temperature_domain
            :members:
        .. autoclass:: mnt.pyfiction.operational_domain_value_range
            :members:
        .. autoclass:: mnt.pyfiction.operational_domain_params
            :members:
        .. autoclass:: mnt.pyfiction.operational_domain_stats
            :members:

        The same four strategies are available here. Grid search and random sampling accept any number of
        sweep dimensions; flood fill and contour tracing follow the shape of the operational region and
        therefore need at least two. In two dimensions, contour tracing walks the boundary as a closed
        curve; in three or more, where the boundary is a surface, it collects the boundary instead.

        Setting ``strategy_to_analyze_operational_status`` to ``FILTER_ONLY`` computes the *operational
        domain sketch*, which classifies each parameter point by filtering alone instead of by physical
        simulation. It requires ``REJECT_KINKS`` and a layout with ``LOGIC`` cells; without either, the
        call raises ``ValueError``. The sketch pairs best with grid search and random sampling — see the
        C++ tab for why combining it with flood fill or contour tracing needs a much higher sample count.

        .. autofunction:: mnt.pyfiction.operational_domain_grid_search
        .. autofunction:: mnt.pyfiction.operational_domain_random_sampling
        .. autofunction:: mnt.pyfiction.operational_domain_flood_fill
        .. autofunction:: mnt.pyfiction.operational_domain_contour_tracing
        .. autofunction:: mnt.pyfiction.critical_temperature_domain_grid_search
        .. autofunction:: mnt.pyfiction.critical_temperature_domain_random_sampling
        .. autofunction:: mnt.pyfiction.critical_temperature_domain_flood_fill
        .. autofunction:: mnt.pyfiction.critical_temperature_domain_contour_tracing

        .. autoclass:: mnt.pyfiction.operational_domain_ratio_params
                :members:

        .. autofunction:: mnt.pyfiction.operational_domain_ratio


Physically Valid Parameters
###########################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::analysis::physically_valid_parameters

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.physically_valid_parameters_domain
             :members:
        .. autoclass:: mnt.pyfiction.physically_valid_parameters


Displacement Robustness Domain
##############################

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/defects/displacement_robustness_domain.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::defects::displacement_robustness_domain
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::defects::displacement_robustness_domain_params
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::defects::displacement_robustness_domain_stats
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::defects::determine_displacement_robustness_domain
        .. doxygenfunction:: fiction::sidb::simulation::defects::determine_probability_of_fabricating_operational_gate

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
       **Header:** ``fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::check_simulation_results_for_equivalence

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.check_simulation_results_for_equivalence


Charge Detection
^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp``

        .. doxygenfunction:: fiction::sidb::simulation::analysis::can_positive_charges_occur

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.can_positive_charges_occur


Binary-dot Logic (BDL) Pair Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::logic::bdl_pair
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::logic::detect_bdl_pairs_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::logic::detect_bdl_pairs

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
        **Header:** ``fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp``

        .. doxygenenum:: fiction::sidb::simulation::logic::bdl_wire_selection
        .. doxygenstruct:: fiction::sidb::simulation::logic::detect_bdl_wires_params
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::logic::bdl_wire
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::logic::detect_bdl_wires

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
        **Header:** ``fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp``

        .. doxygenenum:: fiction::sidb::simulation::analysis::transition_type
        .. doxygenstruct:: fiction::sidb::simulation::analysis::population_stability_information
           :members:
        .. doxygenstruct:: fiction::sidb::simulation::analysis::physical_population_stability_params
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::analysis::physical_population_stability

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.transition_type
            :members:
        .. autoclass:: mnt.pyfiction.population_stability_information_100
            :members:
        .. autoclass:: mnt.pyfiction.population_stability_information_111
            :members:
        .. autoclass:: mnt.pyfiction.physical_population_stability_params
            :members:
        .. autofunction:: mnt.pyfiction.physical_population_stability_100
        .. autofunction:: mnt.pyfiction.physical_population_stability_111


Band-Bending Resilience
^^^^^^^^^^^^^^^^^^^^^^^

.. _band_bending_resilience:

**Header:** ``fiction/technology/sidb/simulation/analysis/band_bending_resilience.hpp``

.. doxygenstruct:: fiction::sidb::simulation::analysis::band_bending_resilience_params
   :members:
.. doxygenfunction:: fiction::sidb::simulation::analysis::band_bending_resilience


Convert Potential to Distance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/model/potential_to_distance_conversion.hpp``

        .. doxygenfunction:: fiction::sidb::model::potential_to_distance_conversion

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.potential_to_distance_conversion


Fabrication Defects
###################

A collection of tools to simulate defects that can occur during the fabrication process of FCN technologies.

SiDB Defect Types
^^^^^^^^^^^^^^^^^

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/model/defect.hpp``

        .. doxygenenum:: fiction::sidb::model::defect_type

        .. doxygenstruct:: fiction::sidb::model::defect
           :members:

        .. doxygenfunction:: fiction::sidb::model::is_charged_defect_type
        .. doxygenfunction:: fiction::sidb::model::is_neutral_defect_type

        .. doxygenfunction:: fiction::sidb::model::is_positively_charged_defect
        .. doxygenfunction:: fiction::sidb::model::is_negatively_charged_defect
        .. doxygenfunction:: fiction::sidb::model::is_neutrally_charged_defect

        .. doxygenvariable:: fiction::sidb::model::CHARGED_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::sidb::model::CHARGED_DEFECT_VERTICAL_SPACING
        .. doxygenvariable:: fiction::sidb::model::NEUTRAL_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::sidb::model::NEUTRAL_DEFECT_VERTICAL_SPACING

        .. doxygenfunction:: fiction::sidb::model::defect_extent

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
^^^^^^^^^^^^^^^^^^

**Header:** ``fiction/technology/sidb/surfaces/defect_surface.hpp``

A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
fabrication defects on the H-Si(100) 2x1 surface.

.. doxygenclass:: fiction::sidb::surfaces::defect_surface
   :members:
.. doxygenclass:: fiction::sidb::surfaces::defect_surface< Lyt, true >
   :members:
.. doxygenclass:: fiction::sidb::surfaces::defect_surface< Lyt, false >
   :members:


SiDB Defect Analysis
^^^^^^^^^^^^^^^^^^^^

.. _defect_influence:

**Header:** ``fiction/technology/sidb/surface_analysis.hpp``

.. doxygenfunction:: fiction::sidb::surface_analysis


**Header:** ``fiction/technology/sidb/simulation/defects/defect_influence.hpp``

.. doxygenstruct:: fiction::sidb::simulation::defects::defect_influence_params
   :members:
.. doxygenenum:: fiction::sidb::simulation::defects::defect_influence_status
.. doxygenclass:: fiction::sidb::simulation::defects::defect_influence_domain
   :members:

.. doxygenstruct:: fiction::sidb::simulation::defects::defect_influence_stats
.. doxygenfunction:: fiction::sidb::simulation::defects::defect_influence_grid_search(const Lyt& lyt, const std::vector<TT>& spec, const defect_influence_params<cell<Lyt>>& params = {}, const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)
.. doxygenfunction:: fiction::sidb::simulation::defects::defect_influence_grid_search(const Lyt& lyt, const defect_influence_params<cell<Lyt>>& params = {}, const std::size_t step_size = 1, defect_influence_stats* stats = nullptr)
.. doxygenfunction:: fiction::sidb::simulation::defects::defect_influence_random_sampling(const Lyt& lyt, const std::vector<TT>& spec, std::size_t samples, const defect_influence_params<cell<Lyt>>& params = {}, defect_influence_stats*                   stats  = nullptr)
.. doxygenfunction:: fiction::sidb::simulation::defects::defect_influence_random_sampling(const Lyt& lyt, std::size_t samples, const defect_influence_params<cell<Lyt>>& params = {}, defect_influence_stats*                   stats  = nullptr)
.. doxygenfunction:: fiction::sidb::simulation::defects::defect_influence_quicktrace(const Lyt& lyt, const std::vector<TT>& spec, const std::size_t samples, const defect_influence_params<cell<Lyt>>& params = {}, defect_influence_stats*                   stats  = nullptr)
.. doxygenfunction:: fiction::sidb::simulation::defects::defect_influence_quicktrace(const Lyt& lyt, const std::size_t samples, const defect_influence_params<cell<Lyt>>& params = {}, defect_influence_stats*                   stats  = nullptr)

**Header:** ``fiction/technology/sidb/simulation/defects/defect_clearance.hpp``

.. doxygenstruct:: fiction::sidb::simulation::defects::defect_clearance
   :members:
.. doxygenfunction:: fiction::sidb::simulation::defects::calculate_defect_clearance
