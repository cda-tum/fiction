//
// Created by marcel on 31.03.21.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/layouts/clocking_scheme.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/traits.hpp>

#include <type_traits>

using namespace fiction;

TEST_CASE("Gate-level layout traits", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK(fiction::is_coordinate_layout_v<gate_layout>);
    CHECK(fiction::is_tile_based_layout_v<gate_layout>);
    CHECK(fiction::is_clocked_layout_v<gate_layout>);
    CHECK(fiction::is_gate_level_layout_v<gate_layout>);
    CHECK(fiction::has_is_empty_tile_v<gate_layout>);
    CHECK(fiction::has_is_empty_v<gate_layout>);
}

TEST_CASE("Deep copy gate-level layout", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    gate_layout original{{5, 5, 0}, twoddwave_clocking<gate_layout>(), "Original"};
    original.create_pi("x1", {0, 2});
    original.create_pi("x2", {2, 4});

    auto copy = original.clone();

    copy.resize(aspect_ratio_t<gate_layout>{10, 10, 1});
    copy.replace_clocking_scheme(use_clocking<gate_layout>());
    copy.set_layout_name("Copy");
    copy.move_node(copy.get_node({0, 2}), {0, 0});
    copy.move_node(copy.get_node({2, 4}), {2, 0});

    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);
    CHECK(original.is_clocking_scheme(clock_name::TWODDWAVE));
    CHECK(original.get_layout_name() == "Original");
    CHECK(original.is_pi_tile({0, 2}));
    CHECK(original.is_pi_tile({2, 4}));

    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
    CHECK(copy.is_clocking_scheme(clock_name::USE));
    CHECK(copy.get_layout_name() == "Copy");
    CHECK(copy.is_pi_tile({0, 0}));
    CHECK(copy.is_pi_tile({2, 0}));
}

TEST_CASE("Creation and usage of constants", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_get_constant_v<gate_layout>);
    REQUIRE(mockturtle::has_is_constant_v<gate_layout>);
    REQUIRE(mockturtle::has_get_node_v<gate_layout>);
    REQUIRE(mockturtle::has_is_complemented_v<gate_layout>);

    const gate_layout layout{{2, 2, 1}};

    CHECK(layout.size() == 2);

    const auto c0 = layout.get_constant(false);
    CHECK(layout.is_constant(layout.get_node(c0)));
    CHECK(!layout.is_pi(layout.get_node(c0)));

    CHECK(layout.size() == 2);
    CHECK(std::is_same_v<std::decay_t<decltype(c0)>, gate_layout::signal>);
    CHECK(layout.get_node(c0) == 0);
    CHECK(!layout.is_complemented(c0));

    const auto c1 = layout.get_constant(true);
    CHECK(layout.is_constant(layout.get_node(c1)));

    CHECK(layout.get_node(c1) == 1);
    CHECK(!layout.is_complemented(c1));
}

TEST_CASE("Creation and usage of primary inputs", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pis_v<gate_layout>);
    REQUIRE(mockturtle::has_num_gates_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_pi_v<gate_layout>);

    gate_layout layout{{2, 2, 1}};

    CHECK(layout.is_empty());

    auto a = layout.create_pi("a", {0, 0});
    CHECK(layout.is_pi(layout.get_node(a)));

    CHECK(!layout.is_empty());

    CHECK(layout.size() == 3);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_gates() == 0);
    CHECK(layout.num_wires() == 1);
    CHECK(layout.num_crossings() == 0);

    CHECK(std::is_same_v<std::decay_t<decltype(a)>, gate_layout::signal>);

    auto b = layout.create_pi("b", {1, 0});
    auto c = layout.create_pi("c", {0, 1});

    CHECK(layout.is_pi(layout.get_node(b)));
    CHECK(layout.is_pi(layout.get_node(c)));

    CHECK(layout.num_pis() == 3);
    CHECK(layout.num_wires() == 3);
    CHECK(layout.num_crossings() == 0);

    CHECK(layout.pi_at(0) == layout.get_node(a));
    CHECK(layout.pi_at(1) == layout.get_node(b));
    CHECK(layout.pi_at(2) == layout.get_node(c));

    layout.foreach_pi(
        [&](gate_layout::node pi, auto i)
        {
            const auto check = [&layout, &pi](auto p, auto s)
            {
                auto t = layout.get_tile(pi);
                CHECK(t == p);
                auto n = layout.get_node(static_cast<mockturtle::signal<gate_layout>>(t));
                CHECK(n == layout.get_node(s));
                auto tn = layout.get_tile(n);
                CHECK(tn == t);
            };

            CHECK(layout.is_pi(pi));
            CHECK(layout.is_pi_tile(layout.get_tile(pi)));
            CHECK(!layout.is_gate_tile(static_cast<tile<gate_layout>>(pi)));
            CHECK(!layout.is_gate(layout.get_node(static_cast<tile<gate_layout>>(pi))));

            switch (i)
            {
                case 0:
                {
                    check(tile<gate_layout>{0, 0}, a);

                    break;
                }

                case 1:
                {
                    check(tile<gate_layout>{1, 0}, b);

                    break;
                }
                case 2:
                {
                    check(tile<gate_layout>{0, 1}, c);

                    break;
                }
                default:
                {
                    CHECK(false);
                }
            }
        });
}

