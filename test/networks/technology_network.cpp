//
// Created by marcel on 20.05.21.
//

// This file is based on mockturtle/test/networks/klut.cpp

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/traits.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>
#include <kitty/operators.hpp>
#include <mockturtle/networks/sequential.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("create and use constants in a technology network", "[technology-network]")
{
    const technology_network tec{};

    CHECK(mockturtle::has_size_v<technology_network>);
    CHECK(mockturtle::has_get_constant_v<technology_network>);
    CHECK(mockturtle::has_is_constant_v<technology_network>);
    CHECK(mockturtle::has_is_pi_v<technology_network>);
    CHECK(mockturtle::has_is_constant_v<technology_network>);
    CHECK(mockturtle::has_get_node_v<technology_network>);
    CHECK(mockturtle::has_is_complemented_v<technology_network>);

    CHECK(tec.size() == 2);

    auto c0 = tec.get_constant(false);
    auto c1 = tec.get_constant(true);

    CHECK(tec.size() == 2);
    CHECK(c0 != c1);
    CHECK(tec.get_node(c0) == 0);
    CHECK(tec.get_node(c1) == 1);
    CHECK(!tec.is_complemented(c0));
    CHECK(!tec.is_complemented(c1));
    CHECK(tec.is_constant(c0));
    CHECK(tec.is_constant(c1));
    CHECK(!tec.is_pi(c0));
    CHECK(!tec.is_pi(c1));
}

TEST_CASE("create and use primary inputs in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_pi_v<technology_network>);
    CHECK(mockturtle::has_is_constant_v<technology_network>);
    CHECK(mockturtle::has_is_pi_v<technology_network>);
    CHECK(mockturtle::has_num_pis_v<technology_network>);

    CHECK(tec.num_pis() == 0);

    auto x1 = tec.create_pi();
    auto x2 = tec.create_pi();

    CHECK(tec.size() == 4);
    CHECK(tec.num_pis() == 2);
    CHECK(x1 != x2);
}

TEST_CASE("create and use primary outputs in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_po_v<technology_network>);
    CHECK(mockturtle::has_num_pos_v<technology_network>);

    auto c0 = tec.get_constant(false);
    auto c1 = tec.get_constant(true);
    auto x  = tec.create_pi();

    tec.create_po(c0);
    tec.create_po(c1);
    tec.create_po(x);

    CHECK(tec.size() == 3);
    CHECK(tec.num_pis() == 1);
    CHECK(tec.num_pos() == 3);
}

