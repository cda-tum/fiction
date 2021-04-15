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

template<class Lyt>
static void creation_and_usage_of_constants()
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<Lyt>);
    REQUIRE(mockturtle::has_size_v<Lyt>);
    REQUIRE(mockturtle::has_get_constant_v<Lyt>);
    REQUIRE(mockturtle::has_is_constant_v<Lyt>);
    REQUIRE(mockturtle::has_get_node_v<Lyt>);
    REQUIRE(mockturtle::has_is_complemented_v<Lyt>);

    Lyt layout{typename Lyt::aspect_ratio{2, 2, 1}};

    CHECK(layout.size() == 1);

    const auto c0 = layout.get_constant(false);
    CHECK(layout.is_constant(layout.get_node(c0)));
    CHECK(!layout.is_pi(layout.get_node(c0)));

    CHECK(layout.size() == 1);
    CHECK(std::is_same_v<std::decay_t<decltype(c0)>, typename Lyt::signal>);
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

TEST_CASE("Creation and usage of constants", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_and_usage_of_constants<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_and_usage_of_constants<mig_layout>();
    }
}

template <class Lyt>
static void creation_and_usage_of_primary_inputs()
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<Lyt>);
    REQUIRE(mockturtle::has_size_v<Lyt>);
    REQUIRE(mockturtle::has_create_pi_v<Lyt>);
    REQUIRE(mockturtle::has_num_pis_v<Lyt>);
    REQUIRE(mockturtle::has_num_gates_v<Lyt>);
    REQUIRE(mockturtle::has_foreach_pi_v<Lyt>);

    Lyt layout{typename Lyt::aspect_ratio{2, 2, 1}};

    auto a = layout.create_pi_tile({0, 0});

    CHECK(layout.size() == 2);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_gates() == 0);

    CHECK(std::is_same_v<std::decay_t<decltype(a)>, typename Lyt::signal>);

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

    auto b = layout.create_pi_tile({1, 0});
    auto c = layout.create_pi_tile({0, 1});

    CHECK(layout.num_pis() == 3);

    layout.foreach_pi_tile([&](auto t, auto i) {
        switch (i)
        {
            case 0:
            {
                CHECK(t == tile{0, 0});
                auto n = layout.get_tile_node(t);
                REQUIRE(n);
                CHECK(*n == layout.get_node(a));
                auto tn = layout.get_node_tile(*n);
                REQUIRE(tn);
                CHECK(*tn == t);

                break;
            }

            case 1:
            {
                CHECK(t == tile{1, 0});
                auto n = layout.get_tile_node(t);
                REQUIRE(n);
                CHECK(*n == layout.get_node(b));
                auto tn = layout.get_node_tile(*n);
                REQUIRE(tn);
                CHECK(*tn == t);

                break;
            }
            case 2:
            {
                CHECK(t == tile{0, 1});
                auto n = layout.get_tile_node(t);
                REQUIRE(n);
                CHECK(*n == layout.get_node(c));
                auto tn = layout.get_node_tile(*n);
                REQUIRE(tn);
                CHECK(*tn == t);

                break;
            }
        }
    });
}

TEST_CASE("Creation and usage of primary inputs", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_and_usage_of_primary_inputs<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_and_usage_of_primary_inputs<mig_layout>();
    }
}

template <class Lyt>
static void creation_and_usage_of_primary_outputs()
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<Lyt>);
    REQUIRE(mockturtle::has_size_v<Lyt>);
    REQUIRE(mockturtle::has_create_pi_v<Lyt>);
    REQUIRE(mockturtle::has_create_po_v<Lyt>);
    REQUIRE(mockturtle::has_num_pis_v<Lyt>);
    REQUIRE(mockturtle::has_num_pos_v<Lyt>);
    REQUIRE(mockturtle::has_foreach_po_v<Lyt>);

    Lyt layout{typename Lyt::aspect_ratio{2, 2, 1}};

    const auto c0 = layout.get_constant(false);
    const auto x1 = layout.create_pi_tile({0, 0});

    CHECK(layout.size() == 2);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_pos() == 0);

    layout.create_po_tile({1, 0}, c0);
    layout.create_po_tile({0, 1}, x1);
    layout.create_po_tile({1, 1}, !x1);

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
            case 0:
            {
                CHECK(t == tile{1, 0});
                auto s = layout.get_tile_signals(t);
                REQUIRE(s.size() == 1);
                CHECK(*s.begin() == c0);
                break;
            }
            case 1:
            {
                CHECK(t == tile{0, 1});
                auto s = layout.get_tile_signals(t);
                REQUIRE(s.size() == 1);
                CHECK(*s.begin() == x1);
                break;
            }
            case 2:
            {
                CHECK(t == tile{1, 1});
                auto s = layout.get_tile_signals(t);
                REQUIRE(s.size() == 1);
                CHECK(*s.begin() == !x1);
                break;
            }
        }
    });
}