TEST_CASE("Creation and usage of primary outputs", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_create_po_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pis_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pos_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_po_v<gate_layout>);

    gate_layout layout{{2, 2, 1}};

    const auto x1 = layout.create_pi("x1", tile<gate_layout>{0, 0});

    CHECK(layout.size() == 3);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_pos() == 0);

    const auto f1 = layout.create_po(x1, "f1", tile<gate_layout>{0, 1});
    const auto f2 = layout.create_po(x1, "f2", tile<gate_layout>{1, 1});

    CHECK(layout.is_po(layout.get_node(f1)));
    CHECK(layout.is_po(layout.get_node(f2)));

    CHECK(layout.is_empty_tile({1, 0}));

    CHECK(layout.size() == 5);
    CHECK(layout.num_pos() == 2);
    CHECK(layout.num_wires() == 3);
    CHECK(layout.num_crossings() == 0);

    CHECK(layout.po_at(0) == f1);
    CHECK(layout.po_at(1) == f2);
    CHECK(layout.po_at(0) != layout.po_at(1));

    layout.foreach_po(
        [&](auto po, auto i)
        {
            const auto check = [&layout, &po](auto c)
            {
                CHECK(static_cast<tile<gate_layout>>(po) == c);
                auto n  = layout.get_node(po);
                auto tn = layout.get_tile(n);
                CHECK(tn == static_cast<tile<gate_layout>>(po));
            };

            CHECK(layout.is_po(layout.get_node(po)));
            CHECK(layout.is_po_tile(static_cast<tile<gate_layout>>(po)));
            CHECK(layout.is_gate_tile(static_cast<tile<gate_layout>>(po)));
            CHECK(layout.is_gate(layout.get_node(static_cast<tile<gate_layout>>(po))));

            switch (i)
            {
                case 0:
                {
                    check(tile<gate_layout>{0, 1});

                    break;
                }
                case 1:
                {
                    check(tile<gate_layout>{1, 1});

                    break;
                }
                default:
                {
                    CHECK(false);
                }
            }
        });
}

TEST_CASE("Node names", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::or_not_gate_layout<gate_layout>();

    CHECK(layout.has_name(layout.get_node({1, 0})));
    CHECK(layout.has_name(layout.get_node({0, 1})));
    CHECK(layout.has_name(layout.get_node({2, 2})));

    CHECK(!layout.has_name(layout.get_node({0, 0})));
    CHECK(!layout.has_name(layout.get_node({1, 1})));
    CHECK(!layout.has_name(layout.get_node({1, 2})));

    CHECK(layout.has_input_name(0));
    CHECK(layout.has_input_name(1));
    CHECK(!layout.has_input_name(2));
    CHECK(!layout.has_input_name(3));

    CHECK(layout.has_output_name(0));
    CHECK(!layout.has_output_name(1));

    CHECK(layout.get_name(layout.get_node({1, 0})) == "x1");
    CHECK(layout.get_name(layout.get_node({0, 1})) == "x2");
    CHECK(layout.get_name(layout.get_node({2, 2})) == "f1");
    CHECK(layout.get_name(layout.get_node({0, 0})).empty());

    CHECK(layout.get_input_name(0) == "x1");
    CHECK(layout.get_input_name(1) == "x2");
    CHECK(layout.get_output_name(0) == "f1");

    layout.set_name(layout.get_node({1, 1}), "or");

    CHECK(layout.has_name(layout.get_node({1, 1})));
    CHECK(layout.get_name(layout.get_node({1, 1})) == "or");

    layout.set_name(layout.get_node({2, 2}), "");

    CHECK(!layout.has_name(layout.get_node({2, 2})));
    CHECK(!layout.has_output_name(0));
}

TEST_CASE("Creation of unary operations", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_create_buf_v<gate_layout>);
    REQUIRE(mockturtle::has_create_not_v<gate_layout>);

    gate_layout layout{{2, 2, 1}};

    CHECK(layout.is_empty());

    auto x1 = layout.create_pi("x1", {0, 0});

    CHECK(layout.size() == 3);

    auto f1 = layout.create_buf(x1, {1, 0});
    auto f2 = layout.create_not(x1, {0, 1});

    CHECK(layout.size() == 5);
    CHECK(layout.num_gates() == 1);
    CHECK(layout.num_wires() == 2);
    CHECK(layout.num_crossings() == 0);

    auto x2 = layout.create_pi("x2", {1, 1});
    CHECK(layout.is_pi(layout.get_node(x2)));

    auto f1n  = layout.get_node(f1);
    auto t10n = layout.get_node(static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{1, 0}));
    CHECK(f1n == t10n);

    auto f2n  = layout.get_node(f2);
    auto t01n = layout.get_node(static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{0, 1}));
    CHECK(f2n == t01n);

    CHECK(!layout.is_empty());

    CHECK(!layout.is_empty_tile({1, 0}));
    CHECK(layout.is_gate_tile({1, 0}));
    CHECK(layout.is_gate(layout.get_node({1, 0})));
    CHECK(layout.is_wire_tile({1, 0}));
    CHECK(layout.is_wire(layout.get_node({1, 0})));

    CHECK(!layout.is_empty_tile({0, 1}));
    CHECK(layout.is_gate_tile({0, 1}));
    CHECK(layout.is_gate(layout.get_node({0, 1})));
    CHECK(!layout.is_wire_tile({0, 1}));
    CHECK(!layout.is_wire(layout.get_node({0, 1})));
}

