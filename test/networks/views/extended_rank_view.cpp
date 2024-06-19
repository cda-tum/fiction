//
// Created by benjamin on 6/18/24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/extended_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/views/rank_view.hpp>

using namespace fiction;

TEST_CASE("Check Traits", "[virtual-rank-view]")
{
    CHECK(!has_create_virtual_pi_v<technology_network>);
    CHECK(has_create_virtual_pi_v<virtual_pi_network>);
}
