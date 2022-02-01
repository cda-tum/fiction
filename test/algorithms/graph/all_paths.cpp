//
// Created by marcel on 01.02.22.
//

#include "catch.hpp"

#include <fiction/algorithms/graph/all_paths.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>

using namespace fiction;

TEST_CASE("Clocked layouts", "[all-paths]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using path = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{1, 1}, twoddwave_clocking<clk_lyt>()};

        const auto collection = all_layout_clocking_paths<path>(layout, {0, 0}, {1, 1});

        CHECK(collection.size() == 2);
        CHECK(collection.contains({{{0, 0}, {0, 1}, {1, 1}}}));
        CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}}}));
    }
}
