//
// Created by marcel on 02.06.22.
//

#define PYBIND11_DETAILED_ERROR_MESSAGES

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include "pyfiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "pyfiction/algorithms/simulation/sidb/time_to_solution.hpp"
#include "pyfiction/inout/read_fgl_layout.hpp"
#include "pyfiction/inout/read_fqca_layout.hpp"
#include "pyfiction/inout/read_sqd_layout.hpp"
#include "pyfiction/inout/write_dot_layout.hpp"
#include "pyfiction/inout/write_fgl_layout.hpp"
#include "pyfiction/inout/write_fqca_layout.hpp"
// #include "pyfiction/inout/write_location_and_ground_state.hpp"
#include "pyfiction/inout/write_operational_domain.hpp"
#include "pyfiction/inout/write_qca_layout.hpp"
#include "pyfiction/inout/write_qcc_layout.hpp"
#include "pyfiction/inout/write_qll_layout.hpp"
#include "pyfiction/inout/write_sqd_layout.hpp"
#include "pyfiction/inout/write_sqd_sim_result.hpp"
#include "pyfiction/inout/write_svg_layout.hpp"
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

namespace pyfiction
{
void register_algorithms(pybind11::module& m);
void sidb_simulation_parameters(pybind11::module& m);
}  // namespace pyfiction

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
    pyfiction::sidb_simulation_parameters(m);
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
    /**
     * Algorithms
     */
    pyfiction::register_algorithms(m);
    /**
     * Algorithms: Simulation (second part here to avoid circular dependency with `sidb_simulation_parameters` which is
     * used in `random_sidb_layout_generator`).
     */
    pyfiction::random_sidb_layout_generator(m);
    pyfiction::time_to_solution(m);
}

#pragma GCC diagnostic pop
