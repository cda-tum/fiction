//
// Created by marcel on 31.03.21.
//

#include "tile_based_layout.hpp"

#include "catch.hpp"

#include <mockturtle/traits.hpp>

#include <map>
#include <set>
#include <type_traits>


using namespace fiction;

TEST_CASE("Tiles", "[tile-based")
{
    auto td = tile_based_layout::tile{};
    CHECK(td.is_dead());

    auto t0 = tile_based_layout::tile{0, 0, 0};
    CHECK(!t0.is_dead());

    CHECK(t0 == td);

    auto t1 = tile_based_layout::tile{1, 2, 0};
    auto t2 = tile_based_layout::tile{1, 2};

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

    std::map<uint64_t, tile_based_layout::tile> tile_repr{
        {0x8000000000000000, tile_based_layout::tile{}},
        {0x0000000000000000, tile_based_layout::tile{0, 0, 0}},
        {0x4000000000000000, tile_based_layout::tile{0, 0, 1}},
        {0x4000000080000001, tile_based_layout::tile{1, 1, 1}},
        {0x3fffffffffffffff, tile_based_layout::tile{2147483647, 2147483647, 0}}};

    for (auto [repr, tile] : tile_repr)
    {
        CHECK(static_cast<tile_based_layout::tile>(repr) == tile);
        CHECK(repr == static_cast<uint64_t>(tile));
        CHECK(tile_based_layout::tile{repr} == tile);
    }
}

