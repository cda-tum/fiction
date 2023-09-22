//
// Created by marcel on 02.06.22.
//

#include "pyfiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "pyfiction/algorithms/network_transformation/network_balancing.hpp"
#include "pyfiction/algorithms/path_finding/a_star.hpp"
#include "pyfiction/algorithms/path_finding/distance.hpp"
#include "pyfiction/algorithms/path_finding/enumerate_all_paths.hpp"
#include "pyfiction/algorithms/path_finding/k_shortest_paths.hpp"
#include "pyfiction/algorithms/physical_design/apply_gate_library.hpp"
#include "pyfiction/algorithms/physical_design/color_routing.hpp"
#include "pyfiction/algorithms/physical_design/exact.hpp"
#include "pyfiction/algorithms/physical_design/hexagonalization.hpp"
#include "pyfiction/algorithms/physical_design/orthogonal.hpp"
#include "pyfiction/algorithms/properties/critical_path_length_and_throughput.hpp"
#include "pyfiction/algorithms/simulation/logic_simulation.hpp"
#include "pyfiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp"
#include "pyfiction/algorithms/simulation/sidb/sidb_charge_state.hpp"
// #include "pyfiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "pyfiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "pyfiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "pyfiction/algorithms/simulation/sidb/is_ground_state.hpp"
#include "pyfiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "pyfiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp"
#include "pyfiction/algorithms/simulation/sidb/quicksim.hpp"
#include "pyfiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "pyfiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "pyfiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
// #include "pyfiction/algorithms/simulation/sidb/time_to_solution.hpp"
#include "pyfiction/algorithms/verification/design_rule_violations.hpp"
#include "pyfiction/algorithms/verification/equivalence_checking.hpp"
#include "pyfiction/io/read_fqca_layout.hpp"
#include "pyfiction/io/read_sqd_layout.hpp"
#include "pyfiction/io/write_dot_layout.hpp"
#include "pyfiction/io/write_fqca_layout.hpp"
#include "pyfiction/io/write_qca_layout.hpp"
#include "pyfiction/io/write_qcc_layout.hpp"
#include "pyfiction/io/write_qll_layout.hpp"
#include "pyfiction/io/write_sqd_layout.hpp"
#include "pyfiction/io/write_svg_layout.hpp"
#include "pyfiction/layouts/cartesian_layout.hpp"
#include "pyfiction/layouts/cell_level_layout.hpp"
#include "pyfiction/layouts/clocked_layout.hpp"
#include "pyfiction/layouts/coordinates.hpp"
#include "pyfiction/layouts/gate_level_layout.hpp"
#include "pyfiction/layouts/hexagonal_layout.hpp"
#include "pyfiction/layouts/obstruction_layout.hpp"
#include "pyfiction/layouts/shifted_cartesian_layout.hpp"
#include "pyfiction/networks/logic_network.hpp"
#include "pyfiction/technology/area.hpp"
#include "pyfiction/technology/charge_distribution_surface.hpp"
#include "pyfiction/technology/sidb_defects.hpp"
#include "pyfiction/utils/routing_utils.hpp"

#include <pybind11/pybind11.h>

PYBIND11_MODULE(pyfiction, m)
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
     * Networks
     */
    pyfiction::logic_network(m);
    /**
     * Algorithms: Physical Design
     */
    pyfiction::exact(m);
    pyfiction::orthogonal(m);
    pyfiction::apply_gate_library(m);
    pyfiction::color_routing(m);
    pyfiction::hexagonalization(m);
    /**
     * Algorithms: Network Transformation
     */
    pyfiction::fanout_substitution(m);
    pyfiction::network_balancing(m);
    /**
     * Algorithms: Path Finding
     */
    pyfiction::distance(m);
    pyfiction::a_star(m);
    pyfiction::yen_k_shortest_paths(m);
    pyfiction::enumerate_all_paths(m);
    /**
     * Algorithms: Properties
     */
    pyfiction::critical_path_length_and_throughput(m);
    /**
     * Algorithms: Simulation
     */
    pyfiction::logic_simulation(m);
    pyfiction::sidb_simulation_engine(m);
    pyfiction::sidb_simulation_parameters(m);
    pyfiction::sidb_simulation_result(m);
    pyfiction::exhaustive_ground_state_simulation(m);
    pyfiction::quicksim(m);
    pyfiction::is_ground_state(m);
    pyfiction::minimum_energy(m);
    pyfiction::energy_distribution(m);
    pyfiction::calculate_energy_and_state_type(m);
    pyfiction::occupation_probability_of_excited_states(m);
    //    pyfiction::critical_temperature(m);
    //    pyfiction::time_to_solution(m);
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
    pyfiction::charge_distribution_surface(m);
    /**
     * Input/Output
     */
    pyfiction::write_dot_layout(m);
    pyfiction::write_qca_layout(m);
    pyfiction::write_svg_layout(m);
    pyfiction::write_sqd_layout(m);
    pyfiction::write_qcc_layout(m);
    pyfiction::write_qll_layout(m);
    pyfiction::write_fqca_layout(m);
    pyfiction::read_fqca_layout(m);
    pyfiction::read_sqd_layout(m);
    /**
     * Utils
     */
    pyfiction::routing_utils(m);
}