TEST_CASE("create and use register in a technology network", "[technology-network]")
{
    using sequential_technology_network = mockturtle::sequential<technology_network>;

    sequential_technology_network tec{};

    REQUIRE(mockturtle::has_foreach_po_v<sequential_technology_network>);
    REQUIRE(mockturtle::has_create_po_v<sequential_technology_network>);
    REQUIRE(mockturtle::has_create_pi_v<sequential_technology_network>);
    REQUIRE(mockturtle::has_create_ro_v<sequential_technology_network>);
    REQUIRE(mockturtle::has_create_ri_v<sequential_technology_network>);
    REQUIRE(mockturtle::has_create_maj_v<sequential_technology_network>);

    const auto c0 = tec.get_constant(false);
    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();

    CHECK(tec.size() == 6);
    CHECK(tec.num_registers() == 0);
    CHECK(tec.num_cis() == 4);
    CHECK(tec.num_cos() == 0);

    const auto f1 = tec.create_maj(x1, x2, x3);
    tec.create_po(f1);

    CHECK(tec.num_pos() == 1);

    const auto s1 = tec.create_ro();  // ntk. input
    tec.create_po(s1);                // po

    const auto f2 = tec.create_maj(f1, x4, c0);
    tec.create_ri(f2);  // ntk. output

    CHECK(tec.num_registers() == 1);
    CHECK(tec.num_cis() == 4 + 1);
    CHECK(tec.num_cos() == 2 + 1);

    tec.foreach_pi(
        [&](auto const& node, auto index)
        {
            CHECK(tec.is_pi(node));
            switch (index)
            {
                case 0:
                    CHECK(node == x1); /* first pi */
                    break;
                case 1:
                    CHECK(node == x2); /* second pi */
                    break;
                case 2:
                    CHECK(node == x3); /* third pi */
                    break;
                case 3:
                    CHECK(node == x4); /* fourth pi */
                    break;
                default: CHECK(false);
            }
        });

    tec.foreach_ci(
        [&](auto const& node, auto index)
        {
            CHECK(tec.is_ci(node));
            switch (index)
            {
                case 0:
                    CHECK(node == x1); /* first pi */
                    break;
                case 1:
                    CHECK(node == x2); /* second pi */
                    break;
                case 2:
                    CHECK(node == x3); /* third pi */
                    break;
                case 3:
                    CHECK(node == x4); /* fourth pi */
                    break;
                case 4:
                    CHECK(node == s1); /* first state-bit */
                    CHECK(tec.is_ci(node));
                    CHECK(!tec.is_pi(node));
                    break;
                default: CHECK(false);
            }
        });

    tec.foreach_po(
        [&](auto const& node, auto index)
        {
            switch (index)
            {
                case 0:
                    CHECK(node == f1); /* first po */
                    break;
                case 1:
                    CHECK(node == s1); /* second po */
                    break;
                default: CHECK(false);
            }
        });

    tec.foreach_co(
        [&](auto const& node, auto index)
        {
            switch (index)
            {
                case 0:
                    CHECK(node == f1); /* first po */
                    break;
                case 1:
                    CHECK(node == s1); /* second po */
                    break;
                case 2:
                    CHECK(node == f2); /* first next-state bit */
                    break;
                default: CHECK(false);
            }
        });
}

TEST_CASE("create unary operations in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_buf_v<technology_network>);
    CHECK(mockturtle::has_create_not_v<technology_network>);

    auto x1 = tec.create_pi();

    CHECK(tec.size() == 3);

    auto f1 = tec.create_buf(x1);
    auto f2 = tec.create_not(x1);

    CHECK(tec.size() == 5);
    CHECK(f1 != x1);
    CHECK(f2 != x1);
}

TEST_CASE("create binary operations in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_and_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    CHECK(tec.size() == 4);

    tec.create_and(x1, x2);
    CHECK(tec.size() == 5);

    tec.create_and(x1, x2);
    CHECK(tec.size() == 6);  // no structural hashing

    tec.create_and(x2, x1);
    CHECK(tec.size() == 7);  // no structural hashing
}

TEST_CASE("create ternary operations in a technology network", "[technology-network]")
{
    technology_network tec{};

    REQUIRE(mockturtle::has_create_maj_v<technology_network>);
    REQUIRE(fiction::has_create_dot_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();

    CHECK(tec.size() == 5);

    const auto f1 = tec.create_maj(x1, x2, x3);
    CHECK(tec.size() == 6);

    const auto f2 = tec.create_maj(x1, x2, tec.get_constant(false));
    CHECK(tec.size() == 7);
    CHECK(f1 != f2);

    const auto f3 = tec.create_maj(x1, x2, tec.get_constant(true));
    CHECK(tec.size() == 8);
    CHECK(f2 != f3);

    const auto f4 = tec.create_dot(x1, x2, x3);
    CHECK(tec.size() == 9);

    const auto f5 = tec.create_dot(x1, x2, tec.get_constant(false));
    CHECK(tec.size() == 10);
    CHECK(f4 != f5);

    const auto f6 = tec.create_dot(x1, x2, tec.get_constant(true));
    CHECK(tec.size() == 11);
    CHECK(f5 != f6);
}

TEST_CASE("create n-ary operations in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_and_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();

    CHECK(tec.size() == 7);

    tec.create_nary_and({x1, x2, x3});
    CHECK(tec.size() == 9);

    tec.create_nary_or({x2, x3, x4});
    CHECK(tec.size() == 11);

    tec.create_nary_xor({x3, x4, x5});
    CHECK(tec.size() == 13);
}

TEST_CASE("clone a node in a technology network", "[technology-network]")
{
    technology_network tec1, tec2;

    CHECK(mockturtle::has_clone_node_v<technology_network>);

    auto a1 = tec1.create_pi();
    auto b1 = tec1.create_pi();
    auto f1 = tec1.create_and(a1, b1);
    CHECK(tec1.size() == 5);

    auto a2 = tec2.create_pi();
    auto b2 = tec2.create_pi();
    CHECK(tec2.size() == 4);

    auto f2 = tec2.clone_node(tec1, tec1.get_node(f1), {a2, b2});
    CHECK(tec2.size() == 5);

    tec2.foreach_fanin(tec2.get_node(f2), [&](auto const& s) { CHECK(!tec2.is_complemented(s)); });
}

TEST_CASE("compute functions from AND and NOT gates in technology networks", "[technology-network]")
{
    technology_network tec{};

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();

    const auto f1 = tec.create_not(a);
    const auto f2 = tec.create_and(a, b);

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);

    const auto sim_f1 = tec.compute(tec.get_node(f1), xs.begin(), xs.begin() + 1);
    const auto sim_f2 = tec.compute(tec.get_node(f2), xs.begin(), xs.end());

    CHECK(sim_f1 == ~xs[0]);
    CHECK(sim_f2 == (xs[0] & xs[1]));
}

