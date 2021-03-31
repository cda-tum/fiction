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

    CHECK(true);
}
