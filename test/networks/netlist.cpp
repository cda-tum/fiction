//
// Created by marcel on 21.10.25.
//

// This file is based on mockturtle/test/networks/klut.cpp

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/netlist.hpp>
#include <fiction/traits.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>
#include <kitty/operators.hpp>
#include <mockturtle/networks/sequential.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("create and use constants in a netlist", "[netlist]")
{
    const netlist ntl{};

    CHECK(mockturtle::has_size_v<netlist>);
    CHECK(mockturtle::has_get_constant_v<netlist>);
    CHECK(mockturtle::has_is_constant_v<netlist>);
    CHECK(mockturtle::has_is_pi_v<netlist>);
    CHECK(mockturtle::has_is_constant_v<netlist>);
    CHECK(mockturtle::has_get_node_v<netlist>);
    CHECK(mockturtle::has_is_complemented_v<netlist>);

    CHECK(ntl.size() == 2);

    auto c0 = ntl.get_constant(false);
    auto c1 = ntl.get_constant(true);

    CHECK(ntl.size() == 2);
    CHECK(c0 != c1);
    CHECK(ntl.get_node(c0) == 0);
    CHECK(ntl.get_node(c1) == 1);
    CHECK(!ntl.is_complemented(c0));
    CHECK(!ntl.is_complemented(c1));
    CHECK(ntl.is_constant(ntl.get_node(c0)));
    CHECK(ntl.is_constant(ntl.get_node(c1)));
    CHECK(!ntl.is_pi(ntl.get_node(c0)));
    CHECK(!ntl.is_pi(ntl.get_node(c1)));
}

TEST_CASE("create and use primary inputs in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_pi_v<netlist>);
    CHECK(mockturtle::has_is_constant_v<netlist>);
    CHECK(mockturtle::has_is_pi_v<netlist>);
    CHECK(mockturtle::has_num_pis_v<netlist>);

    CHECK(ntl.num_pis() == 0);

    auto x1 = ntl.create_pi();
    auto x2 = ntl.create_pi();

    CHECK(ntl.size() == 4);
    CHECK(ntl.num_pis() == 2);
    CHECK(x1 != x2);
}

TEST_CASE("create and use primary outputs in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_po_v<netlist>);
    CHECK(mockturtle::has_num_pos_v<netlist>);

    auto c0 = ntl.get_constant(false);
    auto c1 = ntl.get_constant(true);
    auto x  = ntl.create_pi();

    ntl.create_po(c0);
    ntl.create_po(c1);
    ntl.create_po(x);

    CHECK(ntl.size() == 3);
    CHECK(ntl.num_pis() == 1);
    CHECK(ntl.num_pos() == 3);
}