TEST_CASE("create nodes and compute a function in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_node_v<technology_network>);
    CHECK(mockturtle::has_compute_v<technology_network, kitty::dynamic_truth_table>);

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();
    const auto c = tec.create_pi();

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(3u), tt_const0(0u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "96");

    CHECK(tec.size() == 5);

    const auto const0 = tec.create_node({}, tt_const0);
    const auto const1 = tec.create_node({}, ~tt_const0);
    CHECK(const0 == tec.get_constant(false));
    CHECK(const1 == tec.get_constant(true));

    const auto maj_n = tec.create_node({a, b, c}, tt_maj);
    const auto xor_n = tec.create_node({a, b, c}, tt_xor);

    CHECK(tec.size() == 7);

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);
    kitty::create_nth_var(xs[2], 2);

    const auto sim_maj = tec.compute(tec.get_node(maj_n), xs.begin(), xs.end());
    const auto sim_xor = tec.compute(tec.get_node(xor_n), xs.begin(), xs.end());

    CHECK(sim_maj == kitty::ternary_majority(xs[0], xs[1], xs[2]));
    CHECK(sim_xor == (xs[0] ^ xs[1] ^ xs[2]));
}

TEST_CASE("create fanouts and nodes and compute a function in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_node_v<technology_network>);
    CHECK(mockturtle::has_compute_v<technology_network, kitty::dynamic_truth_table>);

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();
    const auto c = tec.create_pi();

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(3u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "96");

    const auto foa   = tec.create_buf(a);
    const auto fob   = tec.create_buf(b);
    const auto foc   = tec.create_buf(c);
    const auto maj_n = tec.create_node({foa, fob, foc}, tt_maj);
    const auto xor_n = tec.create_node({foa, fob, foc}, tt_xor);

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);
    kitty::create_nth_var(xs[2], 2);

    const auto sim_maj = tec.compute(tec.get_node(maj_n), xs.begin(), xs.end());
    const auto sim_xor = tec.compute(tec.get_node(xor_n), xs.begin(), xs.end());

    CHECK(sim_maj == kitty::ternary_majority(xs[0], xs[1], xs[2]));
    CHECK(sim_xor == (xs[0] ^ xs[1] ^ xs[2]));
}

TEST_CASE("hash nodes in technology network", "[technology-network]")
{
    technology_network tec{};

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();
    const auto c = tec.create_pi();

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(3u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "96");

    tec.create_node({a, b, c}, tt_maj);
    tec.create_node({a, b, c}, tt_xor);

    CHECK(tec.size() == 7);

    tec.create_node({a, b, c}, tt_maj);

    CHECK(tec.size() == 8);  // no structural hashing
}