TEST_CASE("Creation of binary operations", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_create_and_v<gate_layout>);
    REQUIRE(mockturtle::has_create_or_v<gate_layout>);
    REQUIRE(mockturtle::has_create_nand_v<gate_layout>);
    REQUIRE(mockturtle::has_create_nor_v<gate_layout>);
    REQUIRE(mockturtle::has_create_xor_v<gate_layout>);

    gate_layout layout{{2, 2, 1}};

    auto x1 = layout.create_pi("x1", {1, 0});
    auto x2 = layout.create_pi("x2", {0, 1});

    CHECK(layout.num_pis() == 2);

    auto a = layout.create_and(x1, x2, {0, 0});
    auto o = layout.create_or(x1, x2, {1, 1});

    const auto a_node = layout.get_node(a);
    const auto o_node = layout.get_node(o);

    CHECK(a != o);
    CHECK(layout.num_gates() == 2);
    CHECK(layout.size() == 6);

    CHECK(layout.get_node({0, 0}) == layout.get_node(a));
    CHECK(layout.get_node({1, 1}) == layout.get_node(o));

    CHECK(layout.is_gate_tile({0, 0}));
    CHECK(layout.is_gate_tile({1, 1}));

    CHECK(layout.is_gate(layout.get_node({0, 0})));
    CHECK(layout.is_gate(layout.get_node({1, 1})));

    CHECK(!layout.is_wire_tile({0, 0}));
    CHECK(!layout.is_wire_tile({1, 1}));

    CHECK(!layout.is_wire(layout.get_node({0, 0})));
    CHECK(!layout.is_wire(layout.get_node({1, 1})));

    auto na = layout.create_nand(x1, x2, {0, 0});
    auto no = layout.create_nor(x1, x2, {1, 1});

    const auto na_node = layout.get_node(na);
    const auto no_node = layout.get_node(no);

    CHECK(na != no);
    CHECK(na == a);
    CHECK(no == o);
    CHECK(layout.num_gates() == 2);
    CHECK(layout.size() == 8);  // overridden nodes still count towards layout size

    CHECK(layout.get_node({0, 0}) == layout.get_node(na));
    CHECK(layout.get_node({1, 1}) == layout.get_node(no));

    CHECK(layout.is_gate_tile({0, 0}));
    CHECK(layout.is_gate_tile({1, 1}));

    CHECK(layout.is_gate(layout.get_node({0, 0})));
    CHECK(layout.is_gate(layout.get_node({1, 1})));

    CHECK(!layout.is_wire_tile({0, 0}));
    CHECK(!layout.is_wire_tile({1, 1}));

    CHECK(!layout.is_wire(layout.get_node({0, 0})));
    CHECK(!layout.is_wire(layout.get_node({1, 1})));

    CHECK(layout.is_dead(a_node));
    CHECK(layout.is_dead(o_node));

    auto xo = layout.create_nand(x1, x2, {0, 0});
    auto xn = layout.create_nor(x1, x2, {1, 1});

    const auto xo_node = layout.get_node(xo);
    const auto xn_node = layout.get_node(xn);

    CHECK(xo != xn);
    CHECK(xo == a);
    CHECK(xn == o);
    CHECK(layout.num_gates() == 2);
    CHECK(layout.size() == 10);  // overridden nodes still count towards layout size

    CHECK(layout.get_node({0, 0}) == layout.get_node(xo));
    CHECK(layout.get_node({1, 1}) == layout.get_node(xn));

    CHECK(layout.is_gate_tile({0, 0}));
    CHECK(layout.is_gate_tile({1, 1}));

    CHECK(layout.is_gate(layout.get_node({0, 0})));
    CHECK(layout.is_gate(layout.get_node({1, 1})));

    CHECK(!layout.is_wire_tile({0, 0}));
    CHECK(!layout.is_wire_tile({1, 1}));

    CHECK(!layout.is_wire(layout.get_node({0, 0})));
    CHECK(!layout.is_wire(layout.get_node({1, 1})));

    CHECK(layout.is_dead(na_node));
    CHECK(layout.is_dead(no_node));

    CHECK(!layout.is_dead(xo_node));
    CHECK(!layout.is_dead(xn_node));
}

TEST_CASE("Creation of ternary operations", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::is_network_type_v<gate_layout>);
    REQUIRE(mockturtle::has_create_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pis_v<gate_layout>);
    REQUIRE(mockturtle::has_create_maj_v<gate_layout>);

    gate_layout layout{{2, 3, 1}};

    auto x1 = layout.create_pi("x1", {1, 0});
    auto x2 = layout.create_pi("x2", {0, 1});
    auto x3 = layout.create_pi("x3", {1, 2});

    CHECK(layout.num_pis() == 3);

    auto m = layout.create_maj(x1, x2, x3, {1, 1});

    CHECK(x1 != m);
    CHECK(x2 != m);
    CHECK(x3 != m);

    CHECK(layout.num_gates() == 1);
    CHECK(layout.size() == 6);

    CHECK(layout.get_node({1, 1}) == layout.get_node(m));

    CHECK(layout.is_gate_tile({1, 1}));
    CHECK(layout.is_gate(layout.get_node({1, 1})));
    CHECK(!layout.is_wire_tile({1, 1}));
    CHECK(!layout.is_wire(layout.get_node({1, 1})));
}

