//
// Created by benjamin on 7/21/25.
//

#include "fiction/technology/molecular_qca_library.hpp"

#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/algorithms/physical_design/graph_oriented_layout_design.hpp"
#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "fiction/io/write_qll_layout.hpp"
#include "fiction/io/write_svg_layout.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/technology/molqca.hpp"
#include "fiction/technology/qca_one_library.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/physical_design/exact.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <fmt/format.h>

#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Test molQCA", "[molqca]")
{
    /*fiction::technology_network tec{};
    const auto pi1 = tec.create_pi();
    const auto not1 = tec.create_not(pi1);
    const auto buf = tec.create_buf(not1);
    const auto pi2 = tec.create_pi();
    const auto n_and = tec.create_or(pi2, buf);
    tec.create_po(n_and);*/

    /*fiction::technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    // build XOR = (x1 OR x2) AND NOT(x1 AND x2)
    // 1) a ∨ b
    const auto a_or_b    = tec.create_or(x1, x2);
    // 2) a ∧ b
    const auto carry   = tec.create_and(x1, x2);
    // 3) ¬(a ∧ b)
    const auto not_and   = tec.create_not(carry);
    // 4) (a ∨ b) ∧ ¬(a ∧ b)
    const auto sum       = tec.create_and(a_or_b, not_and);

    // outputs
    tec.create_po(sum);
    tec.create_po(carry);*/

    const auto tec = blueprints::mux21_network<fiction::technology_network>();

    /*fiction::technology_network tec{};
    const auto pi1 = tec.create_pi();
    const auto buf = tec.create_buf(pi1);
    tec.create_po(buf);*/

    orthogonal_physical_design_stats stats{};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    // auto gate_lyt = orthogonal<gate_layout>(tec, {}, &stats);

    /*fiction::graph_oriented_layout_design_stats  graph_oriented_layout_design_stats{};
    fiction::graph_oriented_layout_design_params graph_oriented_layout_design_params{};

    auto gate_layout_opt = fiction::graph_oriented_layout_design<gate_layout, technology_network>(
        tec, graph_oriented_layout_design_params, &graph_oriented_layout_design_stats);

    if (!gate_layout_opt) {
        throw std::runtime_error("layout design failed");
    }

    const auto& gate_lyt = *gate_layout_opt;*/

    exact_physical_design_params ps_exact{};
    ps_exact.straight_inverters = true;
    ps_exact.scheme = "2DDWave";
    ps_exact.border_io = false;
    //ps_exact.crossings;
    const auto gate_layout_opt = exact<gate_layout>(tec, ps_exact);

    std::cout << "Exact finished\n";

    if (!gate_layout_opt) {
        throw std::runtime_error("layout design failed");
    }

    std::cout << "Start applying library\n";

    const auto& gate_lyt = *gate_layout_opt;

    using qca_cell_clk_lyt =
        cell_level_layout<qca_technology,
                          synchronization_element_layout  // se_layouts have only been investigated for QCA technologies
                          <clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    const auto layout = apply_gate_library<qca_cell_clk_lyt, qca_one_library>(gate_lyt);

    std::string filename_qca = "lyt.svg";

    fiction::write_qca_layout_svg(layout, filename_qca);

    fiction::write_qll_layout(layout, "lyt.qll");

    using mol_qca_cell_clk_lyt =
        cell_level_layout<mol_qca_technology,
                          synchronization_element_layout  // se_layouts have only been investigated for QCA technologies
                          <clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    // apply here the molecular_qca_library
    const auto layout_mol_qca = apply_gate_library<mol_qca_cell_clk_lyt, molecular_qca_library>(gate_lyt);

    std::string filename_mol = "lyt_mol_qca.svg";

    fiction::write_mol_qca_layout_svg(layout_mol_qca, filename_mol);

    fiction::write_qll_layout(layout_mol_qca, "lyt_mol_qca.qll");
}