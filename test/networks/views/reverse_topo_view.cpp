//
// Created by marcel on 18.01.22.
//

// This file is based on mockturtle/test/views/topo_view.cpp

#include "fiction/networks/views/reverse_topo_view.hpp"

#include "catch.hpp"

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <set>
#include <vector>

using namespace fiction;

TEST_CASE("create a reverse_topo_view on an AIG", "[reverse-topo-view]")
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto f  = aig.create_xor(x1, x2);
    aig.create_po(f);

    std::set<mockturtle::node<mockturtle::aig_network>> nodes;
    aig.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 6);

    reverse_topo_view aig2{aig};
    nodes.clear();
    aig2.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 6);
}

TEST_CASE("create a reverse_topo_view on an AIG without output", "[reverse-topo-view]")
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    aig.create_xor(x1, x2);

    std::set<mockturtle::node<mockturtle::aig_network>> nodes;
    aig.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 6);
    CHECK(aig.size() == 6);
    CHECK(aig.num_gates() == 3);

    reverse_topo_view aig2{aig};
    nodes.clear();
    aig2.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 3);
    CHECK(aig2.size() == 3);
    CHECK(aig2.num_gates() == 0);
}

TEST_CASE("create a reverse_topo_view on an AIG without topo order", "[reverse-topo-view]")
{
    mockturtle::aig_network aig;

    const auto x1    = aig.create_pi();
    const auto x2    = aig.create_pi();
    const auto x3    = aig.create_pi();
    const auto gate1 = aig.create_and(x1, x2);
    const auto gate2 = aig.create_and(x3, gate1);

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

    /* switch gate order on storage */
    aig._storage->nodes[aig.get_node(gate2)].children[0].index = aig.get_node(x1);
    aig._storage->nodes[aig.get_node(gate2)].children[1].index = aig.get_node(x2);

    aig._storage->nodes[aig.get_node(gate1)].children[0].index = aig.get_node(x3);
    aig._storage->nodes[aig.get_node(gate1)].children[1].index = aig.get_node(gate2);

#pragma GCC diagnostic pop

    aig.create_po(gate1);

    /* test topological order of nodes */
    std::vector<mockturtle::node<mockturtle::aig_network>> nodes;
    aig.foreach_node([&nodes](const auto& node) { nodes.push_back(node); });
    CHECK(nodes == std::vector<mockturtle::node<mockturtle::aig_network>>{{0, 1, 2, 3, 4, 5}});

    reverse_topo_view aig2{aig};
    nodes.clear();
    aig2.foreach_node([&nodes](const auto& node) { nodes.push_back(node); });
    CHECK(nodes == std::vector<mockturtle::node<mockturtle::aig_network>>{{4, 5, 3, 2, 1, 0}});

    /* test topological order of gates */
    std::vector<mockturtle::node<mockturtle::aig_network>> gates;
    aig.foreach_gate([&gates](const auto& node) { gates.push_back(node); });
    CHECK(gates == std::vector<mockturtle::node<mockturtle::aig_network>>{{4, 5}});

    gates.clear();
    aig2.foreach_gate([&gates](const auto& node) { gates.push_back(node); });
    CHECK(gates == std::vector<mockturtle::node<mockturtle::aig_network>>{{4, 5}});

    /* test normalized index order */
    uint32_t counter = 0;
    aig2.foreach_node([&aig2, &counter](const auto& node) { CHECK(aig2.node_to_index(node) == counter++); });
}