TEST_CASE("compute functions from AND and NOT gates", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_compute_v<gate_layout, kitty::dynamic_truth_table>);

    gate_layout layout{{3, 1, 0}, open_clocking<gate_layout>(num_clks::FOUR)};

    layout.assign_clock_number({2, 0}, static_cast<typename gate_layout::clock_number_t>(0));
    layout.assign_clock_number({1, 0}, static_cast<typename gate_layout::clock_number_t>(1));
    layout.assign_clock_number({0, 0}, static_cast<typename gate_layout::clock_number_t>(2));

    layout.assign_clock_number({1, 1}, static_cast<typename gate_layout::clock_number_t>(0));
    layout.assign_clock_number({2, 1}, static_cast<typename gate_layout::clock_number_t>(1));
    layout.assign_clock_number({3, 1}, static_cast<typename gate_layout::clock_number_t>(2));

    const auto x1 = layout.create_pi("x1", {2, 0});
    const auto x2 = layout.create_pi("x2", {1, 1});
    const auto a1 = layout.create_and(x1, x2, {1, 0});
    const auto n1 = layout.create_not(x2, {2, 1});
    layout.create_po(a1, "f1", {0, 0});
    layout.create_po(n1, "f2", {3, 1});

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);

    const auto sim_n1 = layout.compute(layout.get_node(n1), xs.begin(), xs.begin() + 1);
    const auto sim_a1 = layout.compute(layout.get_node(a1), xs.begin(), xs.end());

    CHECK(sim_n1 == ~xs[0]);
    CHECK(sim_a1 == (xs[0] & xs[1]));
}

TEST_CASE("create nodes and compute their functions", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_compute_v<gate_layout, kitty::dynamic_truth_table>);

    auto layout = blueprints::xor_maj_gate_layout<gate_layout>();

    const kitty::dynamic_truth_table tt_const0(0u);

    const auto const0 = layout.create_node({}, tt_const0);
    const auto const1 = layout.create_node({}, ~tt_const0);
    CHECK(const0 == layout.get_constant(false));
    CHECK(const1 == layout.get_constant(true));

    CHECK(layout.size() == 9);

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);
    kitty::create_nth_var(xs[2], 2);

    const auto sim_maj = layout.compute(layout.get_node({2, 1}), xs.begin(), xs.end());
    const auto sim_xor = layout.compute(layout.get_node({1, 0}), xs.begin(), xs.begin() + 2);

    CHECK(sim_maj == kitty::ternary_majority(xs[0], xs[1], xs[2]));
    CHECK(sim_xor == (xs[0] ^ xs[1]));
}

