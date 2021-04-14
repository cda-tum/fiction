//
// Created by marcel on 31.03.21.
//

#include "catch.hpp"

#include <iostream>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/traits.hpp>
#include <tile_based_layout.hpp>

using namespace fiction;

TEST_CASE("Tiles", "[tile-based")
{
    auto t0 = tile{0, 0, 0};

    CHECK(t0 == tile{0, 0});

    auto t1 = tile{1, 2, 0};
    auto t2 = tile{1, 2};

    CHECK(t0 < t1);
    CHECK(t1 > t0);
    CHECK(t1 >= t0);
    CHECK(t0 <= t1);
    CHECK(t1 == t2);
    CHECK(t2 == t1);

    t1.z += static_cast<uint64_t>(4ul);

    CHECK(t1 == t2);

    t1.y += static_cast<uint64_t>(2147483648ul);

    CHECK(t1 == t2);

    t1.x += static_cast<uint64_t>(2147483648ul);

    CHECK(t1 == t2);

    t1.x++;

    CHECK(t1 != t2);
    CHECK(t1 > t2);
    CHECK(t1 >= t2);
    CHECK(t2 < t1);
    CHECK(t2 <= t1);
}

template<class Ntk>
static void creation_and_constants()
{
    // adapted from mockturtle/test/networks/*.cpp

    CHECK(mockturtle::is_network_type_v<Ntk>);

    Ntk layout{typename Ntk::aspect_ratio{2, 2, 1}};

    CHECK(layout.size() == 1);
    CHECK(mockturtle::has_get_constant_v<Ntk>);
    CHECK(mockturtle::has_is_constant_v<Ntk>);
    CHECK(mockturtle::has_get_node_v<Ntk>);
    CHECK(mockturtle::has_is_complemented_v<Ntk>);

    const auto c0 = layout.get_constant(false);
    CHECK(layout.is_constant(layout.get_node(c0)));
    CHECK(!layout.is_pi(layout.get_node(c0)));

    CHECK(layout.size() == 1);
    CHECK(std::is_same_v<std::decay_t<decltype(c0)>, typename Ntk::signal>);
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

TEST_CASE("Layout creation and usage of constants", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_and_constants<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_and_constants<mig_layout>();
    }
}

template <class Ntk>
static void creation_and_primary_inputs()
{
    // adapted from mockturtle/test/networks/*.cpp

    Ntk layout{typename Ntk::aspect_ratio{2, 2, 1}};

    auto a = layout.create_pi({0, 0});

    CHECK(layout.size() == 2);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_gates() == 0);

    CHECK(std::is_same_v<std::decay_t<decltype(a)>, typename Ntk::signal>);

    CHECK(a.index == 1);
    CHECK(a.complement == 0);

    a = !a;

    CHECK(a.index == 1);
    CHECK(a.complement == 1);

    a = +a;

    CHECK(a.index == 1);
    CHECK(a.complement == 0);

    a = +a;

    CHECK(a.index == 1);
    CHECK(a.complement == 0);

    a = -a;

    CHECK(a.index == 1);
    CHECK(a.complement == 1);

    a = -a;

    CHECK(a.index == 1);
    CHECK(a.complement == 1);

    a = a ^ true;

    CHECK(a.index == 1);
    CHECK(a.complement == 0);

    a = a ^ true;

    CHECK(a.index == 1);
    CHECK(a.complement == 1);
}

TEST_CASE("Layout creation and usage of primary inputs", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_and_primary_inputs<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_and_primary_inputs<mig_layout>();
    }
}

template <class Ntk>
static void creation_and_primary_outputs()
{
    // adapted from mockturtle/test/networks/*.cpp

    Ntk layout{typename Ntk::aspect_ratio{2, 2, 1}};

    const auto c0 = layout.get_constant(false);
    const auto x1 = layout.create_pi({0, 0});

    CHECK(layout.size() == 2);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_pos() == 0);

    layout.create_po({1, 0}, c0);
    layout.create_po({0, 1}, x1);
    layout.create_po({1, 1}, !x1);

    CHECK(layout.size() == 2);
    CHECK(layout.num_pos() == 3);

    layout.foreach_po([&](auto s, auto i) {
        switch (i)
            {
                case 0: CHECK(s == c0); break;
                case 1: CHECK(s == x1); break;
                case 2: CHECK(s == !x1); break;
            }
    });

    layout.foreach_po_tile([&](auto t, auto i) {
        switch (i)
            {
                case 0: CHECK(t == tile{1, 0}); break;
                case 1: CHECK(t == tile{0, 1}); break;
                case 2: CHECK(t == tile{1, 1}); break;
            }
    });
}

TEST_CASE("Layout creation and usage of primary outputs", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_and_primary_outputs<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_and_primary_outputs<mig_layout>();
    }
}