TEST_CASE("subsitute node by another", "[technology-network]")
{
    technology_network tec{};

    const auto c0 = tec.get_node(tec.get_constant(false));
    const auto c1 = tec.get_node(tec.get_constant(true));
    const auto a  = tec.create_pi();
    const auto b  = tec.create_pi();

    kitty::dynamic_truth_table tt_nand(2u), tt_le(2u), tt_ge(2u), tt_or(2u);
    kitty::create_from_hex_string(tt_nand, "7");
    kitty::create_from_hex_string(tt_le, "2");
    kitty::create_from_hex_string(tt_ge, "4");
    kitty::create_from_hex_string(tt_or, "e");

    // XOR with NAND
    const auto n1 = tec.create_node({a, b}, tt_nand);
    const auto n2 = tec.create_node({a, n1}, tt_nand);
    const auto n3 = tec.create_node({b, n1}, tt_nand);
    const auto n4 = tec.create_node({n2, n3}, tt_nand);
    tec.create_po(n4);

    std::vector<mockturtle::node<technology_network>> nodes;
    tec.foreach_node([&](auto node) { nodes.push_back(node); });

    CHECK(nodes == std::vector<technology_network::node>{c0, c1, a, b, n1, n2, n3, n4});
    CHECK(tec.fanout_size(n4) == 1);
    tec.foreach_po(
        [&](auto f)
        {
            CHECK(f == n4);
            return false;
        });

    // XOR with AND and OR
    const auto n5 = tec.create_node({a, b}, tt_le);
    const auto n6 = tec.create_node({a, b}, tt_ge);
    const auto n7 = tec.create_node({n5, n6}, tt_or);

    nodes.clear();
    tec.foreach_node([&](auto node) { nodes.push_back(node); });

    CHECK(nodes == std::vector{c0, c1, a, b, n1, n2, n3, n4, n5, n6, n7});
    CHECK(tec.fanout_size(n7) == 0);

    // substitute nodes
    tec.substitute_node(n4, n7);

    CHECK(tec.size() == 11);
    CHECK(tec.fanout_size(n4) == 0);
    CHECK(tec.fanout_size(n7) == 1);
    tec.foreach_po(
        [&](auto f)
        {
            CHECK(f == n7);
            return false;
        });
}

