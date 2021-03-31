//
// Created by marcel on 31.03.21.
//

#include "catch.hpp"

#include <tile_based_layout.hpp>
#include <mockturtle/networks/aig.hpp>

using namespace fiction;

TEST_CASE("Layout creation", "[tile-based]")
{
    tile_based_layout<mockturtle::aig_network> layout{};

    auto t1 = layout.tile(1, 2, 0);
    auto t2 = layout.tile(1, 2);

    CHECK(t1 == t2);

    t1.x += 1;

    CHECK(!(t1 == t2));

    CHECK(true);
}
