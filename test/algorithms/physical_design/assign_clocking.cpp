//
// Created by marcel on 20.01.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/assign_clocking.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <type_traits>

using namespace fiction;

template <typename Lyt>
void remove_clocking(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    lyt.foreach_tile([&lyt](const auto& t) { lyt.assign_clock_number(t, 0); });
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

    auto lyt1 = blueprints::straight_wire_gate_layout<gate_layout>();
    remove_clocking(lyt1);

    const auto result1 = assign_clocking(lyt1);

    REQUIRE(result1 == true);

    check_eq(blueprints::straight_wire_gate_layout<gate_layout>(), lyt1);
}
