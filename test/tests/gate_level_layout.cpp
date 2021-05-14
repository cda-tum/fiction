//
// Created by marcel on 31.03.21.
//

#include "tile_based_layout.hpp"
#include "gate_level_layout.hpp"
#include "clocked_layout.hpp"

#include "catch.hpp"

#include <mockturtle/traits.hpp>

#include <type_traits>


using namespace fiction;


TEST_CASE("Creation and usage of constants", "[gate-level]")
{
    // adapted from mockturtle/test/networks/*.cpp
    
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_get_constant_v<gate_layout>);
    REQUIRE(mockturtle::has_is_constant_v<gate_layout>);
    REQUIRE(mockturtle::has_get_node_v<gate_layout>);
    REQUIRE(mockturtle::has_is_complemented_v<gate_layout>);

    gate_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    CHECK(layout.size() == 0);

    const auto c0 = layout.get_constant(false);
    CHECK(layout.is_constant(layout.get_node(c0)));
    CHECK(!layout.is_pi(layout.get_node(c0)));

    CHECK(layout.size() == 0);
    CHECK(std::is_same_v<std::decay_t<decltype(c0)>, gate_layout::signal>);
    CHECK(layout.get_node(c0) == 0);
    CHECK(!layout.is_complemented(c0));

    const auto c1 = layout.get_constant(true);
    CHECK(layout.is_constant(layout.get_node(c1)));

    CHECK(layout.get_node(c1) == 0);
    CHECK(!layout.is_complemented(c1));
}

TEST_CASE("Creation and usage of primary inputs", "[gate-level]")
{
    // adapted from mockturtle/test/networks/*.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pis_v<gate_layout>);
    REQUIRE(mockturtle::has_num_gates_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_pi_v<gate_layout>);

    gate_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    auto a = layout.create_pi("a", {0, 0});
    CHECK(layout.is_pi(layout.get_node(a)));

    CHECK(layout.size() == 1);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_gates() == 0);

    CHECK(std::is_same_v<std::decay_t<decltype(a)>, gate_layout::signal>);

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
                auto n = layout.get_node(static_cast<gate_layout::signal>(t));
                CHECK(n == layout.get_node(s));
                auto tn = layout.get_tile(n);
                CHECK(tn == t);
            };

            CHECK(layout.is_pi(pi));
            CHECK(!layout.is_gate_tile(static_cast<gate_layout::tile>(pi)));

            switch (i)
            {
                case 0:
                {
                    check(gate_layout::tile{0, 0}, a);

                    break;
                }

                case 1:
                {
                    check(gate_layout::tile{1, 0}, b);

                    break;
                }
                case 2:
                {
                    check(gate_layout::tile{0, 1}, c);

                    break;
                }
            }
        });
}

TEST_CASE("Creation and usage of primary outputs", "[gate-level]")
{
    // adapted from mockturtle/test/networks/*.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_create_po_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pis_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pos_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_po_v<gate_layout>);

    gate_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

    const auto x1 = layout.create_pi("x1", gate_layout::tile{0, 0});

    CHECK(layout.size() == 1);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_pos() == 0);

    const auto f1 = layout.create_po(x1, "f1", gate_layout::tile{0, 1});
    const auto f2 = layout.create_po(!x1, "f2", gate_layout::tile{1, 1});

    CHECK(layout.is_po(layout.get_node(f1)));
    CHECK(layout.is_po(layout.get_node(f2)));

    CHECK(layout.is_empty_tile({1, 0}));

    CHECK(layout.size() == 3);
    CHECK(layout.num_pos() == 2);

    layout.foreach_po(
        [&](auto po, auto i)
        {
            const auto check = [&layout, &po](auto c)
            {
                auto t = layout.get_tile(po);
                CHECK(t == c);
                auto n = layout.get_node(static_cast<gate_layout::signal>(t));
                CHECK(n == po);
                auto tn = layout.get_tile(n);
                CHECK(tn == t);
            };

            CHECK(layout.is_po(po));
            CHECK(!layout.is_gate_tile(static_cast<gate_layout::tile>(po)));

            switch (i)
            {
                case 0:
                {
                    check(gate_layout::tile{0, 1});

                    break;
                }
                case 1:
                {
                    check(gate_layout::tile{1, 1});

                    break;
                }
            }
        });
}

TEST_CASE("Creation of unary operations", "[gate-level]")
{
    // adapted from mockturtle/test/networks/*.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_create_buf_v<gate_layout>);
    REQUIRE(mockturtle::has_create_not_v<gate_layout>);

    gate_layout layout{gate_layout::aspect_ratio{2, 2, 1}};

    auto x1 = layout.create_pi("x1", {0, 0});

    CHECK(layout.size() == 1);

    auto f1 = layout.create_buf(x1, {1, 0});
    auto f2 = layout.create_not(x1, {0, 1});

    CHECK(layout.size() == 3);

    auto x2 = layout.create_pi("x2", {1, 1});
    CHECK(layout.is_pi(layout.get_node(x2)));

    auto f1n  = layout.get_node(f1);
    auto t10n = layout.get_node(static_cast<gate_layout::signal>(gate_layout::tile{1, 0}));
    CHECK(f1n == t10n);

    auto f2n  = layout.get_node(f2);
    auto t01n = layout.get_node(static_cast<gate_layout::signal>(gate_layout::tile{0, 1}));
    CHECK(f2n == t01n);

    CHECK(!layout.is_empty_tile({1, 0}));
    CHECK(!layout.is_gate_tile({1, 0}));
    CHECK(layout.is_wire_tile({1, 0}));

    CHECK(!layout.is_empty_tile({0, 1}));
    CHECK(layout.is_gate_tile({0, 1}));
    CHECK(!layout.is_wire_tile({0, 1}));
}

TEST_CASE("Creation of binary operations", "[gate-level]")
{
    // adapted from mockturtle/test/networks/*.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_create_and_v<gate_layout>);
    REQUIRE(mockturtle::has_create_or_v<gate_layout>);

    gate_layout layout{tile_based_layout::aspect_ratio{2, 2, 1}};

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

TEST_CASE("Creation of ternary operations", "[gate-level]")
{
    // adapted from mockturtle/test/networks/*.cpp
    
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pis_v<gate_layout>);
    REQUIRE(mockturtle::has_create_maj_v<gate_layout>);

    gate_layout layout{tile_based_layout::aspect_ratio{2, 3, 1}};

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
