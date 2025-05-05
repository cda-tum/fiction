//
// Created by benjamin on 08.04.25.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/network_transformation/network_balancing.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/networks/views/bfs_topo_view.hpp"
#include "fiction/networks/views/mutable_rank_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/graph/mincross.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/names_view.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Dummy Check", "[mincross]")
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto f  = aig.create_xor(x1, x2);
    aig.create_po(f);

    const auto aig_r = fiction::mutable_rank_view(aig);

    mincross_stats        st{};
    const mincross_params p{};
    const auto            ntk = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 1);
}

TEST_CASE("Fixed PIs Check", "[mincross]")
{
    fiction::technology_network tec;

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto b  = tec.create_not(x2);
    const auto f  = tec.create_and(x1, x3);
    tec.create_po(f);
    tec.create_po(b);

    const auto aig_r = fiction::mutable_rank_view(tec);

    mincross_stats  st{};
    mincross_params p{};
    p.fixed_pis = false;
    auto ntk    = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 0);

    p.fixed_pis = true;
    ntk         = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 1);
}

TEST_CASE("Planar Network", "[mincross]")
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    const auto f1 = aig.create_and(x1, x2);
    const auto f2 = aig.create_and(x2, x3);
    aig.create_po(f1);
    aig.create_po(f2);

    auto                                                   aig_r = fiction::mutable_rank_view(aig);
    std::vector<mockturtle::node<mockturtle::aig_network>> rank1;
    rank1.push_back(aig_r.get_node(f2));
    rank1.push_back(aig_r.get_node(f1));
    aig_r.set_ranks(1, rank1);

    mincross_stats        st{};
    const mincross_params p{};
    auto                  ntk = mincross(aig_r, p, &st, false);  // counts crossings
    CHECK(st.num_crossings == 3);

    ntk = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 0);
}

TEST_CASE("Majority", "[mincross]")
{
    fiction::technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto f1 = tec.create_maj(x1, x2, x3);
    const auto f2 = tec.create_not(x1);
    tec.create_po(f1);
    tec.create_po(f2);

    auto                                                       tec_r = fiction::mutable_rank_view(tec);
    std::vector<mockturtle::node<fiction::technology_network>> rank1;
    rank1.push_back(tec_r.get_node(f1));
    rank1.push_back(tec_r.get_node(f2));
    tec_r.set_ranks(1, rank1);

    mincross_stats        st{};
    const mincross_params p{};
    auto                  ntk = mincross(tec_r, p, &st, false);  // counts crossings
    CHECK(st.num_crossings == 2);

    ntk = mincross(tec_r, p, &st);
    CHECK(st.num_crossings == 0);
}

TEST_CASE("Minimize Crossings Adder", "[mincross]")
{
    auto tec = blueprints::full_adder_network<mockturtle::names_view<technology_network>>();

    auto tec_b = network_balancing<technology_network>(tec);

    auto tec_topo = fiction::bfs_topo_view(tec_b);

    auto tec_r = fiction::mutable_rank_view(tec_b);

    mincross_stats        st{};
    const mincross_params p{};

    auto ntk = mincross(tec_r, p, &st, false);  // counts crossings
    CHECK(st.num_crossings == 5);

    ntk = mincross(tec_r, p, &st);
    CHECK(st.num_crossings == 2);
}