TEST_CASE("Tile iteration", "[tile-based]")
{
    tile_based_layout::aspect_ratio ar{50, 50, 1};

    tile_based_layout layout{ar};

    std::set<tile_based_layout::tile> visited{};

    const auto check1 = [&visited, &ar, &layout](const auto& t)
    {
        CHECK(t < ar);
        CHECK(layout.is_empty_tile(t));

        // no tile is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.tiles())
    {
        check1(t);
    }

    visited.clear();

    layout.foreach_tile(check1);

    visited.clear();

    tile_based_layout::aspect_ratio ar_ground{ar.x, ar.y, 0};

    const auto check2 = [&visited, &ar_ground](const auto& t)
    {
        // iteration stays in ground layer
        CHECK(t.z == 0);
        CHECK(t < ar_ground);

        // no tile is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);

    };

    for (auto&& t : layout.ground_tiles())
    {
        check2(t);
    }

    visited.clear();

    layout.foreach_ground_tile(check2);

    visited.clear();

    tile_based_layout::tile start{42, 20}, stop{25, 40};

    const auto check3 = [&visited, &start, &stop](const auto& t)
    {
        CHECK(t.z == 0);
        // iteration stays in between the bounds
        CHECK(t >= start);
        CHECK(t < stop);

        // no tile is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.tiles(start, stop))
    {
        check3(t);
    }

    visited.clear();

    layout.foreach_tile(check3, start, stop);
}

TEST_CASE("Cardinal operations", "[tile-based]")
{
    tile_based_layout::aspect_ratio ar{10, 10, 1};

    tile_based_layout layout{ar};
    
    auto nt = layout.north({5, 5});
    CHECK(!nt.is_dead());
    CHECK(nt == tile_based_layout::tile{5, 4});
    nt = layout.north({5, 0});
    CHECK(!nt.is_dead());
    CHECK(nt == tile_based_layout::tile{5, 0});
    CHECK(layout.is_border(nt));
    CHECK(layout.is_northern_border(nt));
    CHECK(layout.is_ground_layer(nt));

    auto et = layout.east({5, 5});
    CHECK(!et.is_dead());
    CHECK(et == tile_based_layout::tile{6, 5});
    et = layout.east({10, 5});
    CHECK(!et.is_dead());
    CHECK(et == tile_based_layout::tile{10, 5});
    CHECK(layout.is_border(et));
    CHECK(layout.is_eastern_border(et));
    CHECK(layout.is_ground_layer(et));
    et = layout.east({11, 5});
    CHECK(et.is_dead());

    auto st = layout.south({5, 5});
    CHECK(!st.is_dead());
    CHECK(st == tile_based_layout::tile{5, 6});
    st = layout.south({5, 10});
    CHECK(!st.is_dead());
    CHECK(st == tile_based_layout::tile{5, 10});
    CHECK(layout.is_border(st));
    CHECK(layout.is_southern_border(st));
    CHECK(layout.is_ground_layer(st));
    st = layout.south({5, 11});
    CHECK(st.is_dead());

    auto wt = layout.west({5, 5});
    CHECK(!wt.is_dead());
    CHECK(wt == tile_based_layout::tile{4, 5});
    wt = layout.west({0, 5});
    CHECK(!wt.is_dead());
    CHECK(wt == tile_based_layout::tile{0, 5});
    CHECK(layout.is_border(wt));
    CHECK(layout.is_western_border(wt));
    CHECK(layout.is_ground_layer(wt));

    auto at = layout.above({5, 5, 0});
    CHECK(!at.is_dead());
    CHECK(at == tile_based_layout::tile{5, 5, 1});
    at = layout.above({5, 5, 1});
    CHECK(!at.is_dead());
    CHECK(at == tile_based_layout::tile{5, 5, 1});
    CHECK(layout.is_crossing_layer(at));
    CHECK(!layout.is_border(at));
//    at = layout.above({5, 5, 2});
//    CHECK(at.is_dead());

    auto bt = layout.below({5, 5, 1});
    CHECK(!bt.is_dead());
    CHECK(bt == tile_based_layout::tile{5, 5, 0});
    bt = layout.below({5, 5, 0});
    CHECK(!bt.is_dead());
    CHECK(bt == tile_based_layout::tile{5, 5, 0});
    CHECK(layout.is_ground_layer(bt));
}

TEST_CASE("Creation and usage of constants", "[tile-based]")
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<tile_based_layout>);
    REQUIRE(mockturtle::has_size_v<tile_based_layout>);
    REQUIRE(mockturtle::has_get_constant_v<tile_based_layout>);
    REQUIRE(mockturtle::has_is_constant_v<tile_based_layout>);
    REQUIRE(mockturtle::has_get_node_v<tile_based_layout>);
    REQUIRE(mockturtle::has_is_complemented_v<tile_based_layout>);

    tile_based_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    CHECK(layout.size() == 0);

    const auto c0 = layout.get_constant(false);
    CHECK(layout.is_constant(layout.get_node(c0)));
    CHECK(!layout.is_pi(layout.get_node(c0)));

    CHECK(layout.size() == 0);
    CHECK(std::is_same_v<std::decay_t<decltype(c0)>, tile_based_layout::signal>);
    CHECK(layout.get_node(c0) == 0);
    CHECK(!layout.is_complemented(c0));

    const auto c1 = layout.get_constant(true);
    CHECK(layout.is_constant(layout.get_node(c1)));

    CHECK(layout.get_node(c1) == 0);
    CHECK(!layout.is_complemented(c1));
}