TEST_CASE("node and signal iteration", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_foreach_node_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_pi_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_po_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_fanin_v<gate_layout>);
    REQUIRE(mockturtle::has_foreach_fanout_v<gate_layout>);

    auto layout = blueprints::and_or_gate_layout<gate_layout>();

    const auto a = tile<gate_layout>{1, 0};
    const auto o = tile<gate_layout>{2, 1};

    CHECK(layout.size() == 8);

    /* iterate over nodes */
    uint32_t mask{0}, counter{0};
    layout.foreach_node(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 255);
    CHECK(counter == 28);

    mask = 0;
    layout.foreach_node([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 255);

    mask = counter = 0;
    layout.foreach_node(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 1);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_node(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 1);

    /* iterate over gates */
    mask = counter = 0;
    layout.foreach_gate(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 240);
    CHECK(counter == 6);

    mask = 0;
    layout.foreach_gate([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 240);

    mask = counter = 0;
    layout.foreach_gate(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 16);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_gate(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 16);

    /* iterate over wires */
    mask = counter = 0;
    layout.foreach_wire(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 204);
    CHECK(counter == 6);

    mask = 0;
    layout.foreach_wire([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 204);

    mask = counter = 0;
    layout.foreach_wire(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 4);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_wire(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 4);

    /* iterate over PIs */
    mask = counter = 0;
    layout.foreach_pi(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 12);
    CHECK(counter == 1);

    mask = 0;
    layout.foreach_pi([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 12);

    mask = counter = 0;
    layout.foreach_pi(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 4);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_pi(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 4);

    /* iterate over POs */
    mask = counter = 0;
    layout.foreach_po(
        [&](auto s, auto i)
        {
            mask |= (1u << layout.get_node(s));
            counter += i;
        });
    CHECK(mask == 192);
    CHECK(counter == 1);

    mask = 0;
    layout.foreach_po([&](auto s) { mask |= (1u << layout.get_node(s)); });
    CHECK(mask == 192);

    mask = counter = 0;
    layout.foreach_po(
        [&](auto s, auto i)
        {
            mask |= (1u << layout.get_node(s));
            counter += i;
            return false;
        });
    CHECK(mask == 64);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_po(
        [&](auto s)
        {
            mask |= (1u << layout.get_node(s));
            return false;
        });
    CHECK(mask == 64);

    /* iterate over fanins */
    mask = counter = 0;
    layout.foreach_fanin(layout.get_node(a),
                         [&](auto s, auto i)
                         {
                             mask |= (1u << layout.get_node(s));
                             counter += i;
                         });
    CHECK(mask == 12);
    CHECK(counter == 1);

    mask = 0;
    layout.foreach_fanin(layout.get_node(a), [&](auto s) { mask |= (1u << layout.get_node(s)); });
    CHECK(mask == 12);

    mask = counter = 0;
    layout.foreach_fanin(layout.get_node(a),
                         [&](auto s, auto i)
                         {
                             mask |= (1u << layout.get_node(s));
                             counter += i;
                             return false;
                         });
    CHECK(mask == 4);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_fanin(layout.get_node(a),
                         [&](auto s)
                         {
                             mask |= (1u << layout.get_node(s));
                             return false;
                         });
    CHECK(mask == 4);

    /* iterate over fanouts */
    mask = counter = 0;
    layout.foreach_fanout(layout.get_node(o),
                          [&](auto fon, auto i)
                          {
                              mask |= (1u << fon);
                              counter += i;
                          });
    CHECK(mask == 128);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_fanout(layout.get_node(o), [&](auto fon) { mask |= (1u << fon); });
    CHECK(mask == 128);

    mask = counter = 0;
    layout.foreach_fanout(layout.get_node(o),
                          [&](auto fon, auto i)
                          {
                              mask |= (1u << fon);
                              counter += i;
                              return false;
                          });
    CHECK(mask == 128);
    CHECK(counter == 0);

    mask = 0;
    layout.foreach_fanout(layout.get_node(o),
                          [&](auto fon)
                          {
                              mask |= (1u << fon);
                              return false;
                          });
    CHECK(mask == 128);
}

TEST_CASE("Iteration disrespecting clocking", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::and_not_gate_layout<gate_layout>();

    // remove clocking
    layout.foreach_tile([&layout](const auto& t) { layout.assign_clock_number(t, 0); });

    CHECK(layout.fanin_size<true>(layout.get_node({2, 0})) == 0);
    CHECK(layout.fanin_size<false>(layout.get_node({2, 0})) == 0);
    CHECK(layout.fanout_size<true>(layout.get_node({2, 0})) == 0);
    CHECK(layout.fanout_size<false>(layout.get_node({2, 0})) == 1);

    CHECK(layout.fanin_size<true>(layout.get_node({1, 1})) == 0);
    CHECK(layout.fanin_size<false>(layout.get_node({1, 1})) == 0);
    CHECK(layout.fanout_size<true>(layout.get_node({1, 1})) == 0);
    CHECK(layout.fanout_size<false>(layout.get_node({1, 1})) == 2);

    CHECK(layout.fanin_size<true>(layout.get_node({1, 0})) == 0);
    CHECK(layout.fanin_size<false>(layout.get_node({1, 0})) == 2);
    CHECK(layout.fanout_size<true>(layout.get_node({1, 0})) == 0);
    CHECK(layout.fanout_size<false>(layout.get_node({1, 0})) == 1);

    CHECK(layout.fanin_size<true>(layout.get_node({2, 1})) == 0);
    CHECK(layout.fanin_size<false>(layout.get_node({2, 1})) == 1);
    CHECK(layout.fanout_size<true>(layout.get_node({2, 1})) == 0);
    CHECK(layout.fanout_size<false>(layout.get_node({2, 1})) == 1);

    CHECK(layout.fanin_size<true>(layout.get_node({0, 0})) == 0);
    CHECK(layout.fanin_size<false>(layout.get_node({0, 0})) == 1);
    CHECK(layout.fanout_size<true>(layout.get_node({0, 0})) == 0);
    CHECK(layout.fanout_size<false>(layout.get_node({0, 0})) == 0);

    CHECK(layout.fanin_size<true>(layout.get_node({3, 1})) == 0);
    CHECK(layout.fanin_size<false>(layout.get_node({3, 1})) == 1);
    CHECK(layout.fanout_size<true>(layout.get_node({3, 1})) == 0);
    CHECK(layout.fanout_size<false>(layout.get_node({3, 1})) == 0);
}

TEST_CASE("Gate-level layout properties", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_size_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pis_v<gate_layout>);
    REQUIRE(mockturtle::has_num_pos_v<gate_layout>);
    REQUIRE(mockturtle::has_num_gates_v<gate_layout>);
    REQUIRE(mockturtle::has_fanin_size_v<gate_layout>);
    REQUIRE(mockturtle::has_fanout_size_v<gate_layout>);

    auto layout = blueprints::and_not_gate_layout<gate_layout>();

    const auto x1 = tile<gate_layout>{2, 0};
    const auto x2 = tile<gate_layout>{1, 1};
    const auto a1 = tile<gate_layout>{1, 0};
    const auto n1 = tile<gate_layout>{2, 1};
    const auto f1 = tile<gate_layout>{0, 0};
    const auto f2 = tile<gate_layout>{3, 1};

    CHECK(layout.size() == 8);
    CHECK(layout.num_pis() == 2);
    CHECK(layout.num_pos() == 2);
    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.fanin_size(layout.get_node(x1)) == 0);
    CHECK(layout.fanin_size(layout.get_node(x2)) == 0);
    CHECK(layout.fanin_size(layout.get_node(a1)) == 2);
    CHECK(layout.fanin_size(layout.get_node(n1)) == 1);
    CHECK(layout.fanin_size(layout.get_node(f1)) == 1);
    CHECK(layout.fanin_size(layout.get_node(f2)) == 1);
    CHECK(layout.fanout_size(layout.get_node(x1)) == 1);
    CHECK(layout.fanout_size(layout.get_node(x2)) == 2);
    CHECK(layout.fanout_size(layout.get_node(a1)) == 1);
    CHECK(layout.fanout_size(layout.get_node(n1)) == 1);
    CHECK(layout.fanout_size(layout.get_node(f1)) == 0);
    CHECK(layout.fanout_size(layout.get_node(f2)) == 0);
}

TEST_CASE("Functional properties", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_is_and_v<gate_layout>);
    REQUIRE(mockturtle::has_is_or_v<gate_layout>);
    REQUIRE(fiction::has_is_nand_v<gate_layout>);
    REQUIRE(fiction::has_is_nor_v<gate_layout>);
    REQUIRE(mockturtle::has_is_maj_v<gate_layout>);
    REQUIRE(mockturtle::has_is_xor_v<gate_layout>);
    REQUIRE(fiction::has_is_lt_v<gate_layout>);
    REQUIRE(fiction::has_is_le_v<gate_layout>);
    REQUIRE(fiction::has_is_gt_v<gate_layout>);
    REQUIRE(fiction::has_is_ge_v<gate_layout>);
    REQUIRE(mockturtle::has_is_function_v<gate_layout>);

    auto layout = blueprints::non_structural_all_function_gate_layout<gate_layout>();

    const auto x1 = tile<gate_layout>{0, 0};
    const auto x2 = tile<gate_layout>{1, 0};
    const auto x3 = tile<gate_layout>{2, 0};

    const auto a = tile<gate_layout>{0, 1};
    const auto o = tile<gate_layout>{1, 1};
    const auto x = tile<gate_layout>{2, 1};

    const auto m = tile<gate_layout>{0, 2};
    const auto f = tile<gate_layout>{1, 2};
    const auto w = tile<gate_layout>{2, 2};

    const auto n  = tile<gate_layout>{2, 3};
    const auto po = tile<gate_layout>{1, 3};

    const auto na = tile<gate_layout>{0, 4};
    const auto no = tile<gate_layout>{1, 4};

    const auto lt = tile<gate_layout>{0, 5};
    const auto le = tile<gate_layout>{1, 5};
    const auto gt = tile<gate_layout>{2, 5};
    const auto ge = tile<gate_layout>{3, 5};

    CHECK(layout.is_pi(layout.get_node(x1)));
    CHECK(layout.is_pi(layout.get_node(x2)));
    CHECK(layout.is_pi(layout.get_node(x3)));

    CHECK(layout.is_and(layout.get_node(a)));
    CHECK(layout.is_or(layout.get_node(o)));
    CHECK(layout.is_xor(layout.get_node(x)));

    CHECK(layout.is_maj(layout.get_node(m)));
    CHECK(layout.is_fanout(layout.get_node(f)));
    CHECK(layout.is_wire(layout.get_node(w)));
    CHECK(!layout.is_inv(layout.get_node(w)));

    CHECK(layout.is_inv(layout.get_node(n)));
    CHECK(!layout.is_wire(layout.get_node(n)));
    CHECK(layout.is_po(layout.get_node(po)));

    CHECK(layout.is_nand(layout.get_node(na)));
    CHECK(layout.is_nor(layout.get_node(no)));

    CHECK(layout.is_lt(layout.get_node(lt)));
    CHECK(layout.is_le(layout.get_node(le)));
    CHECK(layout.is_gt(layout.get_node(gt)));
    CHECK(layout.is_ge(layout.get_node(ge)));
}

