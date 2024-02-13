//
// Created by marcel on 20.01.24.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/layouts/coordinates.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/assign_clocking.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <bill/sat/interface/common.hpp>

#include <type_traits>

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
             bill::solvers::ghack, bill::solvers::glucose_41, bill::solvers::bsat2,
#if !defined(BILL_WINDOWS_PLATFORM)
                 bill::solvers::maple, bill::solvers::bmcg,
#endif
         })
    {
        remove_clocking(lyt);

        const auto result = assign_clocking(lyt, assign_clocking_params{solver});

        REQUIRE(result == true);

        check_eq(lyt, lyt_clone);
    }
}

TEST_CASE("Assign clock numbers to an empty layout", "[assign-clocking]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    gate_layout layout{{5, 5}};

    CHECK(assign_clocking(layout) == true);
}

TEST_CASE("Assign clock numbers to simple layouts", "[assign-clocking]")
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

TEST_CASE("Assign clock numbers to complex layouts", "[assign-clocking]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    remove_assign_and_check_clocking(orthogonal<gate_layout>(blueprints::maj1_network<mockturtle::aig_network>()));
    remove_assign_and_check_clocking(orthogonal<gate_layout>(blueprints::maj4_network<mockturtle::aig_network>()));
    remove_assign_and_check_clocking(
        orthogonal<gate_layout>(blueprints::nary_operation_network<mockturtle::aig_network>()));
}