TEST_CASE("Creation and usage of primary outputs", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_and_usage_of_primary_outputs<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_and_usage_of_primary_outputs<mig_layout>();
    }
}

template <class Lyt>
static void creation_of_unary_operations()
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<Lyt>);
    REQUIRE(mockturtle::has_size_v<Lyt>);
    REQUIRE(mockturtle::has_create_pi_v<Lyt>);
    REQUIRE(mockturtle::has_create_buf_v<Lyt>);
    REQUIRE(mockturtle::has_create_not_v<Lyt>);

    Lyt layout{typename Lyt::aspect_ratio{2, 2, 1}};

    auto x1 = layout.create_pi_tile({0, 0});

    CHECK(layout.size() == 2);

    auto f1 = layout.create_buf_tile({1, 0}, x1);
    auto f2 = layout.create_not_tile({0, 1}, x1);

    CHECK(layout.size() == 2);
    CHECK(f1 == x1);
    CHECK(f2 == !x1);

    auto s1 = layout.get_tile_signals({1, 0});
    REQUIRE(s1.size() == 1);
    CHECK(*s1.begin() == f1);

    auto s2 = layout.get_tile_signals({0, 1});
    REQUIRE(s2.size() == 1);
    CHECK(*s2.begin() == f2);
}

TEST_CASE("Creation of unary operations", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_of_unary_operations<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_of_unary_operations<mig_layout>();
    }
}

template <class Lyt>
static void creation_of_binary_operations()
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<Lyt>);
    REQUIRE(mockturtle::has_create_pi_v<Lyt>);
    REQUIRE(mockturtle::has_create_and_v<Lyt>);
    REQUIRE(mockturtle::has_create_or_v<Lyt>);

    Lyt layout{typename Lyt::aspect_ratio{2, 2, 1}};

    auto x1 = layout.create_pi_tile({1, 0});
    auto x2 = layout.create_pi_tile({0, 1});

    CHECK(layout.num_pis() == 2);

    auto a = layout.create_and_tile({0, 0}, x1, x2);
    auto o = layout.create_or_tile({1, 1}, x1, x2);

    CHECK(a != o);
    CHECK(layout.num_gates() == 2);

    CHECK(layout.get_tile_node({0, 0}) == layout.get_node(a));
    CHECK(layout.get_tile_node({1, 1}) == layout.get_node(o));
}

TEST_CASE("Creation of binary operations", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_of_binary_operations<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_of_binary_operations<mig_layout>();
    }
}

template <class Lyt>
static void creation_of_ternary_operations()
{
    // adapted from mockturtle/test/networks/*.cpp

    REQUIRE(mockturtle::is_network_type_v<Lyt>);
    REQUIRE(mockturtle::has_create_pi_v<Lyt>);
    REQUIRE(mockturtle::has_num_pis_v<Lyt>);
    REQUIRE(mockturtle::has_create_maj_v<Lyt>);

    Lyt layout{typename Lyt::aspect_ratio{2, 3, 1}};

    auto x1 = layout.create_pi_tile({1, 0});
    auto x2 = layout.create_pi_tile({0, 1});
    auto x3 = layout.create_pi_tile({1, 2});

    CHECK(layout.num_pis() == 3);

    auto m = layout.create_maj_tile({1, 1}, x1, x2, x3);

    CHECK(x1 != m);
    CHECK(x2 != m);
    CHECK(x3 != m);

    if constexpr (std::is_same<Lyt, tile_based_layout<mockturtle::mig_network>>::value)
    {
        CHECK(layout.num_gates() == 1);
    }

    CHECK(layout.get_tile_node({1, 1}) == layout.get_node(m));
}

TEST_CASE("Creation of ternary operations", "[tile-based]")
{
    SECTION("AIG layouts")
    {
        using aig_layout = tile_based_layout<mockturtle::aig_network>;

        creation_of_ternary_operations<aig_layout>();
    }

    SECTION("MIG layouts")
    {
        using mig_layout = tile_based_layout<mockturtle::mig_network>;

        creation_of_ternary_operations<mig_layout>();
    }
}