TEST_CASE("Custom node values", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_clear_values_v<gate_layout>);
    REQUIRE(mockturtle::has_value_v<gate_layout>);
    REQUIRE(mockturtle::has_set_value_v<gate_layout>);
    REQUIRE(mockturtle::has_incr_value_v<gate_layout>);
    REQUIRE(mockturtle::has_decr_value_v<gate_layout>);

    auto layout = blueprints::and_or_gate_layout<gate_layout>();

    CHECK(layout.size() == 8);

    layout.clear_values();
    layout.foreach_node(
        [&](auto n)
        {
            CHECK(layout.value(n) == 0);
            layout.set_value(n, static_cast<uint32_t>(n));
            CHECK(layout.value(n) == n);
            CHECK(layout.incr_value(n) == n);
            CHECK(layout.value(n) == n + 1);
            CHECK(layout.decr_value(n) == n);
            CHECK(layout.value(n) == n);
        });
    layout.clear_values();
    layout.foreach_node([&](auto n) { CHECK(layout.value(n) == 0); });
}

TEST_CASE("Visited values", "[gate-level-layout]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_clear_visited_v<gate_layout>);
    REQUIRE(mockturtle::has_visited_v<gate_layout>);
    REQUIRE(mockturtle::has_set_visited_v<gate_layout>);

    auto layout = blueprints::and_or_gate_layout<gate_layout>();

    CHECK(layout.size() == 8);

    layout.clear_visited();
    layout.foreach_node(
        [&](auto n)
        {
            CHECK(layout.visited(n) == 0);
            layout.set_visited(n, static_cast<uint32_t>(n));
            CHECK(layout.visited(n) == n);
        });
    layout.clear_visited();
    layout.foreach_node([&](auto n) { CHECK(layout.visited(n) == 0); });
}