// TEST_CASE("create and use register in a netlist", "[netlist]")
// {
//     using sequential_netlist = mockturtle::sequential<netlist>;
//
//     sequential_netlist ntl{};
//
//     REQUIRE(mockturtle::has_foreach_po_v<sequential_netlist>);
//     REQUIRE(mockturtle::has_create_po_v<sequential_netlist>);
//     REQUIRE(mockturtle::has_create_pi_v<sequential_netlist>);
//     REQUIRE(mockturtle::has_create_ro_v<sequential_netlist>);
//     REQUIRE(mockturtle::has_create_ri_v<sequential_netlist>);
//     REQUIRE(mockturtle::has_create_maj_v<sequential_netlist>);
//
//     const auto c0 = ntl.get_constant(false);
//     const auto x1 = ntl.create_pi();
//     const auto x2 = ntl.create_pi();
//     const auto x3 = ntl.create_pi();
//     const auto x4 = ntl.create_pi();
//
//     CHECK(ntl.size() == 6);
//     CHECK(ntl.num_registers() == 0);
//     CHECK(ntl.num_cis() == 4);
//     CHECK(ntl.num_cos() == 0);
//
//     const auto f1 = ntl.create_maj(x1, x2, x3);
//     ntl.create_po(f1);
//
//     CHECK(ntl.num_pos() == 1);
//
//     const auto s1 = ntl.create_ro();  // ntk. input
//     ntl.create_po(s1);                // po
//
//     const auto f2 = ntl.create_maj(f1, x4, c0);
//     ntl.create_ri(f2);  // ntk. output
//
//     CHECK(ntl.num_registers() == 1);
//     CHECK(ntl.num_cis() == 4 + 1);
//     CHECK(ntl.num_cos() == 2 + 1);
//
//     ntl.foreach_pi(
//         [&](auto const& node, auto index)
//         {
//             CHECK(ntl.is_pi(node));
//             switch (index)
//             {
//                 case 0:
//                     CHECK(node == x1); /* first pi */
//                     break;
//                 case 1:
//                     CHECK(node == x2); /* second pi */
//                     break;
//                 case 2:
//                     CHECK(node == x3); /* third pi */
//                     break;
//                 case 3:
//                     CHECK(node == x4); /* fourth pi */
//                     break;
//                 default: CHECK(false);
//             }
//         });
//
//     ntl.foreach_ci(
//         [&](auto const& node, auto index)
//         {
//             CHECK(ntl.is_ci(node));
//             switch (index)
//             {
//                 case 0:
//                     CHECK(node == x1); /* first pi */
//                     break;
//                 case 1:
//                     CHECK(node == x2); /* second pi */
//                     break;
//                 case 2:
//                     CHECK(node == x3); /* third pi */
//                     break;
//                 case 3:
//                     CHECK(node == x4); /* fourth pi */
//                     break;
//                 case 4:
//                     CHECK(node == s1); /* first state-bit */
//                     CHECK(ntl.is_ci(node));
//                     CHECK(!ntl.is_pi(node));
//                     break;
//                 default: CHECK(false);
//             }
//         });
//
//     ntl.foreach_po(
//         [&](auto const& node, auto index)
//         {
//             switch (index)
//             {
//                 case 0:
//                     CHECK(node == f1); /* first po */
//                     break;
//                 case 1:
//                     CHECK(node == s1); /* second po */
//                     break;
//                 default: CHECK(false);
//             }
//         });
//
//     ntl.foreach_co(
//         [&](auto const& node, auto index)
//         {
//             switch (index)
//             {
//                 case 0:
//                     CHECK(node == f1); /* first po */
//                     break;
//                 case 1:
//                     CHECK(node == s1); /* second po */
//                     break;
//                 case 2:
//                     CHECK(node == f2); /* first next-state bit */
//                     break;
//                 default: CHECK(false);
//             }
//         });
// }

TEST_CASE("create unary operations in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_buf_v<netlist>);
    CHECK(mockturtle::has_create_not_v<netlist>);

    auto x1 = ntl.create_pi();

    CHECK(ntl.size() == 3);

    auto f1 = ntl.create_buf(x1);
    auto f2 = ntl.create_not(x1);

    CHECK(ntl.size() == 5);
    CHECK(f1 != x1);
    CHECK(f2 != x1);
}

TEST_CASE("create binary operations in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_and_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();

    CHECK(ntl.size() == 4);

    ntl.create_and(x1, x2);
    CHECK(ntl.size() == 5);

    ntl.create_and(x1, x2);
    CHECK(ntl.size() == 6);  // no structural hashing

    ntl.create_and(x2, x1);
    CHECK(ntl.size() == 7);  // no structural hashing
}

