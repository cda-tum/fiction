//
// Created by marcel on 02.06.22.
//

#define PYBIND11_DETAILED_ERROR_MESSAGES

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include "pyfiction/algorithms/iter/bdl_input_iterator.hpp"
#include "pyfiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "pyfiction/algorithms/network_transformation/network_balancing.hpp"
#include "pyfiction/algorithms/network_transformation/technology_mapping.hpp"
#include "pyfiction/algorithms/path_finding/a_star.hpp"
#include "pyfiction/algorithms/path_finding/distance.hpp"
#include "pyfiction/algorithms/path_finding/enumerate_all_paths.hpp"
#include "pyfiction/algorithms/path_finding/k_shortest_paths.hpp"
#include "pyfiction/algorithms/physical_design/apply_gate_library.hpp"
#include "pyfiction/algorithms/physical_design/color_routing.hpp"
#include "pyfiction/algorithms/physical_design/design_sidb_gates.hpp"
#include "pyfiction/algorithms/physical_design/exact.hpp"
#include "pyfiction/algorithms/physical_design/graph_oriented_layout_design.hpp"
#include "pyfiction/algorithms/physical_design/hexagonalization.hpp"
#include "pyfiction/algorithms/physical_design/orthogonal.hpp"
#include "pyfiction/algorithms/physical_design/post_layout_optimization.hpp"
#include "pyfiction/algorithms/physical_design/wiring_reduction.hpp"
#include "pyfiction/algorithms/properties/critical_path_length_and_throughput.hpp"
#include "pyfiction/algorithms/simulation/logic_simulation.hpp"
#include "pyfiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp"
#include "pyfiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "pyfiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "pyfiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "pyfiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "pyfiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "pyfiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "pyfiction/algorithms/simulation/sidb/is_ground_state.hpp"
#include "pyfiction/algorithms/simulation/sidb/is_operational.hpp"
#include "pyfiction/algorithms/simulation/sidb/operational_domain_ratio.hpp"
#include "pyfiction/algorithms/simulation/sidb/physically_valid_parameters.hpp"
// #include "pyfiction/algorithms/simulation/sidb/defect_influence.hpp"
#include "pyfiction/algorithms/simulation/sidb/check_simulation_result_for_equivalence.hpp"
#include "pyfiction/algorithms/simulation/sidb/clustercomplete.hpp"
#include "pyfiction/algorithms/simulation/sidb/displacement_robustness_domain.hpp"
#include "pyfiction/algorithms/simulation/sidb/groundstate_from_simulation_result.hpp"
#include "pyfiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "pyfiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp"
#include "pyfiction/algorithms/simulation/sidb/operational_domain.hpp"
#include "pyfiction/algorithms/simulation/sidb/physical_population_stability.hpp"
#include "pyfiction/algorithms/simulation/sidb/potential_to_distance_conversion.hpp"
#include "pyfiction/algorithms/simulation/sidb/quickexact.hpp"
#include "pyfiction/algorithms/simulation/sidb/quicksim.hpp"
#include "pyfiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "pyfiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "pyfiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "pyfiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "pyfiction/algorithms/simulation/sidb/time_to_solution.hpp"
#include "pyfiction/algorithms/verification/design_rule_violations.hpp"
#include "pyfiction/algorithms/verification/equivalence_checking.hpp"
#include "pyfiction/inout/read_fgl_layout.hpp"
#include "pyfiction/inout/read_fqca_layout.hpp"
#include "pyfiction/inout/read_sqd_layout.hpp"
#include "pyfiction/inout/write_dot_layout.hpp"
#include "pyfiction/inout/write_fgl_layout.hpp"
#include "pyfiction/inout/write_fqca_layout.hpp"
#include "pyfiction/inout/write_svg_layout.hpp"
// #include "pyfiction/inout/write_location_and_ground_state.hpp"
#include "pyfiction/inout/write_operational_domain.hpp"
#include "pyfiction/inout/write_qca_layout.hpp"
#include "pyfiction/inout/write_qcc_layout.hpp"
#include "pyfiction/inout/write_qll_layout.hpp"
#include "pyfiction/inout/write_sqd_layout.hpp"
#include "pyfiction/inout/write_sqd_sim_result.hpp"
#include "pyfiction/layouts/cartesian_layout.hpp"
#include "pyfiction/layouts/cell_level_layout.hpp"
#include "pyfiction/layouts/clocked_layout.hpp"
#include "pyfiction/layouts/coordinates.hpp"
#include "pyfiction/layouts/gate_level_layout.hpp"
#include "pyfiction/layouts/hexagonal_layout.hpp"
#include "pyfiction/layouts/obstruction_layout.hpp"
#include "pyfiction/layouts/shifted_cartesian_layout.hpp"
#include "pyfiction/networks/logic_networks.hpp"
#include "pyfiction/networks/truth_tables.hpp"
#include "pyfiction/technology/area.hpp"
#include "pyfiction/technology/charge_distribution_surface.hpp"
#include "pyfiction/technology/sidb_charge_state.hpp"
#include "pyfiction/technology/sidb_defects.hpp"
#include "pyfiction/technology/sidb_lattice.hpp"
#include "pyfiction/technology/sidb_nm_distance.hpp"
#include "pyfiction/technology/sidb_nm_position.hpp"
#include "pyfiction/utils/layout_utils.hpp"
#include "pyfiction/utils/name_utils.hpp"
#include "pyfiction/utils/network_utils.hpp"
#include "pyfiction/utils/placement_utils.hpp"
#include "pyfiction/utils/routing_utils.hpp"
#include "pyfiction/utils/truth_table_utils.hpp"
#include "pyfiction/utils/version_info.hpp"

