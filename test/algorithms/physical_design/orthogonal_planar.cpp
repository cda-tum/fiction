#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/algorithms/physical_design/orthogonal_planar.hpp>
#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/technology/qca_one_library.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/views/names_view.hpp>

using namespace fiction;

void check_stats(const orthogonal_physical_design_stats& st) noexcept
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

template <typename Ntk>
void check_ortho_planar(const Ntk& ntk)
{
    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    fiction::network_balancing_params b_ps;
    b_ps.unify_outputs = true;

    const auto tec_balanced = fiction::network_balancing<Ntk>(fiction::fanout_substitution<Ntk>(ntk), b_ps);

    auto tec_ranked   = fiction::mutable_rank_view(tec_balanced);
    auto planarized_b = fiction::node_duplication_planarization(tec_ranked);

    mockturtle::equivalence_checking_stats eq_st;
    const auto                             cec_m = mockturtle::equivalence_checking(
        *fiction::virtual_miter<fiction::technology_network>(ntk, planarized_b), {}, &eq_st);
    REQUIRE(cec_m.has_value());
    CHECK(*cec_m == 1);

    orthogonal_physical_design_stats orthogonal_planar_stats{};

    const auto gate_level_layout = fiction::orthogonal_planar<gate_lyt>(planarized_b, {}, &orthogonal_planar_stats);

    CHECK(gate_level_layout.num_crossings() == 0);
    check_stats(orthogonal_planar_stats);
    CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(gate_level_layout));
}

TEST_CASE("Orthogonal planar layout tests", "[orthogonal-planar]")
{
    // Simple correctness and corner-case networks
    check_ortho_planar(blueprints::se_coloring_corner_case_network<technology_network>());
    check_ortho_planar(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_ortho_planar(blueprints::clpl<technology_network>());

    // Network with constant inputs
    check_ortho_planar(blueprints::unbalanced_and_inv_network<technology_network>());

    // Multi-output network
    check_ortho_planar(blueprints::multi_output_network<technology_network>());
}

TEST_CASE("Name conservation after planar orthogonal physical design", "[orthogonal-planar]")
{
    using gate_lyt = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto topolinano = blueprints::topolinano_network<mockturtle::names_view<technology_network>>();
    topolinano.set_network_name("topolinano");

    fiction::network_balancing_params b_ps;
    b_ps.unify_outputs = true;

    const auto topolinano_balanced = fiction::network_balancing<mockturtle::names_view<technology_network>>(
        fiction::fanout_substitution<mockturtle::names_view<technology_network>>(topolinano), b_ps);

    auto topolinano_ranked = fiction::mutable_rank_view(topolinano_balanced);
    auto planarized_b      = fiction::node_duplication_planarization(topolinano_ranked);

    CHECK(planarized_b.get_name(2) == "x1");
    CHECK(planarized_b.get_name(3) == "x2");
    CHECK(planarized_b.get_network_name() == "topolinano");

    orthogonal_physical_design_stats orthogonal_planar_stats{};

    const auto layout = fiction::orthogonal_planar<gate_lyt>(planarized_b, {}, &orthogonal_planar_stats);

    // network name
    CHECK(layout.get_layout_name() == "topolinano");
    // PI names (they are ordered corresponding to their placement in the layout)
    CHECK(layout.get_input_name(1) == "x1");
    CHECK(layout.get_input_name(0) == "x2");
    // PO names
    CHECK(layout.get_output_name(0) == "f1");
    CHECK(layout.get_output_name(1) == "f2");
    CHECK(layout.get_output_name(2) == "f3");
}