TEST_CASE("create ternary operations in a netlist", "[netlist]")
{
    netlist ntl{};

    REQUIRE(mockturtle::has_create_maj_v<netlist>);
    REQUIRE(fiction::has_create_dot_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();
    const auto x3 = ntl.create_pi();

    CHECK(ntl.size() == 5);

    const auto f1 = ntl.create_maj(x1, x2, x3);
    CHECK(ntl.size() == 6);

    const auto f2 = ntl.create_maj(x1, x2, ntl.get_constant(false));
    CHECK(ntl.size() == 7);
    CHECK(f1 != f2);

    const auto f3 = ntl.create_maj(x1, x2, ntl.get_constant(true));
    CHECK(ntl.size() == 8);
    CHECK(f2 != f3);

    const auto f4 = ntl.create_dot(x1, x2, x3);
    CHECK(ntl.size() == 9);

    const auto f5 = ntl.create_dot(x1, x2, ntl.get_constant(false));
    CHECK(ntl.size() == 10);
    CHECK(f4 != f5);

    const auto f6 = ntl.create_dot(x1, x2, ntl.get_constant(true));
    CHECK(ntl.size() == 11);
    CHECK(f5 != f6);
}

TEST_CASE("create n-ary operations in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_and_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();
    const auto x3 = ntl.create_pi();
    const auto x4 = ntl.create_pi();
    const auto x5 = ntl.create_pi();

    CHECK(ntl.size() == 7);

    ntl.create_nary_and({x1, x2, x3});
    CHECK(ntl.size() == 9);

    ntl.create_nary_or({x2, x3, x4});
    CHECK(ntl.size() == 11);

    ntl.create_nary_xor({x3, x4, x5});
    CHECK(ntl.size() == 13);
}

TEST_CASE("clone a node in a netlist", "[netlist]")
{
    netlist ntl1, ntl2;

    CHECK(mockturtle::has_clone_node_v<netlist>);

    auto a1 = ntl1.create_pi();
    auto b1 = ntl1.create_pi();
    auto f1 = ntl1.create_and(a1, b1);
    CHECK(ntl1.size() == 5);

    auto a2 = ntl2.create_pi();
    auto b2 = ntl2.create_pi();
    CHECK(ntl2.size() == 4);

    auto f2 = ntl2.clone_node(ntl1, ntl1.get_node(f1), {a2, b2});
    CHECK(ntl2.size() == 5);

    ntl2.foreach_fanin(ntl2.get_node(f2), [&](auto const& s) { CHECK(!ntl2.is_complemented(s)); });
}

TEST_CASE("compute functions from AND and NOT gates in netlists", "[netlist]")
{
    netlist ntl{};

    const auto a = ntl.create_pi();
    const auto b = ntl.create_pi();

    const auto f1 = ntl.create_not(a);
    const auto f2 = ntl.create_and(a, b);

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);

    const auto sim_f1 = ntl.compute(ntl.get_node(f1), xs.begin(), xs.begin() + 1);
    const auto sim_f2 = ntl.compute(ntl.get_node(f2), xs.begin(), xs.end());

    CHECK(sim_f1 == ~xs[0]);
    CHECK(sim_f2 == (xs[0] & xs[1]));
}

TEST_CASE("create nodes and compute a function in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_node_v<netlist>);
    CHECK(mockturtle::has_compute_v<netlist, kitty::dynamic_truth_table>);

    const auto a = ntl.create_pi();
    const auto b = ntl.create_pi();
    const auto c = ntl.create_pi();

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(3u), tt_const0(0u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "96");

    CHECK(ntl.size() == 5);

    const auto const0 = ntl.create_node({}, tt_const0);
    const auto const1 = ntl.create_node({}, ~tt_const0);
    CHECK(const0 == ntl.get_constant(false));
    CHECK(const1 == ntl.get_constant(true));

    const auto maj_n = ntl.create_node({a, b, c}, tt_maj);
    const auto xor_n = ntl.create_node({a, b, c}, tt_xor);

    CHECK(ntl.size() == 7);

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);
    kitty::create_nth_var(xs[2], 2);

    const auto sim_maj = ntl.compute(ntl.get_node(maj_n), xs.begin(), xs.end());
    const auto sim_xor = ntl.compute(ntl.get_node(xor_n), xs.begin(), xs.end());

    CHECK(sim_maj == kitty::ternary_majority(xs[0], xs[1], xs[2]));
    CHECK(sim_xor == (xs[0] ^ xs[1] ^ xs[2]));
}