TEST_CASE("Crossings", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::crossing_layout<gate_layout>();

    CHECK(layout.num_crossings() == 1);
    CHECK(layout.fanout_size(layout.get_node({1, 1})) == 1);
    CHECK(layout.fanout_size(layout.get_node({2, 1})) == 1);
    CHECK(layout.fanout_size(layout.get_node({2, 1, 1})) == 1);
    CHECK(layout.fanin_size(layout.get_node({2, 1})) == 1);
    CHECK(layout.fanin_size(layout.get_node({2, 1, 1})) == 1);
    CHECK(layout.fanin_size(layout.get_node({3, 1})) == 1);
    CHECK(layout.fanin_size(layout.get_node({2, 2})) == 2);

    layout.foreach_fanout(layout.get_node({1, 1}),
                          [&layout](const auto& fon) { CHECK(fon == layout.get_node({2, 1, 1})); });

    layout.foreach_fanout(layout.get_node({2, 1}),
                          [&layout](const auto& fon) { CHECK(fon == layout.get_node({2, 2})); });

    layout.foreach_fanout(layout.get_node({2, 1, 1}),
                          [&layout](const auto& fon) { CHECK(fon == layout.get_node({3, 1})); });

    layout.foreach_fanin(layout.get_node({2, 1}),
                         [&layout](const auto& fi) { CHECK(layout.get_node(fi) == layout.get_node({2, 0})); });

    layout.foreach_fanin(layout.get_node({2, 1, 1}),
                         [&layout](const auto& fi) { CHECK(layout.get_node(fi) == layout.get_node({1, 1})); });

    layout.foreach_fanin(layout.get_node({3, 1}),
                         [&layout](const auto& fi) { CHECK(layout.get_node(fi) == layout.get_node({2, 1, 1})); });
}

TEST_CASE("Move nodes", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::and_or_gate_layout<gate_layout>();

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);

    auto and_node = layout.get_node({1, 0});
    auto or_node  = layout.get_node({2, 1});

    // switch AND and OR

    // move OR out of the way
    layout.move_node(or_node, {3, 0}, {});

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);

    // move AND where OR was
    layout.move_node(and_node, {2, 1},
                     {{static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{2, 0}),
                       static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{1, 1})}});

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);

    // move OR where AND was
    layout.move_node(or_node, {1, 0},
                     {{static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{2, 0}),
                       static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{1, 1})}});

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);

    CHECK(!layout.is_dead(and_node));
    CHECK(!layout.is_dead(or_node));

    CHECK(and_node == layout.get_node({2, 1}));
    CHECK(layout.is_and(layout.get_node({2, 1})));
    CHECK(or_node == layout.get_node({1, 0}));
    CHECK(layout.is_or(layout.get_node({1, 0})));

    layout.foreach_fanin(and_node,
                         [](const auto& f)
                         {
                             CHECK(((static_cast<tile<gate_layout>>(f) == tile<gate_layout>{2, 0}) ||
                                    (static_cast<tile<gate_layout>>(f) == tile<gate_layout>{1, 1})));
                         });

    layout.foreach_fanin(or_node,
                         [](const auto& f)
                         {
                             CHECK(((static_cast<tile<gate_layout>>(f) == tile<gate_layout>{2, 0}) ||
                                    (static_cast<tile<gate_layout>>(f) == tile<gate_layout>{1, 1})));
                         });

    layout.foreach_fanout(and_node,
                          [&layout](const auto& fon) { CHECK(layout.get_tile(fon) == tile<gate_layout>{3, 1}); });

    layout.foreach_fanout(or_node,
                          [&layout](const auto& fon) { CHECK(layout.get_tile(fon) == tile<gate_layout>{0, 0}); });

    // move PI

    auto pi_node = layout.get_node({2, 0});

    CHECK(layout.is_pi(pi_node));

    layout.move_node(pi_node, {3, 0}, {});

    CHECK(layout.is_pi(pi_node));

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);

    // move PO

    auto po_node = layout.get_node({0, 0});

    CHECK(layout.is_po(po_node));

    layout.move_node(po_node, {0, 1}, {static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{1, 0})});

    CHECK(layout.is_po(po_node));

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);

    // remove PO

    CHECK(layout.num_pos() == 2);

    layout.move_node(po_node, {}, {static_cast<mockturtle::signal<gate_layout>>(tile<gate_layout>{1, 0})});

    CHECK(layout.num_pos() == 1);

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 3);  // PO is gone now
    CHECK(layout.num_crossings() == 0);
}