TEST_CASE("Creation and usage of primary inputs", "[tile-based]")
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<tile_based_layout>);
    REQUIRE(mockturtle::has_size_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_pi_v<tile_based_layout>);
    REQUIRE(mockturtle::has_num_pis_v<tile_based_layout>);
    REQUIRE(mockturtle::has_num_gates_v<tile_based_layout>);
    REQUIRE(mockturtle::has_foreach_pi_v<tile_based_layout>);

    tile_based_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    auto a = layout.create_pi("a", {0, 0});
    CHECK(layout.is_pi(layout.get_node(a)));

    CHECK(layout.size() == 1);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_gates() == 0);

    CHECK(std::is_same_v<std::decay_t<decltype(a)>, tile_based_layout::signal>);

    auto b = layout.create_pi("b", {1, 0});
    auto c = layout.create_pi("c", {0, 1});

    CHECK(layout.is_pi(layout.get_node(b)));
    CHECK(layout.is_pi(layout.get_node(c)));

    CHECK(layout.num_pis() == 3);

    layout.foreach_pi(
        [&](auto pi, auto i)
        {
            const auto check = [&layout, &pi](auto c, auto s)
            {
                auto t = layout.get_tile(pi);
                CHECK(t == c);
                auto n = layout.get_node(static_cast<tile_based_layout::signal>(t));
                CHECK(n == layout.get_node(s));
                auto tn = layout.get_tile(n);
                CHECK(tn == t);
            };

            CHECK(layout.is_pi(pi));
            CHECK(!layout.is_gate_tile(static_cast<tile_based_layout::tile>(pi)));

            switch (i)
            {
                case 0:
                {
                    check(tile_based_layout::tile{0, 0}, a);

                    break;
                }

                case 1:
                {
                    check(tile_based_layout::tile{1, 0}, b);

                    break;
                }
                case 2:
                {
                    check(tile_based_layout::tile{0, 1}, c);

                    break;
                }
            }
        });
}

TEST_CASE("Creation and usage of primary outputs", "[tile-based]")
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<tile_based_layout>);
    REQUIRE(mockturtle::has_size_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_pi_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_po_v<tile_based_layout>);
    REQUIRE(mockturtle::has_num_pis_v<tile_based_layout>);
    REQUIRE(mockturtle::has_num_pos_v<tile_based_layout>);
    REQUIRE(mockturtle::has_foreach_po_v<tile_based_layout>);

    tile_based_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    const auto x1 = layout.create_pi("x1", tile_based_layout::tile{0, 0});

    CHECK(layout.size() == 1);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_pos() == 0);

    const auto f1 = layout.create_po(x1, "f1", tile_based_layout::tile{0, 1});
    const auto f2 = layout.create_po(!x1, "f2", tile_based_layout::tile{1, 1});

    CHECK(layout.is_po(layout.get_node(f1)));
    CHECK(layout.is_po(layout.get_node(f2)));

    CHECK(layout.size() == 3);
    CHECK(layout.num_pos() == 2);

    layout.foreach_po(
        [&](auto po, auto i)
        {
            const auto check = [&layout, &po](auto c, auto s)
            {
                auto t = layout.get_tile(po);
                CHECK(t == c);
                auto n = layout.get_node(static_cast<tile_based_layout::signal>(t));
                CHECK(n == po);
                auto tn = layout.get_tile(n);
                CHECK(tn == t);
            };

            CHECK(layout.is_po(po));
            CHECK(!layout.is_gate_tile(static_cast<tile_based_layout::tile>(po)));

            switch (i)
            {
                case 0:
                {
                    check(tile_based_layout::tile{0, 1}, x1);

                    break;
                }
                case 1:
                {
                    check(tile_based_layout::tile{1, 1}, !x1);

                    break;
                }
            }
        });
}

TEST_CASE("Creation of unary operations", "[tile-based]")
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<tile_based_layout>);
    REQUIRE(mockturtle::has_size_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_pi_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_buf_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_not_v<tile_based_layout>);

    tile_based_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    auto x1 = layout.create_pi("x1", {0, 0});

    CHECK(layout.size() == 1);

    auto f1 = layout.create_buf(x1, {1, 0});
    auto f2 = layout.create_not(x1, {0, 1});

    CHECK(layout.size() == 3);

    auto x2 = layout.create_pi("x2", {1, 1});
    CHECK(layout.is_pi(layout.get_node(x2)));

    auto f1n  = layout.get_node(f1);
    auto t10n = layout.get_node(static_cast<tile_based_layout::signal>(tile_based_layout::tile{1, 0}));
    CHECK(f1n == t10n);

    auto f2n  = layout.get_node(f2);
    auto t01n = layout.get_node(static_cast<tile_based_layout::signal>(tile_based_layout::tile{0, 1}));
    CHECK(f2n == t01n);

    CHECK(!layout.is_gate_tile({1, 0}));
    CHECK(layout.is_wire_tile({1, 0}));

    CHECK(layout.is_gate_tile({0, 1}));
    CHECK(!layout.is_wire_tile({0, 1}));
}

