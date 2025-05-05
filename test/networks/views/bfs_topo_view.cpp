//
// Created by benjamin on 08.04.24.
//
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <set>
#include <vector>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/traits.hpp>
#include <fiction/networks/views/bfs_topo_view.hpp>

using namespace fiction;

TEST_CASE( "Create a topo_view on an AIG", "[bfs-topo-view]" )
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto f = aig.create_xor( x1, x2 );
    aig.create_po( f );

    std::set<mockturtle::node<mockturtle::aig_network>> nodes;
    aig.foreach_node( [&nodes]( auto node ) { nodes.insert( node ); } );
    CHECK( nodes.size() == 6 );

    const fiction::bfs_topo_view aig2{ aig };
    nodes.clear();
    aig2.foreach_node( [&nodes]( auto node ) { nodes.insert( node ); } );
    CHECK( nodes.size() == 6 );
}

TEST_CASE( "Check BFS order", "[bfs-topo-view]" )
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    const auto x4 = aig.create_pi();
    const auto x5 = aig.create_pi();
    const auto f1 = aig.create_and( x1, x2 );
    const auto f2 = aig.create_and( f1, x3 ); // Node 7 = last element in BFS
    const auto f3 = aig.create_and( x4, x5 ); // Node 8 = last element created
    aig.create_po( f2 );
    aig.create_po( f3 );

    std::vector<mockturtle::node<mockturtle::aig_network>> nodes;
    aig.foreach_node( [&nodes]( auto node ) { nodes.push_back( node ); } );
    CHECK( *nodes.rbegin() == 8 );

    const fiction::bfs_topo_view aig2{ aig };
    nodes.clear();
    aig2.foreach_node( [&nodes]( auto node ) { nodes.push_back( node ); } );
    CHECK( *nodes.rbegin() == 7 );
}

TEST_CASE( "Create a bfs_topo_view on an AIG without bfs topo order", "[bfs-topo-view]" )
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    const auto gate1 = aig.create_and( x1, x2 );
    const auto gate2 = aig.create_and( x3, gate1 );

    /* switch gate order on storage */
    aig._storage->nodes[aig.get_node( gate2 )].children[0].index = aig.get_node( x1 );
    aig._storage->nodes[aig.get_node( gate2 )].children[1].index = aig.get_node( x2 );

    aig._storage->nodes[aig.get_node( gate1 )].children[0].index = aig.get_node( x3 );
    aig._storage->nodes[aig.get_node( gate1 )].children[1].index = aig.get_node( gate2 );

    aig.create_po( gate1 );

    /* test topological order of nodes */
    std::vector<mockturtle::node<mockturtle::aig_network>> nodes;
    aig.foreach_node( [&nodes]( auto node ) { nodes.push_back( node ); } );
    CHECK( nodes == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 0, 1, 2, 3, 4, 5 } } );

    const bfs_topo_view aig2{ aig };
    nodes.clear();
    aig2.foreach_node( [&nodes]( auto node ) { nodes.push_back( node ); } );
    CHECK( nodes == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 0, 1, 2, 3, 5, 4 } } );

    /* test topological order of gates */
    std::vector<mockturtle::node<mockturtle::aig_network>> gates;
    aig.foreach_gate( [&gates]( auto node ) { gates.push_back( node ); } );
    CHECK( gates == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 4, 5 } } );

    gates.clear();
    aig2.foreach_gate( [&gates]( auto node ) { gates.push_back( node ); } );
    CHECK( gates == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 5, 4 } } );

    /* test normalized index order */
    uint32_t counter = 0;
    aig2.foreach_node( [&aig2, &counter]( auto node ) {
                          CHECK( aig2.node_to_index( node ) == counter++ );
                      } );
}

TEST_CASE( "Test reverse bfs topo order", "[bfs-topo-view]" )
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    const auto gate1 = aig.create_and( x1, x2 );
    const auto gate2 = aig.create_and( x3, gate1 );

    /* switch gate order on storage */
    aig._storage->nodes[aig.get_node( gate2 )].children[0].index = aig.get_node( x1 );
    aig._storage->nodes[aig.get_node( gate2 )].children[1].index = aig.get_node( x2 );

    aig._storage->nodes[aig.get_node( gate1 )].children[0].index = aig.get_node( x3 );
    aig._storage->nodes[aig.get_node( gate1 )].children[1].index = aig.get_node( gate2 );

    aig.create_po( gate1 );

    /* test topological order of nodes */
    std::vector<mockturtle::node<mockturtle::aig_network>> nodes;
    aig.foreach_node( [&nodes]( auto node ) { nodes.push_back( node ); } );
    CHECK( nodes == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 0, 1, 2, 3, 4, 5 } } );

    const bfs_topo_view aig2{ aig };
    nodes.clear();
    aig2.foreach_node_reverse( [&nodes]( auto node ) { nodes.push_back( node ); } );
    CHECK( nodes == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 4, 5, 3, 2, 1, 0 } } );

    /* test topological order of gates */
    std::vector<mockturtle::node<mockturtle::aig_network>> gates;
    aig.foreach_gate( [&gates]( auto node ) { gates.push_back( node ); } );
    CHECK( gates == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 4, 5 } } );

    gates.clear();
    aig2.foreach_gate_reverse( [&gates]( auto node ) { gates.push_back( node ); } );
    CHECK( gates == std::vector<mockturtle::node<mockturtle::aig_network>>{ { 4, 5 } } );
}
