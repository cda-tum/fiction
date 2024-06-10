//
// Created by marcel on 20.01.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/determine_clocking.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <bill/sat/interface/common.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/utils/stopwatch.hpp>

using namespace fiction;

template <typename Lyt>
void remove_clocking(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    lyt.foreach_tile([&lyt](const auto& t) { lyt.assign_clock_number(t, 0); });
}

template <typename Lyt>
void remove_assign_and_check_clocking(Lyt lyt)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    const auto lyt_clone = lyt.clone();

    // for each supported solver
    for (const auto& solver : {
             bill::solvers::ghack,
             bill::solvers::glucose_41,
             bill::solvers::bsat2,
#if !defined(BILL_WINDOWS_PLATFORM)
             bill::solvers::maple,
             bill::solvers::bmcg,
#endif
         })
    {
        remove_clocking(lyt);

        determine_clocking_stats st{};

        const auto result = determine_clocking(lyt, determine_clocking_params{solver}, &st);

        REQUIRE(result == true);
        CHECK(mockturtle::to_seconds(st.time_total) > 0);

        check_eq(lyt, lyt_clone);
    }
}

TEST_CASE("Determine clock numbers for an empty layout", "[determine-clocking]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    gate_layout layout{{5, 5}};

    CHECK(determine_clocking(layout) == true);
}

TEST_CASE("Determine clock numbers for simple layouts", "[determine-clocking]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    remove_assign_and_check_clocking(blueprints::straight_wire_gate_layout<gate_layout>());
    remove_assign_and_check_clocking(blueprints::and_not_gate_layout<gate_layout>());
    remove_assign_and_check_clocking(blueprints::or_not_gate_layout<gate_layout>());
    remove_assign_and_check_clocking(blueprints::use_and_gate_layout<gate_layout>());
    remove_assign_and_check_clocking(blueprints::res_maj_gate_layout<gate_layout>());
    remove_assign_and_check_clocking(blueprints::crossing_layout<gate_layout>());
    remove_assign_and_check_clocking(blueprints::fanout_layout<gate_layout>());
    remove_assign_and_check_clocking(blueprints::unbalanced_and_layout<gate_layout>());
}

TEST_CASE("Determine clock numbers for complex layouts", "[determine-clocking]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    remove_assign_and_check_clocking(orthogonal<gate_layout>(blueprints::maj1_network<mockturtle::aig_network>()));
    remove_assign_and_check_clocking(orthogonal<gate_layout>(blueprints::maj4_network<mockturtle::aig_network>()));
    remove_assign_and_check_clocking(
        orthogonal<gate_layout>(blueprints::nary_operation_network<mockturtle::aig_network>()));
}

TEST_CASE("Determine clock numbers for non-Cartesian layout topologies", "[determine-clocking]")
{
    SECTION("shifted Cartesian")
    {
        SECTION("odd column")
        {
            using gate_layout = gate_level_layout<
                clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_column_cartesian>>>>;

            remove_assign_and_check_clocking(blueprints::shifted_cart_and_or_inv_gate_layout<gate_layout>());
        }
        SECTION("even row")
        {
            using gate_layout = gate_level_layout<
                clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, even_row_cartesian>>>>;

            remove_assign_and_check_clocking(blueprints::row_clocked_and_xor_gate_layout<gate_layout>());
        }
    }
    SECTION("hexagonal")
    {
        SECTION("even row")
        {
            using gate_layout =
                gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>;

            remove_assign_and_check_clocking(blueprints::row_clocked_and_xor_gate_layout<gate_layout>());
        }
    }
}

TEST_CASE("Determine clock numbers for a 3-phase layout", "[determine-clocking]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    remove_assign_and_check_clocking(
        orthogonal<gate_layout>(blueprints::maj1_network<mockturtle::aig_network>(), {num_clks::THREE}));
}

TEST_CASE("Determine clock numbers for a non-clockable layout", "[determine-clocking]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto lyt = blueprints::unclockable_gate_layout<gate_layout>();

    CHECK(determine_clocking(lyt) == false);
}
