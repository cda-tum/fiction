//
// Created by marcel on 31.03.21.
//

#include "clocked_layout.hpp"
#include "tile_based_layout.hpp"

#include "catch.hpp"


using namespace fiction;


TEST_CASE("Clocking", "[clocked]")
{
    clocked_layout<tile_based_layout> layout{tile_based_layout::aspect_ratio{2, 2, 1}, twoddwave_4_clocking};

    CHECK(layout.is_clocking(clock_name::twoddwave4));
    CHECK(layout.is_regularly_clocked());
    CHECK(layout.num_clocks() == 4);

    CHECK(layout.tile_clocking({0, 0}) == 0);
    CHECK(layout.tile_clocking({1, 0}) == 1);
    CHECK(layout.tile_clocking({0, 1}) == 1);
    CHECK(layout.tile_clocking({1, 1}) == 2);

    CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
    CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
    CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));
    CHECK(!layout.is_incoming_clocked({1, 1}, {1, 1}));

    CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
    CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
    CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
    CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 1}));
    CHECK(!layout.is_outgoing_clocked({1, 1}, {1, 1}));

    layout.assign_clocking({1, 0}, 2);
    layout.assign_clocking({0, 1}, 2);
    layout.assign_clocking({1, 1}, 3);

    CHECK(layout.tile_clocking({0, 0}) == 0);
    CHECK(layout.tile_clocking({1, 0}) == 2);
    CHECK(layout.tile_clocking({0, 1}) == 2);
    CHECK(layout.tile_clocking({1, 1}) == 3);
}