TEST_CASE("Move crossing", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::crossing_layout<gate_layout>();

    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 9);
    CHECK(layout.num_crossings() == 1);
    CHECK(layout.num_pis() == 4);
    CHECK(layout.num_pos() == 2);

    auto crossing = layout.get_node({2, 1, 1});

    // move crossing to empty location
    layout.move_node(crossing, {3, 0}, {});
    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 9);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == 4);
    CHECK(layout.num_pos() == 2);

    crossing = layout.get_node({3, 0});

    // move crossing back
    layout.move_node(crossing, {2, 1, 1}, {});
    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 9);
    CHECK(layout.num_crossings() == 1);
    CHECK(layout.num_pis() == 4);
    CHECK(layout.num_pos() == 2);

    auto underneath_crossing = layout.get_node({2, 1, 0});

    // move crossing to empty location
    layout.move_node(underneath_crossing, {3, 0}, {});
    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 9);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == 4);
    CHECK(layout.num_pos() == 2);

    underneath_crossing = layout.get_node({3, 0});

    // move crossing back
    layout.move_node(underneath_crossing, {2, 1, 0}, {});
    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 9);
    CHECK(layout.num_crossings() == 1);
    CHECK(layout.num_pis() == 4);
    CHECK(layout.num_pos() == 2);
}

TEST_CASE("Clear tiles", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::and_or_gate_layout<gate_layout>();

    REQUIRE(layout.num_gates() == 2);
    REQUIRE(layout.num_wires() == 4);
    REQUIRE(layout.num_crossings() == 0);
    REQUIRE(layout.num_pis() == 2);
    REQUIRE(layout.num_pos() == 2);

    layout.clear_tile({1, 0});

    CHECK(!layout.is_gate_tile({1, 0}));
    CHECK(layout.num_gates() == 1);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == 2);
    CHECK(layout.num_pos() == 2);

    layout.clear_tile({2, 1});

    CHECK(!layout.is_gate_tile({2, 1}));
    CHECK(layout.num_gates() == 0);
    CHECK(layout.num_wires() == 4);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == 2);
    CHECK(layout.num_pos() == 2);

    layout.clear_tile({2, 0});

    CHECK(!layout.is_wire_tile({2, 0}));
    CHECK(!layout.is_pi_tile({2, 0}));
    CHECK(layout.num_gates() == 0);
    CHECK(layout.num_wires() == 3);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == 2);
    CHECK(layout.num_pos() == 2);

    layout.clear_tile({0, 0});

    CHECK(!layout.is_wire_tile({0, 0}));
    CHECK(!layout.is_po_tile({0, 0}));
    CHECK(layout.num_gates() == 0);
    CHECK(layout.num_wires() == 2);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == 2);
    CHECK(layout.num_pos() == 1);
}

TEST_CASE("Clear crossing", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::crossing_layout<gate_layout>();

    REQUIRE(layout.num_gates() == 2);
    REQUIRE(layout.num_wires() == 9);
    REQUIRE(layout.num_crossings() == 1);
    REQUIRE(layout.num_pis() == 4);
    REQUIRE(layout.num_pos() == 2);

    layout.clear_tile({2, 1, 1});

    CHECK(!layout.is_wire_tile({2, 1, 1}));
    CHECK(layout.is_empty_tile({2, 1, 1}));
    CHECK(layout.num_gates() == 2);
    CHECK(layout.num_wires() == 8);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == 4);
    CHECK(layout.num_pos() == 2);
}

TEST_CASE("Gate-level cardinal operations", "[gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::crossing_layout<gate_layout>();

    CHECK(layout.has_no_incoming_signal({0, 1}));
    CHECK(layout.has_no_incoming_signal({0, 2}));
    CHECK(layout.has_no_incoming_signal({1, 0}));
    CHECK(layout.has_no_incoming_signal({2, 0}));
    CHECK(layout.has_no_outgoing_signal({3, 1}));
    CHECK(layout.has_no_outgoing_signal({3, 2}));

    CHECK(layout.has_southern_outgoing_signal({1, 0}));
    CHECK(layout.has_southern_outgoing_signal({2, 0}));
    CHECK(layout.has_eastern_outgoing_signal({0, 1}));
    CHECK(layout.has_eastern_outgoing_signal({0, 2}));

    CHECK(layout.has_northern_incoming_signal({1, 1}));
    CHECK(layout.has_western_incoming_signal({1, 1}));
    CHECK(layout.has_eastern_outgoing_signal({1, 1}));

    CHECK(layout.has_eastern_outgoing_signal({2, 1, 1}));
    CHECK(layout.has_western_incoming_signal({2, 1, 1}));

    CHECK(layout.has_northern_incoming_signal({2, 1}));
    CHECK(layout.has_southern_outgoing_signal({2, 1}));

    CHECK(layout.has_eastern_outgoing_signal({1, 2}));
    CHECK(layout.has_western_incoming_signal({1, 2}));

    CHECK(layout.has_northern_incoming_signal({2, 2}));
    CHECK(layout.has_western_incoming_signal({2, 2}));
    CHECK(layout.has_eastern_outgoing_signal({2, 2}));

    CHECK(layout.has_western_incoming_signal({3, 1}));
    CHECK(layout.has_western_incoming_signal({3, 2}));
}