TEST_CASE("structural properties of a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_size_v<technology_network>);
    CHECK(mockturtle::has_num_pis_v<technology_network>);
    CHECK(mockturtle::has_num_pos_v<technology_network>);
    CHECK(mockturtle::has_num_gates_v<technology_network>);
    CHECK(mockturtle::has_fanin_size_v<technology_network>);
    CHECK(mockturtle::has_fanout_size_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    const auto f1 = tec.create_and(x1, x2);
    const auto f2 = tec.create_and(x2, x1);

    tec.create_po(f1);
    tec.create_po(f2);

    CHECK(tec.size() == 6);
    CHECK(tec.num_pis() == 2);
    CHECK(tec.num_pos() == 2);
    CHECK(tec.num_gates() == 2);
    CHECK(tec.fanin_size(tec.get_node(x1)) == 0);
    CHECK(tec.fanin_size(tec.get_node(x2)) == 0);
    CHECK(tec.fanin_size(tec.get_node(f1)) == 2);
    CHECK(tec.fanin_size(tec.get_node(f2)) == 2);
    CHECK(tec.fanout_size(tec.get_node(x1)) == 2);
    CHECK(tec.fanout_size(tec.get_node(x2)) == 2);
    CHECK(tec.fanout_size(tec.get_node(f1)) == 1);
    CHECK(tec.fanout_size(tec.get_node(f2)) == 1);
}

TEST_CASE("Node functions of a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_create_and_v<technology_network>);
    CHECK(mockturtle::has_create_nand_v<technology_network>);
    CHECK(mockturtle::has_create_or_v<technology_network>);
    CHECK(mockturtle::has_create_nor_v<technology_network>);
    CHECK(mockturtle::has_create_xor_v<technology_network>);
    CHECK(mockturtle::has_create_xnor_v<technology_network>);
    CHECK(mockturtle::has_create_lt_v<technology_network>);
    CHECK(mockturtle::has_create_le_v<technology_network>);
    CHECK(mockturtle::has_create_gt_v<technology_network>);
    CHECK(mockturtle::has_create_ge_v<technology_network>);
    CHECK(mockturtle::has_create_maj_v<technology_network>);
    CHECK(mockturtle::has_create_ite_v<technology_network>);
    CHECK(mockturtle::has_create_xor3_v<technology_network>);

    CHECK(mockturtle::has_is_and_v<technology_network>);
    CHECK(fiction::has_is_nand_v<technology_network>);
    CHECK(mockturtle::has_is_or_v<technology_network>);
    CHECK(fiction::has_is_nor_v<technology_network>);
    CHECK(mockturtle::has_is_xor_v<technology_network>);
    CHECK(fiction::has_is_xnor_v<technology_network>);
    CHECK(fiction::has_is_lt_v<technology_network>);
    CHECK(fiction::has_is_le_v<technology_network>);
    CHECK(fiction::has_is_gt_v<technology_network>);
    CHECK(fiction::has_is_ge_v<technology_network>);
    CHECK(mockturtle::has_is_maj_v<technology_network>);
    CHECK(mockturtle::has_is_ite_v<technology_network>);
    CHECK(mockturtle::has_is_xor3_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();

    const auto and_signal   = tec.create_and(x1, x2);
    const auto nand_signal  = tec.create_nand(x1, x2);
    const auto or_signal    = tec.create_or(x1, x2);
    const auto nor_signal   = tec.create_nor(x1, x2);
    const auto xor2_signal  = tec.create_xor(x1, x2);
    const auto xnor2_signal = tec.create_xnor(x1, x2);
    const auto lt_signal    = tec.create_lt(x1, x2);
    const auto le_signal    = tec.create_le(x1, x2);
    const auto gt_signal    = tec.create_gt(x1, x2);
    const auto ge_signal    = tec.create_ge(x1, x2);
    const auto maj_signal   = tec.create_maj(x1, x2, x3);
    const auto dot_signal   = tec.create_dot(x1, x2, x3);
    const auto ite_signal   = tec.create_ite(x1, x2, x3);
    const auto xor3_signal  = tec.create_xor3(x1, x2, x3);

    tec.create_po(and_signal);
    tec.create_po(nand_signal);
    tec.create_po(or_signal);
    tec.create_po(nor_signal);
    tec.create_po(xor2_signal);
    tec.create_po(xnor2_signal);
    tec.create_po(maj_signal);
    tec.create_po(dot_signal);
    tec.create_po(ite_signal);
    tec.create_po(xor3_signal);

    CHECK(tec.is_and(tec.get_node(and_signal)));
    CHECK(tec.is_nand(tec.get_node(nand_signal)));
    CHECK(tec.is_or(tec.get_node(or_signal)));
    CHECK(tec.is_nor(tec.get_node(nor_signal)));
    CHECK(tec.is_xor(tec.get_node(xor2_signal)));
    CHECK(tec.is_lt(tec.get_node(lt_signal)));
    CHECK(tec.is_le(tec.get_node(le_signal)));
    CHECK(tec.is_gt(tec.get_node(gt_signal)));
    CHECK(tec.is_ge(tec.get_node(ge_signal)));
    CHECK(tec.is_xnor(tec.get_node(xnor2_signal)));
    CHECK(tec.is_maj(tec.get_node(maj_signal)));
    CHECK(tec.is_dot(tec.get_node(dot_signal)));
    CHECK(tec.is_ite(tec.get_node(ite_signal)));
    CHECK(tec.is_xor3(tec.get_node(xor3_signal)));
}

TEST_CASE("node and signal iteration in a technology network", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_foreach_node_v<technology_network>);
    CHECK(mockturtle::has_foreach_pi_v<technology_network>);
    CHECK(mockturtle::has_foreach_po_v<technology_network>);
    CHECK(mockturtle::has_foreach_fanin_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto f1 = tec.create_and(x1, x2);
    const auto f2 = tec.create_and(x2, x1);
    tec.create_po(f1);
    tec.create_po(f2);

    CHECK(tec.size() == 6);

    /* iterate over nodes */
    uint32_t mask{0}, counter{0};
    tec.foreach_node(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 63);
    CHECK(counter == 15);

    mask = 0;
    tec.foreach_node([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 63);

    mask = counter = 0;
    tec.foreach_node(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 1);
    CHECK(counter == 0);

    mask = 0;
    tec.foreach_node(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 1);

    /* iterate over PIs */
    mask = counter = 0;
    tec.foreach_pi(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 12);
    CHECK(counter == 1);

    mask = 0;
    tec.foreach_pi([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 12);

    mask = counter = 0;
    tec.foreach_pi(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 4);
    CHECK(counter == 0);

    mask = 0;
    tec.foreach_pi(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 4);

    /* iterate over POs */
    mask = counter = 0;
    tec.foreach_po(
        [&](auto s, auto i)
        {
            mask |= (1u << tec.get_node(s));
            counter += i;
        });
    CHECK(mask == 48);
    CHECK(counter == 1);

    mask = 0;
    tec.foreach_po([&](auto s) { mask |= (1u << tec.get_node(s)); });
    CHECK(mask == 48);

    mask = counter = 0;
    tec.foreach_po(
        [&](auto s, auto i)
        {
            mask |= (1u << tec.get_node(s));
            counter += i;
            return false;
        });
    CHECK(mask == 16);
    CHECK(counter == 0);

    mask = 0;
    tec.foreach_po(
        [&](auto s)
        {
            mask |= (1u << tec.get_node(s));
            return false;
        });
    CHECK(mask == 16);
}

TEST_CASE("custom node values in technology networks", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_values_v<technology_network>);
    CHECK(mockturtle::has_value_v<technology_network>);
    CHECK(mockturtle::has_set_value_v<technology_network>);
    CHECK(mockturtle::has_incr_value_v<technology_network>);
    CHECK(mockturtle::has_decr_value_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto f1 = tec.create_and(x1, x2);
    const auto f2 = tec.create_and(x2, x1);
    tec.create_po(f1);
    tec.create_po(f2);

    CHECK(tec.size() == 6);

    tec.clear_values();
    tec.foreach_node(
        [&](auto n)
        {
            CHECK(tec.value(n) == 0);
            tec.set_value(n, static_cast<uint32_t>(n));
            CHECK(tec.value(n) == n);
            CHECK(tec.incr_value(n) == n);
            CHECK(tec.value(n) == n + 1);
            CHECK(tec.decr_value(n) == n);
            CHECK(tec.value(n) == n);
        });
    tec.clear_values();
    tec.foreach_node([&](auto n) { CHECK(tec.value(n) == 0); });
}

TEST_CASE("visited values in technology networks", "[technology-network]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto f1 = tec.create_and(x1, x2);
    const auto f2 = tec.create_and(x2, x1);
    tec.create_po(f1);
    tec.create_po(f2);

    CHECK(tec.size() == 6);

    tec.clear_visited();
    tec.foreach_node(
        [&](auto n)
        {
            CHECK(tec.visited(n) == 0);
            tec.set_visited(n, static_cast<uint32_t>(n));
            CHECK(tec.visited(n) == n);
        });
    tec.clear_visited();
    tec.foreach_node([&](auto n) { CHECK(tec.visited(n) == 0); });
}

TEST_CASE("substitute PO signals", "[technology-network]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();

    const auto a = tec.create_and(x1, x2);
    const auto i = tec.create_not(x3);
    const auto b = tec.create_buf(i);

    tec.create_po(a);
    tec.create_po(i);
    tec.create_po(b);

    CHECK(tec.size() == 8);

    CHECK(tec.is_po(a));
    CHECK(tec.is_po(i));
    CHECK(tec.is_po(b));

    CHECK(tec.fanout_size(a) == 1);
    CHECK(tec.fanout_size(i) == 2);
    CHECK(tec.fanout_size(b) == 1);

    tec.substitute_po_signals();

    CHECK(tec.size() == 10);
    CHECK(!tec.is_po(a));
    CHECK(!tec.is_po(i));
    CHECK(tec.is_po(b));  // b pointed to a buf already and, thus, wasn't substituted

    CHECK(tec.fanout_size(a) == 1);
    CHECK(tec.fanout_size(i) == 2);
    CHECK(tec.fanout_size(b) == 1);
}
