//
// Created by marcel on 31.03.21.
//

#include "catch.hpp"

#include <tile_based_layout.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/traits.hpp>

using namespace fiction;

TEST_CASE("Tiles", "[tile-based")
{
    auto t1 = tile{1, 2, 0};
    auto t2 = tile{1, 2};

    CHECK(t1 == t2);

    t1.z += 4;

    CHECK(t1 == t2);

    t1.x++;

    CHECK(!(t1 == t2));
}

TEST_CASE("AIG layout creation and constant usage", "[tile-based]")
{
    // adapted from mockturtle/test/networks/aig.cpp

    using aig_layout = tile_based_layout<mockturtle::aig_network>;
    aig_layout layout{aspect_ratio{2, 2, 1}};

    CHECK(layout.size() == 1);
    CHECK(mockturtle::has_get_constant_v<aig_layout>);
//    CHECK(mockturtle::has_is_constant_v<aig_layout>);
    CHECK(mockturtle::has_get_node_v<aig_layout>);
    CHECK(mockturtle::has_is_complemented_v<aig_layout>);

    const auto c0 = layout.get_constant(false);
    CHECK(layout.is_constant(layout.get_node(c0)));
    CHECK(!layout.is_pi(layout.get_node(c0)));

    CHECK(layout.size() == 1);
    CHECK(std::is_same_v<std::decay_t<decltype(c0)>, aig_layout::signal>);
    CHECK(layout.get_node(c0) == 0);
    CHECK(!layout.is_complemented(c0));

    const auto c1 = layout.get_constant(true);

    CHECK(layout.get_node(c1) == 0);
    CHECK(layout.is_complemented(c1));

    CHECK(c0 != c1);
    CHECK(c0 == !c1);
    CHECK((!c0) == c1);
    CHECK((!c0) != !c1);
    CHECK(-c0 == c1);
    CHECK(-c1 == c1);
    CHECK(c0 == +c1);
    CHECK(c0 == +c0);
}

TEST_CASE("MIG layout creation and constant usage", "[tile-based]")
{
    // adapted from mockturtle/test/networks/mig.cpp

    using mig_layout = tile_based_layout<mockturtle::mig_network>;
    mig_layout layout{aspect_ratio{2, 2, 1}};

    CHECK(layout.size() == 1);
    CHECK(mockturtle::has_get_constant_v<mig_layout>);
//    CHECK(mockturtle::has_is_constant_v<mig_layout>);
    CHECK(mockturtle::has_get_node_v<mig_layout>);
    CHECK(mockturtle::has_is_complemented_v<mig_layout>);

    const auto c0 = layout.get_constant(false);
    CHECK(layout.is_constant(layout.get_node(c0)));
    CHECK(!layout.is_pi(layout.get_node(c0)));

    CHECK(layout.size() == 1);
    CHECK(std::is_same_v<std::decay_t<decltype(c0)>, mig_layout::signal>);
    CHECK(layout.get_node(c0) == 0);
    CHECK(!layout.is_complemented(c0));

    const auto c1 = layout.get_constant(true);

    CHECK(layout.get_node(c1) == 0);
    CHECK(layout.is_complemented(c1));

    CHECK(c0 != c1);
    CHECK(c0 == !c1);
    CHECK((!c0) == c1);
    CHECK((!c0) != !c1);
    CHECK(-c0 == c1);
    CHECK(-c1 == c1);
    CHECK(c0 == +c1);
    CHECK(c0 == +c0);
}
