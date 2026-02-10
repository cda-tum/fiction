#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/physical_design/planar_layout_from_network_embedding.hpp>
#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/qca_one_library.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/views/names_view.hpp>

#include <utils/blueprints/network_blueprints.hpp>

using namespace fiction;

static void check_stats(const planar_layout_from_network_embedding_stats& st) noexcept
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

template <typename Ntk>
static void check_plane(const Ntk& ntk)
{
    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    fiction::network_balancing_params b_ps;
    b_ps.unify_outputs = true;

    const auto tec_balanced = fiction::network_balancing<Ntk>(fiction::fanout_substitution<Ntk>(ntk), b_ps);

    auto tec_ranked   = fiction::mutable_rank_view(tec_balanced);
    auto planarized_b = fiction::node_duplication_planarization(tec_ranked);

    // clang-tidy bugprone-unchecked-optional-access false positive in tests
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    mockturtle::equivalence_checking_stats eq_st;
    const auto                             cec_m = mockturtle::equivalence_checking(
        *fiction::virtual_miter<fiction::technology_network>(ntk, planarized_b), {}, &eq_st);
    REQUIRE(cec_m.has_value());
    const auto result = *cec_m;
    CHECK(result == 1);
    // NOLINTEND(bugprone-unchecked-optional-access)

    planar_layout_from_network_embedding_stats planar_layout_from_network_embedding_stats{};

    const auto gate_level_layout =
        fiction::plane<gate_lyt>(planarized_b, {}, &planar_layout_from_network_embedding_stats);

    CHECK(gate_level_layout.num_crossings() == 0);
    check_stats(planar_layout_from_network_embedding_stats);
    CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(gate_level_layout));
}

TEST_CASE("Check exceptions", "[planar-layout-from-network-embedding]")
{
    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;
    fiction::network_balancing_params b_ps;
    b_ps.unify_outputs = true;

    auto                                       maj        = blueprints::maj1_network<technology_network>();
    auto                                       maj_ranked = fiction::mutable_rank_view(maj);
    planar_layout_from_network_embedding_stats planar_layout_from_network_embedding_stats{};
    CHECK_THROWS_WITH(fiction::plane<gate_lyt>(maj_ranked, {}, &planar_layout_from_network_embedding_stats),
                      "network contains nodes that exceed the supported fanin size");

    auto ao        = blueprints::and_or_network<technology_network>();
    auto ao_ranked = fiction::mutable_rank_view(ao);
    CHECK_THROWS_WITH(fiction::plane<gate_lyt>(ao_ranked, {}, &planar_layout_from_network_embedding_stats),
                      "Input network has to be planar");
}

TEST_CASE("Planar layout tests", "[planar-layout-from-network-embedding]")
{
    // Simple correctness and corner-case networks
    check_plane(blueprints::se_coloring_corner_case_network<technology_network>());
    check_plane(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_plane(blueprints::clpl<technology_network>());

    // Network with constant inputs
    check_plane(blueprints::unbalanced_and_inv_network<technology_network>());

    // Multi-output network
    check_plane(blueprints::multi_output_network<technology_network>());

    // Parity network
    check_plane(blueprints::parity_network<technology_network>());
}

TEST_CASE("Name conservation after planar physical design", "[planar-layout-from-network-embedding]")
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

    CHECK(planarized_b.get_network_name() == "topolinano");
    CHECK(planarized_b.get_output_name(0) == "f1");
    CHECK(planarized_b.get_output_name(1) == "f2");
    CHECK(planarized_b.get_output_name(2) == "f3");

    planar_layout_from_network_embedding_stats  planar_layout_from_network_embedding_stats{};
    planar_layout_from_network_embedding_params ps{};
    ps.verbose = true;

    const auto layout = fiction::plane<gate_lyt>(planarized_b, ps, &planar_layout_from_network_embedding_stats);

    // network name
    CHECK(layout.get_layout_name() == "topolinano");
    // PO names
    CHECK(layout.get_output_name(0) == "f1");
    CHECK(layout.get_output_name(1) == "f2");
    CHECK(layout.get_output_name(2) == "f3");
}