TEST_CASE("create fanouts and nodes and compute a function in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_node_v<netlist>);
    CHECK(mockturtle::has_compute_v<netlist, kitty::dynamic_truth_table>);

    const auto a = ntl.create_pi();
    const auto b = ntl.create_pi();
    const auto c = ntl.create_pi();

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(3u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "96");

    const auto foa   = ntl.create_buf(a);
    const auto fob   = ntl.create_buf(b);
    const auto foc   = ntl.create_buf(c);
    const auto maj_n = ntl.create_node({foa, fob, foc}, tt_maj);
    const auto xor_n = ntl.create_node({foa, fob, foc}, tt_xor);

    std::vector<kitty::dynamic_truth_table> xs;
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    xs.emplace_back(3u);
    kitty::create_nth_var(xs[0], 0);
    kitty::create_nth_var(xs[1], 1);
    kitty::create_nth_var(xs[2], 2);

    const auto sim_maj = ntl.compute(ntl.get_node(maj_n), xs.begin(), xs.end());
    const auto sim_xor = ntl.compute(ntl.get_node(xor_n), xs.begin(), xs.end());

    CHECK(sim_maj == kitty::ternary_majority(xs[0], xs[1], xs[2]));
    CHECK(sim_xor == (xs[0] ^ xs[1] ^ xs[2]));
}

TEST_CASE("hash nodes in netlist", "[netlist]")
{
    netlist ntl{};

    const auto a = ntl.create_pi();
    const auto b = ntl.create_pi();
    const auto c = ntl.create_pi();

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(3u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "96");

    ntl.create_node({a, b, c}, tt_maj);
    ntl.create_node({a, b, c}, tt_xor);

    CHECK(ntl.size() == 7);

    ntl.create_node({a, b, c}, tt_maj);

    CHECK(ntl.size() == 8);  // no structural hashing
}

TEST_CASE("subsitute node by another", "[netlist]")
{
    netlist ntl{};

    const auto c0 = ntl.get_node(ntl.get_constant(false));
    const auto c1 = ntl.get_node(ntl.get_constant(true));
    const auto a  = ntl.get_node(ntl.create_pi());
    const auto b  = ntl.get_node(ntl.create_pi());

    kitty::dynamic_truth_table tt_nand(2u), tt_le(2u), tt_ge(2u), tt_or(2u);
    kitty::create_from_hex_string(tt_nand, "7");
    kitty::create_from_hex_string(tt_le, "2");
    kitty::create_from_hex_string(tt_ge, "4");
    kitty::create_from_hex_string(tt_or, "e");

    // XOR with NAND
    const auto n1 = ntl.get_node(ntl.create_node({ntl.make_signal(a), ntl.make_signal(b)}, tt_nand));
    const auto n2 = ntl.get_node(ntl.create_node({ntl.make_signal(a), ntl.make_signal(n1)}, tt_nand));
    const auto n3 = ntl.get_node(ntl.create_node({ntl.make_signal(b), ntl.make_signal(n1)}, tt_nand));
    const auto n4 = ntl.get_node(ntl.create_node({ntl.make_signal(n2), ntl.make_signal(n3)}, tt_nand));
    ntl.create_po(ntl.make_signal(n4));

    std::vector<mockturtle::node<netlist>> nodes;
    ntl.foreach_node([&](auto node) { nodes.push_back(node); });

    CHECK(nodes == std::vector<netlist::node>{c0, c1, a, b, n1, n2, n3, n4});
    CHECK(ntl.fanout_size(n4) == 1);
    ntl.foreach_po(
        [&](auto f)
        {
            CHECK(ntl.get_node(f) == n4);
            return false;
        });

    // XOR with AND and OR
    const auto n5 = ntl.get_node(ntl.create_node({ntl.make_signal(a), ntl.make_signal(b)}, tt_le));
    const auto n6 = ntl.get_node(ntl.create_node({ntl.make_signal(a), ntl.make_signal(b)}, tt_ge));
    const auto n7 = ntl.get_node(ntl.create_node({ntl.make_signal(n5), ntl.make_signal(n6)}, tt_or));

    nodes.clear();
    ntl.foreach_node([&](auto node) { nodes.push_back(node); });

    CHECK(nodes == std::vector<mockturtle::node<netlist>>{c0, c1, a, b, n1, n2, n3, n4, n5, n6, n7});
    CHECK(ntl.fanout_size(n7) == 0);

    // substitute nodes
    ntl.substitute_node(n4, ntl.make_signal(n7));

    CHECK(ntl.size() == 11);
    CHECK(ntl.fanout_size(n4) == 0);
    CHECK(ntl.fanout_size(n7) == 1);
    ntl.foreach_po(
        [&](auto f)
        {
            CHECK(ntl.get_node(f) == n7);
            return false;
        });
}

