//
// Created by marcel on 18.06.23.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/path_finding/distance_map.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>

using namespace fiction;

TEST_CASE("Distance map", "[distance-map]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;

    SECTION("2DDWave clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{2, 2}, twoddwave_clocking<clk_lyt>()};

        const auto dist_map = initialize_distance_map(layout, twoddwave_distance_functor<clk_lyt>{});

        CHECK(manhattan_distance<clk_lyt>(layout, {0, 0}, {0, 0}) == 0);
        CHECK(manhattan_distance<clk_lyt>(layout, {1, 1}, {1, 1}) == 0);
        CHECK(manhattan_distance<clk_lyt>(layout, {0, 0}, {0, 1}) == 1);
        CHECK(manhattan_distance<clk_lyt>(layout, {0, 0}, {1, 1}) == 2);
        CHECK(manhattan_distance<clk_lyt>(layout, {1, 2}, {3, 3}) == 3);
        CHECK(manhattan_distance<clk_lyt>(layout, {0, 0}, {4, 4}) == 8);
        CHECK(manhattan_distance<clk_lyt>(layout, {4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(manhattan_distance<clk_lyt>(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(manhattan_distance<clk_lyt>(layout, {0, 0, 1}, {8, 9, 1}) == 17);
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        const cart_lyt layout{};

        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {0, 0}) == 0);
        CHECK(manhattan_distance<cart_lyt>(layout, {1, 1}, {1, 1}) == 0);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {0, 1}) == 1);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {1, 1}) == 2);
        CHECK(manhattan_distance<cart_lyt>(layout, {1, 2}, {3, 3}) == 3);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {4, 4}) == 8);
        CHECK(manhattan_distance<cart_lyt>(layout, {4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 1}) == 17);

        // negative coordinates
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {-1, -1}) == 2);
        CHECK(manhattan_distance<cart_lyt>(layout, {-4, -3}, {1, -1}) == 7);
        CHECK(manhattan_distance<cart_lyt>(layout, {-2, -8}, {-6, -4}) == 8);
    }
}