#include <pybind11/pybind11.h>

PYBIND11_MODULE(pyfiction, m, pybind11::mod_gil_not_used())
{
    // docstring
    m.doc() = "Python bindings for fiction, a framework for Design Automation for Field-coupled Nanotechnologies";

    /**
     * Coordinates
     */
    pyfiction::offset_coordinate(m);
    pyfiction::cube_coordinate(m);
    pyfiction::siqad_coordinate(m);
    pyfiction::coordinate_utility(m);
    /**
     * Logic
     */
    pyfiction::logic_networks(m);
    pyfiction::truth_tables(m);
    /**
     * Layouts
     */
    pyfiction::cartesian_layout(m);
    pyfiction::shifted_cartesian_layout(m);
    pyfiction::hexagonal_layout(m);
    pyfiction::clocked_layouts(m);
    pyfiction::gate_level_layouts(m);
    pyfiction::cell_level_layouts(m);
    pyfiction::obstruction_layouts(m);
    /**
     * Algorithms: Simulation
     */
    pyfiction::logic_simulation(m);
    pyfiction::sidb_simulation_engine(m);
    pyfiction::sidb_simulation_parameters(m);
    pyfiction::sidb_simulation_result(m);
    pyfiction::can_positive_charges_occur(m);
    pyfiction::physical_population_stability(m);
    pyfiction::potential_to_distance_conversion(m);
    pyfiction::detect_bdl_pairs(m);
    pyfiction::detect_bdl_wires(m);
    pyfiction::is_operational(m);
    pyfiction::operational_domain(m);
    pyfiction::exhaustive_ground_state_simulation(m);
    pyfiction::quicksim(m);
    pyfiction::quickexact(m);
    pyfiction::clustercomplete(m);
    pyfiction::is_ground_state(m);
    pyfiction::minimum_energy(m);
    pyfiction::energy_distribution(m);
    pyfiction::calculate_energy_and_state_type(m);
    pyfiction::occupation_probability_of_excited_states(m);
    pyfiction::critical_temperature(m);
    pyfiction::random_sidb_layout_generator(m);
    pyfiction::time_to_solution(m);
    pyfiction::groundstate_from_simulation_result(m);
    pyfiction::check_simulation_results_for_equivalence(m);
    pyfiction::physically_valid_parameters(m);
    pyfiction::determine_displacement_robustness_domain(m);
    pyfiction::compute_operational_ratio(m);
    /**
     * Algorithms: Iterators
     */
    pyfiction::bdl_input_iterator(m);
    /**
     * Algorithms: Network Transformation
     */
    pyfiction::fanout_substitution(m);
    pyfiction::network_balancing(m);
    pyfiction::technology_mapping(m);
    /**
     * Algorithms: Path Finding
     */
    pyfiction::distance(m);
    pyfiction::a_star(m);
    pyfiction::yen_k_shortest_paths(m);
    pyfiction::enumerate_all_paths(m);
    /**
     * Algorithms: Physical Design
     */
    pyfiction::exact(m);
    pyfiction::orthogonal(m);
    pyfiction::graph_oriented_layout_design(m);
    pyfiction::apply_gate_library(m);
    pyfiction::color_routing(m);
    pyfiction::hexagonalization(m);
    pyfiction::post_layout_optimization(m);
    pyfiction::wiring_reduction(m);
    pyfiction::design_sidb_gates(m);
    /**
     * Algorithms: Properties
     */
    pyfiction::critical_path_length_and_throughput(m);
    /*
     * Algorithms: Verification
     */
    pyfiction::equivalence_checking(m);
    pyfiction::design_rule_violations(m);
    /**
     * Technology
     */
    pyfiction::area(m);
    pyfiction::sidb_defects(m);
    pyfiction::sidb_charge_state(m);
    pyfiction::sidb_nm_position(m);
    pyfiction::sidb_nm_distance(m);
    pyfiction::sidb_lattices(m);
    pyfiction::charge_distribution_surfaces(m);
    /**
     * Input/Output
     */
    pyfiction::write_dot_layout(m);
    pyfiction::write_fgl_layout(m);
    pyfiction::write_qca_layout(m);
    pyfiction::write_svg_layout(m);
    pyfiction::write_sqd_layout(m);
    pyfiction::write_qcc_layout(m);
    pyfiction::write_qll_layout(m);
    pyfiction::write_fqca_layout(m);
    pyfiction::write_sqd_sim_result(m);
    // pyfiction::write_location_and_ground_state(m);
    pyfiction::write_operational_domain(m);
    pyfiction::read_fgl_layout(m);
    pyfiction::read_fqca_layout(m);
    pyfiction::read_sqd_layout(m);
    /**
     * Utils
     */
    pyfiction::layout_utils(m);
    pyfiction::routing_utils(m);
    pyfiction::name_utils(m);
    pyfiction::network_utils(m);
    pyfiction::placement_utils(m);
    pyfiction::truth_table_utils(m);
    pyfiction::version_info(m);
}

#pragma GCC diagnostic pop