TEST_CASE("structural properties of a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_size_v<netlist>);
    CHECK(mockturtle::has_num_pis_v<netlist>);
    CHECK(mockturtle::has_num_pos_v<netlist>);
    CHECK(mockturtle::has_num_gates_v<netlist>);
    CHECK(mockturtle::has_fanin_size_v<netlist>);
    CHECK(mockturtle::has_fanout_size_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();

    const auto f1 = ntl.create_and(x1, x2);
    const auto f2 = ntl.create_and(x2, x1);

    ntl.create_po(f1);
    ntl.create_po(f2);

    CHECK(ntl.size() == 6);
    CHECK(ntl.num_pis() == 2);
    CHECK(ntl.num_pos() == 2);
    CHECK(ntl.num_gates() == 2);
    CHECK(ntl.fanin_size(ntl.get_node(x1)) == 0);
    CHECK(ntl.fanin_size(ntl.get_node(x2)) == 0);
    CHECK(ntl.fanin_size(ntl.get_node(f1)) == 2);
    CHECK(ntl.fanin_size(ntl.get_node(f2)) == 2);
    CHECK(ntl.fanout_size(ntl.get_node(x1)) == 2);
    CHECK(ntl.fanout_size(ntl.get_node(x2)) == 2);
    CHECK(ntl.fanout_size(ntl.get_node(f1)) == 1);
    CHECK(ntl.fanout_size(ntl.get_node(f2)) == 1);
}

TEST_CASE("Node functions of a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_create_and_v<netlist>);
    CHECK(mockturtle::has_create_nand_v<netlist>);
    CHECK(mockturtle::has_create_or_v<netlist>);
    CHECK(mockturtle::has_create_nor_v<netlist>);
    CHECK(mockturtle::has_create_xor_v<netlist>);
    CHECK(mockturtle::has_create_xnor_v<netlist>);
    CHECK(mockturtle::has_create_lt_v<netlist>);
    CHECK(mockturtle::has_create_le_v<netlist>);
    CHECK(mockturtle::has_create_gt_v<netlist>);
    CHECK(mockturtle::has_create_ge_v<netlist>);
    CHECK(mockturtle::has_create_maj_v<netlist>);
    CHECK(mockturtle::has_create_ite_v<netlist>);
    CHECK(mockturtle::has_create_xor3_v<netlist>);

    CHECK(mockturtle::has_is_and_v<netlist>);
    CHECK(fiction::has_is_nand_v<netlist>);
    CHECK(mockturtle::has_is_or_v<netlist>);
    CHECK(fiction::has_is_nor_v<netlist>);
    CHECK(mockturtle::has_is_xor_v<netlist>);
    CHECK(fiction::has_is_xnor_v<netlist>);
    CHECK(fiction::has_is_lt_v<netlist>);
    CHECK(fiction::has_is_le_v<netlist>);
    CHECK(fiction::has_is_gt_v<netlist>);
    CHECK(fiction::has_is_ge_v<netlist>);
    CHECK(mockturtle::has_is_maj_v<netlist>);
    CHECK(mockturtle::has_is_ite_v<netlist>);
    CHECK(mockturtle::has_is_xor3_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();
    const auto x3 = ntl.create_pi();

    const auto and_signal   = ntl.create_and(x1, x2);
    const auto nand_signal  = ntl.create_nand(x1, x2);
    const auto or_signal    = ntl.create_or(x1, x2);
    const auto nor_signal   = ntl.create_nor(x1, x2);
    const auto xor2_signal  = ntl.create_xor(x1, x2);
    const auto xnor2_signal = ntl.create_xnor(x1, x2);
    const auto lt_signal    = ntl.create_lt(x1, x2);
    const auto le_signal    = ntl.create_le(x1, x2);
    const auto gt_signal    = ntl.create_gt(x1, x2);
    const auto ge_signal    = ntl.create_ge(x1, x2);
    const auto maj_signal   = ntl.create_maj(x1, x2, x3);
    const auto dot_signal   = ntl.create_dot(x1, x2, x3);
    const auto ite_signal   = ntl.create_ite(x1, x2, x3);
    const auto xor3_signal  = ntl.create_xor3(x1, x2, x3);

    ntl.create_po(and_signal);
    ntl.create_po(nand_signal);
    ntl.create_po(or_signal);
    ntl.create_po(nor_signal);
    ntl.create_po(xor2_signal);
    ntl.create_po(xnor2_signal);
    ntl.create_po(maj_signal);
    ntl.create_po(dot_signal);
    ntl.create_po(ite_signal);
    ntl.create_po(xor3_signal);

    CHECK(ntl.is_and(ntl.get_node(and_signal)));
    CHECK(ntl.is_nand(ntl.get_node(nand_signal)));
    CHECK(ntl.is_or(ntl.get_node(or_signal)));
    CHECK(ntl.is_nor(ntl.get_node(nor_signal)));
    CHECK(ntl.is_xor(ntl.get_node(xor2_signal)));
    CHECK(ntl.is_lt(ntl.get_node(lt_signal)));
    CHECK(ntl.is_le(ntl.get_node(le_signal)));
    CHECK(ntl.is_gt(ntl.get_node(gt_signal)));
    CHECK(ntl.is_ge(ntl.get_node(ge_signal)));
    CHECK(ntl.is_xnor(ntl.get_node(xnor2_signal)));
    CHECK(ntl.is_maj(ntl.get_node(maj_signal)));
    CHECK(ntl.is_dot(ntl.get_node(dot_signal)));
    CHECK(ntl.is_ite(ntl.get_node(ite_signal)));
    CHECK(ntl.is_xor3(ntl.get_node(xor3_signal)));
}

TEST_CASE("node and signal iteration in a netlist", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_foreach_node_v<netlist>);
    CHECK(mockturtle::has_foreach_pi_v<netlist>);
    CHECK(mockturtle::has_foreach_po_v<netlist>);
    CHECK(mockturtle::has_foreach_fanin_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();
    const auto f1 = ntl.create_and(x1, x2);
    const auto f2 = ntl.create_and(x2, x1);
    ntl.create_po(f1);
    ntl.create_po(f2);

    CHECK(ntl.size() == 6);

    /* iterate over nodes */
    uint32_t mask{0}, counter{0};
    ntl.foreach_node(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 63);
    CHECK(counter == 15);

    mask = 0;
    ntl.foreach_node([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 63);

    mask = counter = 0;
    ntl.foreach_node(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 1);
    CHECK(counter == 0);

    mask = 0;
    ntl.foreach_node(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 1);

    /* iterate over PIs */
    mask = counter = 0;
    ntl.foreach_pi(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
        });
    CHECK(mask == 12);
    CHECK(counter == 1);

    mask = 0;
    ntl.foreach_pi([&](auto n) { mask |= (1u << n); });
    CHECK(mask == 12);

    mask = counter = 0;
    ntl.foreach_pi(
        [&](auto n, auto i)
        {
            mask |= (1u << n);
            counter += i;
            return false;
        });
    CHECK(mask == 4);
    CHECK(counter == 0);

    mask = 0;
    ntl.foreach_pi(
        [&](auto n)
        {
            mask |= (1u << n);
            return false;
        });
    CHECK(mask == 4);

    /* iterate over POs */
    mask = counter = 0;
    ntl.foreach_po(
        [&](auto s, auto i)
        {
            mask |= (1u << ntl.get_node(s));
            counter += i;
        });
    CHECK(mask == 48);
    CHECK(counter == 1);

    mask = 0;
    ntl.foreach_po([&](auto s) { mask |= (1u << ntl.get_node(s)); });
    CHECK(mask == 48);

    mask = counter = 0;
    ntl.foreach_po(
        [&](auto s, auto i)
        {
            mask |= (1u << ntl.get_node(s));
            counter += i;
            return false;
        });
    CHECK(mask == 16);
    CHECK(counter == 0);

    mask = 0;
    ntl.foreach_po(
        [&](auto s)
        {
            mask |= (1u << ntl.get_node(s));
            return false;
        });
    CHECK(mask == 16);
}

TEST_CASE("custom node values in netlists", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_clear_values_v<netlist>);
    CHECK(mockturtle::has_value_v<netlist>);
    CHECK(mockturtle::has_set_value_v<netlist>);
    CHECK(mockturtle::has_incr_value_v<netlist>);
    CHECK(mockturtle::has_decr_value_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();
    const auto f1 = ntl.create_and(x1, x2);
    const auto f2 = ntl.create_and(x2, x1);
    ntl.create_po(f1);
    ntl.create_po(f2);

    CHECK(ntl.size() == 6);

    ntl.clear_values();
    ntl.foreach_node(
        [&](auto n)
        {
            CHECK(ntl.value(n) == 0);
            ntl.set_value(n, static_cast<uint32_t>(n));
            CHECK(ntl.value(n) == n);
            CHECK(ntl.incr_value(n) == n);
            CHECK(ntl.value(n) == n + 1);
            CHECK(ntl.decr_value(n) == n);
            CHECK(ntl.value(n) == n);
        });
    ntl.clear_values();
    ntl.foreach_node([&](auto n) { CHECK(ntl.value(n) == 0); });
}

TEST_CASE("visited values in netlists", "[netlist]")
{
    netlist ntl{};

    CHECK(mockturtle::has_clear_visited_v<netlist>);
    CHECK(mockturtle::has_visited_v<netlist>);
    CHECK(mockturtle::has_set_visited_v<netlist>);

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();
    const auto f1 = ntl.create_and(x1, x2);
    const auto f2 = ntl.create_and(x2, x1);
    ntl.create_po(f1);
    ntl.create_po(f2);

    CHECK(ntl.size() == 6);

    ntl.clear_visited();
    ntl.foreach_node(
        [&](auto n)
        {
            CHECK(ntl.visited(n) == 0);
            ntl.set_visited(n, static_cast<uint32_t>(n));
            CHECK(ntl.visited(n) == n);
        });
    ntl.clear_visited();
    ntl.foreach_node([&](auto n) { CHECK(ntl.visited(n) == 0); });
}

TEST_CASE("substitute PO signals", "[netlist]")
{
    netlist ntl{};

    const auto x1 = ntl.create_pi();
    const auto x2 = ntl.create_pi();
    const auto x3 = ntl.create_pi();

    const auto a = ntl.create_and(x1, x2);
    const auto i = ntl.create_not(x3);
    const auto b = ntl.create_buf(i);

    ntl.create_po(a);
    ntl.create_po(i);
    ntl.create_po(b);

    CHECK(ntl.size() == 8);

    CHECK(ntl.is_po(a));
    CHECK(ntl.is_po(i));
    CHECK(ntl.is_po(b));

    CHECK(ntl.fanout_size(ntl.get_node(a)) == 1);
    CHECK(ntl.fanout_size(ntl.get_node(i)) == 2);
    CHECK(ntl.fanout_size(ntl.get_node(b)) == 1);

    ntl.substitute_po_signals();

    CHECK(ntl.size() == 10);
    CHECK(!ntl.is_po(a));
    CHECK(!ntl.is_po(i));
    CHECK(ntl.is_po(b));  // b pointed to a buf already and, thus, wasn't substituted

    CHECK(ntl.fanout_size(ntl.get_node(a)) == 1);
    CHECK(ntl.fanout_size(ntl.get_node(i)) == 2);
    CHECK(ntl.fanout_size(ntl.get_node(b)) == 1);
}