TEST_CASE("Creation of binary operations", "[tile-based]")
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_pi_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_and_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_or_v<tile_based_layout>);

    tile_based_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    auto x1 = layout.create_pi("x1", {1, 0});
    auto x2 = layout.create_pi("x2", {0, 1});

    CHECK(layout.num_pis() == 2);

    auto a = layout.create_and(x1, x2, {0, 0});
    auto o = layout.create_or(x1, x2, {1, 1});

    CHECK(a != o);
    CHECK(layout.num_gates() == 2);
    CHECK(layout.size() == 4);

    CHECK(layout.get_node({0, 0}) == layout.get_node(a));
    CHECK(layout.get_node({1, 1}) == layout.get_node(o));

    CHECK(layout.is_gate_tile({0, 0}));
    CHECK(layout.is_gate_tile({1, 1}));

    CHECK(!layout.is_wire_tile({0, 0}));
    CHECK(!layout.is_wire_tile({1, 1}));
}

TEST_CASE("Creation of ternary operations", "[tile-based]")
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_pi_v<tile_based_layout>);
    REQUIRE(mockturtle::has_num_pis_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_maj_v<tile_based_layout>);

    tile_based_layout layout{tile_based_layout::aspect_ratio{2, 3, 1}};

    auto x1 = layout.create_pi("x1",{1, 0});
    auto x2 = layout.create_pi("x2",{0, 1});
    auto x3 = layout.create_pi("x3",{1, 2});

    CHECK(layout.num_pis() == 3);

    auto m = layout.create_maj(x1, x2, x3, {1, 1});

    CHECK(x1 != m);
    CHECK(x2 != m);
    CHECK(x3 != m);

    CHECK(layout.num_gates() == 1);
    CHECK(layout.size() == 4);

    CHECK(layout.get_node({1, 1}) == layout.get_node(m));

    CHECK(layout.is_gate_tile({1, 1}));
    CHECK(!layout.is_wire_tile({1, 1}));
}

TEST_CASE("Clocking", "[tile-based]")
{
    REQUIRE(mockturtle::is_network_type_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_pi_v<tile_based_layout>);
    REQUIRE(mockturtle::has_num_pis_v<tile_based_layout>);
    REQUIRE(mockturtle::has_create_and_v<tile_based_layout>);

    tile_based_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}, twoddwave_4_clocking};

    auto x1 = layout.create_pi("x1", {1, 0});
    auto x2 = layout.create_pi("x2", {0, 1});

    auto a1 = layout.create_and(x1, x2, {1, 1});

    CHECK(layout.is_regularly_clocked());
    CHECK(layout.tile_clocking({0, 0}) == 0);
    CHECK(layout.tile_clocking({1, 0}) == 1);
    CHECK(layout.tile_clocking({0, 1}) == 1);
    CHECK(layout.tile_clocking({1, 1}) == 2);

    CHECK(layout.num_clocks() == 4);
    CHECK(layout.is_clocking(clock_name::twoddwave4));

    layout.assign_clocking(static_cast<tile_based_layout::tile>(x1), 2);
    layout.assign_clocking(static_cast<tile_based_layout::tile>(x2), 2);
    layout.assign_clocking(static_cast<tile_based_layout::tile>(a1), 3);

    CHECK(layout.tile_clocking({0, 0}) == 0);
    CHECK(layout.tile_clocking({1, 0}) == 2);
    CHECK(layout.tile_clocking({0, 1}) == 2);
    CHECK(layout.tile_clocking({1, 1}) == 3);
}
