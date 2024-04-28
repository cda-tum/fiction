//
// Created by marcel on 02.06.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/technology/qca_one_library.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <type_traits>

using namespace fiction;

TEST_CASE("East-south coloring", "[orthogonal]")
{
    const auto check = [](const auto& ntk)
    {
        auto container = detail::east_south_edge_coloring(ntk);
        CHECK(detail::is_east_south_colored(container.color_ntk));
    };

    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::maj1_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::maj4_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::se_coloring_corner_case_network<technology_network>())});
    check(mockturtle::fanout_view{fanout_substitution<technology_network>(
        blueprints::fanout_substitution_corner_case_network<technology_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::nary_operation_network<technology_network>())});
    check(mockturtle::fanout_view{fanout_substitution<technology_network>(blueprints::clpl<technology_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::half_adder_network<mockturtle::mig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::full_adder_network<mockturtle::mig_network>())});
}

void check_stats(const orthogonal_physical_design_stats& st) noexcept
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

template <typename Lyt, typename Ntk>
void check_ortho_equiv(const Ntk& ntk)
{
    orthogonal_physical_design_stats stats{};

    auto layout = orthogonal<Lyt>(ntk, {}, &stats);

    check_stats(stats);
    check_eq(ntk, layout);
}

template <typename Lyt>
void check_ortho_equiv_all()
{
    check_ortho_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_ortho_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    check_ortho_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    check_ortho_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    check_ortho_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_ortho_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    check_ortho_equiv<Lyt>(blueprints::clpl<technology_network>());

    // constant input network
    check_ortho_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::mig_network>());
}

TEST_CASE("Layout equivalence", "[algorithms]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        check_ortho_equiv_all<gate_layout>();
    }
    SECTION("Hexagonal layouts")
    {
        SECTION("odd row")
        {
            using gate_layout =
                gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>;

            check_ortho_equiv_all<gate_layout>();
        }
        SECTION("even row")
        {
            using gate_layout =
                gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>;

            check_ortho_equiv_all<gate_layout>();
        }
        SECTION("odd column")
        {
            using gate_layout = gate_level_layout<
                clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>>;

            check_ortho_equiv_all<gate_layout>();
        }
        SECTION("even column")
        {
            using gate_layout = gate_level_layout<
                clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>>;

            check_ortho_equiv_all<gate_layout>();
        }
    }
}

TEST_CASE("Gate library application", "[orthogonal]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    const auto check = [](const auto& ntk)
    {
        orthogonal_physical_design_stats stats{};

        auto layout = orthogonal<gate_layout>(ntk, {}, &stats);

        CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(layout));
    };

    check(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check(blueprints::maj1_network<mockturtle::aig_network>());
    check(blueprints::maj4_network<mockturtle::aig_network>());
    check(blueprints::se_coloring_corner_case_network<technology_network>());
    check(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check(blueprints::clpl<technology_network>());
    check(blueprints::half_adder_network<mockturtle::mig_network>());

    // constant input network
    check(blueprints::unbalanced_and_inv_network<mockturtle::mig_network>());
}

TEST_CASE("Name conservation after orthogonal physical design", "[orthogonal]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
    maj.set_network_name("maj");

    const auto layout = orthogonal<gate_layout>(maj);

    // network name
    CHECK(layout.get_layout_name() == "maj");

    // PI names
    CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}
