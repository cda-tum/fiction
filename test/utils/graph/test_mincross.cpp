/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/utils/graph/mincross.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/bfs_topo_view.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/graph/mincross.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/traits.hpp>

#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::utils::graph;

/**
 * Checks that each output rank contains the input rank's nodes.
 *
 * @tparam Ntk Ranked network type.
 * @param input Original network.
 * @param result Reordered network.
 */
template <typename Ntk>
void check_rank_permutations(const Ntk& input, const Ntk& result)
{
    CHECK(result.check_validity());
    REQUIRE(result.depth() == input.depth());
    for (uint32_t level = 0; level <= input.depth(); ++level)
    {
        CAPTURE(level);
        CHECK_THAT(result.get_ranks(level), Catch::Matchers::UnorderedRangeEquals(input.get_ranks(level)));
    }
}

TEST_CASE("Minimize crossings in an XOR network", "[mincross]")
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto f  = aig.create_xor(x1, x2);
    aig.create_po(f);

    const auto aig_r = mutable_rank_view(aig);

    mincross_stats        st{};
    const mincross_params p{};
    const auto            ntk = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 1);
    CHECK(count_crossings(ntk) == 1);
}

TEST_CASE("Keep primary inputs in rank order when requested", "[mincross]")
{
    technology_network tec;

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto b  = tec.create_not(x2);
    const auto f  = tec.create_and(x1, x3);
    tec.create_po(f);
    tec.create_po(b);

    const auto aig_r        = mutable_rank_view(tec);
    const auto original_pis = aig_r.get_ranks(0);

    mincross_stats  st{};
    mincross_params p{.fixed_pis = false};
    auto            ntk = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 0);
    CHECK(count_crossings(ntk) == 0);

    st          = {};
    p.fixed_pis = true;
    ntk         = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 1);
    CHECK(count_crossings(ntk) == 1);
    CHECK(ntk.get_ranks(0) == original_pis);
    CHECK(aig_r.get_ranks(0) == original_pis);
    check_rank_permutations(aig_r, ntk);
}

TEST_CASE("Minimize crossings without changing network connectivity", "[mincross]")
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    const auto f1 = aig.create_and(x1, x2);
    const auto f2 = aig.create_and(x2, x3);
    aig.create_po(f1);
    aig.create_po(f2);

    auto                                                   aig_r = mutable_rank_view(aig);
    std::vector<mockturtle::node<mockturtle::aig_network>> rank1;
    rank1.push_back(aig_r.get_node(f2));
    rank1.push_back(aig_r.get_node(f1));
    aig_r.set_ranks(1, rank1);
    const auto original_pis = aig_r.get_ranks(0);

    mincross_stats  st{};
    mincross_params p{.optimize = false};

    auto ntk = mincross(aig_r, p, &st);  // counts crossings
    CHECK(st.num_crossings == 3);
    CHECK(ntk.get_all_ranks() == aig_r.get_all_ranks());

    st         = {};
    p.optimize = true;
    ntk        = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 0);
    CHECK(count_crossings(ntk) == 0);
    check_rank_permutations(aig_r, ntk);

    st          = {};
    p.fixed_pis = true;
    ntk         = mincross(aig_r, p, &st);
    CHECK(st.num_crossings == 0);
    CHECK(count_crossings(ntk) == 0);
    CHECK(ntk.get_ranks(0) == original_pis);
    CHECK(aig_r.get_ranks(0) == original_pis);
    const std::vector<mockturtle::node<mockturtle::aig_network>> expected_rank{aig.get_node(f1), aig.get_node(f2)};
    CHECK(ntk.get_ranks(1) == expected_rank);
    CHECK(aig_r.get_ranks(1) == rank1);
    check_rank_permutations(aig_r, ntk);
    CHECK(ntk.po_at(0) == f1);
    CHECK(ntk.po_at(1) == f2);
    std::vector<mockturtle::signal<mockturtle::aig_network>> fanins1{};
    std::vector<mockturtle::signal<mockturtle::aig_network>> fanins2{};
    ntk.foreach_fanin(ntk.get_node(f1), [&fanins1](const auto& f) { fanins1.push_back(f); });
    ntk.foreach_fanin(ntk.get_node(f2), [&fanins2](const auto& f) { fanins2.push_back(f); });
    const std::vector<mockturtle::signal<mockturtle::aig_network>> expected_fanins1{x1, x2};
    const std::vector<mockturtle::signal<mockturtle::aig_network>> expected_fanins2{x2, x3};
    CHECK_THAT(fanins1, Catch::Matchers::UnorderedRangeEquals(expected_fanins1));
    CHECK_THAT(fanins2, Catch::Matchers::UnorderedRangeEquals(expected_fanins2));
}

TEST_CASE("Minimize crossings in a majority network", "[mincross]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto f1 = tec.create_maj(x1, x2, x3);
    const auto f2 = tec.create_not(x1);
    tec.create_po(f1);
    tec.create_po(f2);

    auto                                              tec_r = mutable_rank_view(tec);
    std::vector<mockturtle::node<technology_network>> rank1;
    rank1.push_back(tec_r.get_node(f1));
    rank1.push_back(tec_r.get_node(f2));
    tec_r.set_ranks(1, rank1);

    mincross_stats  st{};
    mincross_params p{.optimize = false};

    auto ntk = mincross(tec_r, p, &st);  // counts crossings
    CHECK(st.num_crossings == 2);
    CHECK(ntk.get_all_ranks() == tec_r.get_all_ranks());

    st         = {};
    p.optimize = true;
    ntk        = mincross(tec_r, p, &st);
    CHECK(st.num_crossings == 0);
    CHECK(count_crossings(ntk) == 0);
    check_rank_permutations(tec_r, ntk);
}

TEST_CASE("Minimize crossings in a full adder", "[mincross]")
{
    auto tec = blueprints::full_adder_network<named_technology_network>();

    auto tec_b = network_balancing<technology_network>(tec);

    auto tec_topo = bfs_topo_view(tec_b);

    auto tec_r = mutable_rank_view(tec_topo);

    mincross_stats  st{};
    mincross_params p{.optimize = false};

    auto ntk = mincross(tec_r, p, &st);  // counts crossings
    CHECK(st.num_crossings == 4);
    CHECK(ntk.get_all_ranks() == tec_r.get_all_ranks());

    st         = {};
    p.optimize = true;
    ntk        = mincross(tec_r, p, &st);
    CHECK(st.num_crossings == 2);
    CHECK(count_crossings(ntk) == 2);
    check_rank_permutations(tec_r, ntk);
}

TEST_CASE("Counting crossings agrees with mincross without optimization", "[mincross]")
{
    for (const auto& tec :
         {blueprints::full_adder_network<technology_network>(), blueprints::mux21_network<technology_network>(),
          blueprints::maj4_network<technology_network>()})
    {
        INFO("Network size: " << tec.size());
        const auto tec_r = mutable_rank_view(network_balancing<technology_network>(tec));

        mincross_stats        st{};
        const mincross_params p{.optimize = false};

        static_cast<void>(mincross(tec_r, p, &st));

        CHECK(count_crossings(tec_r) == st.num_crossings);
    }
}

TEST_CASE("Counting crossings of a planar network", "[mincross]")
{
    mockturtle::aig_network aig;

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    aig.create_po(aig.create_and(x1, x2));
    aig.create_po(aig.create_and(x2, x3));

    auto aig_r = mutable_rank_view(aig);

    // the creation order of the gates puts them in the order of their inputs
    CHECK(count_crossings(aig_r) == 0);

    aig_r.set_ranks(1, {aig_r.get_node(aig.po_at(1)), aig_r.get_node(aig.po_at(0))});

    CHECK(count_crossings(aig_r) == 3);
}

TEST_CASE("Counting crossings rejects unbalanced networks", "[mincross]")
{
    const auto tec = blueprints::unbalanced_and_inv_network<technology_network>();

    CHECK_THROWS_AS(count_crossings(mutable_rank_view(tec)), std::invalid_argument);
}

TEST_CASE("Count crossings for parallel edges", "[mincross]")
{
    technology_network tec{};
    const auto         a = tec.create_pi();
    const auto         b = tec.create_pi();
    const auto         x = tec.create_and(b, b);
    const auto         y = tec.create_and(a, b);
    tec.create_po(x);
    tec.create_po(y);
    auto ranked = mutable_rank_view(tec);

    SECTION("Parallel edges cross a third edge")
    {
        mincross_stats st{};
        const auto     result = mincross(ranked, mincross_params{.optimize = false}, &st);
        CHECK(st.num_crossings == 2);
        CHECK(count_crossings(result) == 2);
    }
    SECTION("Edges sharing an endpoint do not cross")
    {
        ranked.set_ranks(1, {ranked.get_node(y), ranked.get_node(x)});
        mincross_stats st{};
        const auto     result = mincross(ranked, mincross_params{.optimize = false}, &st);
        CHECK(st.num_crossings == 0);
        CHECK(count_crossings(result) == 0);
    }
    SECTION("Optimized statistics count every edge")
    {
        mincross_stats st{};
        const auto     result = mincross(ranked, {}, &st);
        CHECK(st.num_crossings == 0);
        CHECK(count_crossings(result) == 0);
    }
}

TEST_CASE("Crossing minimization rejects unbalanced networks", "[mincross]")
{
    technology_network tec{};
    const auto         a = tec.create_pi();
    const auto         b = tec.create_pi();
    const auto         c = tec.create_pi();
    const auto         d = tec.create_pi();
    const auto         e = tec.create_pi();
    const auto         g = tec.create_and(a, b);
    tec.create_po(tec.create_and(g, b));
    tec.create_po(tec.create_and(g, c));
    tec.create_po(tec.create_and(g, d));
    tec.create_po(tec.create_and(g, e));
    auto ranked = mutable_rank_view(tec);

    CHECK_THROWS_AS(mincross(ranked, mincross_params{.optimize = false}), std::invalid_argument);
    CHECK_THROWS_AS(mincross(ranked), std::invalid_argument);
}

TEST_CASE("Optimized parallel-edge networks report remaining crossings", "[mincross]")
{
    technology_network tec{};
    const auto         a = tec.create_pi();
    const auto         b = tec.create_pi();
    tec.create_po(tec.create_and(a, a));
    tec.create_po(tec.create_and(a, b));
    tec.create_po(tec.create_and(a, b));
    tec.create_po(tec.create_and(b, b));
    auto           ranked = mutable_rank_view(tec);
    mincross_stats st{};
    const auto     result = mincross(ranked, {}, &st);

    CHECK(count_crossings(result) == 1);
    CHECK(st.num_crossings == count_crossings(result));
}

TEST_CASE("Minimize crossings in depth-zero and constant-fanin networks", "[mincross]")
{
    technology_network tec{};
    const auto         a = tec.create_pi();
    SECTION("Input-only network")
    {
        tec.create_po(a);
        auto           ranked = mutable_rank_view(tec);
        mincross_stats st{};
        const auto     result = mincross(ranked, {}, &st);
        CHECK(st.num_crossings == 0);
        CHECK(count_crossings(result) == 0);
        CHECK(result.get_all_ranks() == ranked.get_all_ranks());
    }
    SECTION("Constant input is not an edge between ranks")
    {
        tec.create_po(tec.create_and(a, tec.get_constant(false)));
        auto           ranked = mutable_rank_view(tec);
        mincross_stats st{};
        const auto     result = mincross(ranked, {}, &st);
        CHECK(st.num_crossings == 0);
        CHECK(count_crossings(result) == 0);
        check_rank_permutations(ranked, result);
    }
}

TEST_CASE("Count parallel edges between internal ranks", "[mincross]")
{
    technology_network tec{};
    const auto         a = tec.create_pi();
    const auto         b = tec.create_pi();
    const auto         x = tec.create_buf(a);
    const auto         y = tec.create_buf(b);
    tec.create_po(tec.create_and(y, y));
    tec.create_po(tec.create_and(x, y));
    auto ranked = mutable_rank_view(tec);

    SECTION("Count without optimization")
    {
        mincross_stats st{};
        const auto     result = mincross(ranked, mincross_params{.optimize = false}, &st);
        CHECK(st.num_crossings == 2);
        CHECK(count_crossings(result) == 2);
    }
    SECTION("Minimize crossings")
    {
        mincross_stats st{};
        const auto     result = mincross(ranked, {}, &st);
        CHECK(st.num_crossings == 0);
        CHECK(count_crossings(result) == 0);
        check_rank_permutations(ranked, result);
    }
}

TEST_CASE("Minimize crossings with repeated internal fanins", "[mincross]")
{
    technology_network tec{};
    const auto         a = tec.create_pi();
    const auto         b = tec.create_pi();
    const auto         c = tec.create_pi();
    const auto         d = tec.create_pi();
    const auto         w = tec.create_and(d, a);
    const auto         x = tec.create_and(b, b);
    const auto         y = tec.create_and(a, a);
    const auto         z = tec.create_and(a, c);
    tec.create_po(tec.create_and(w, x));
    tec.create_po(tec.create_and(x, y));
    tec.create_po(tec.create_and(y, z));
    tec.create_po(tec.create_and(z, w));
    auto           ranked = mutable_rank_view(tec);
    mincross_stats st{};
    const auto     result = mincross(ranked, {}, &st);

    // Exhaustive permutations of the three four-node ranks have a minimum of three crossings.
    CHECK(st.num_crossings == 3);
    CHECK(count_crossings(result) == 3);
}

TEST_CASE("Minimize crossings when primary inputs and gates are interleaved", "[mincross]")
{
    technology_network tec{};
    const auto         a = tec.create_pi();
    const auto         b = tec.create_pi();
    const auto         x = tec.create_and(a, b);
    const auto         c = tec.create_pi();
    const auto         y = tec.create_and(b, c);
    tec.create_po(x);
    tec.create_po(y);
    auto ranked = mutable_rank_view(tec);
    ranked.set_ranks(1, {ranked.get_node(y), ranked.get_node(x)});

    CHECK(count_crossings(ranked) == 3);
    mincross_stats st{};
    const auto     result = mincross(ranked, mincross_params{.fixed_pis = true}, &st);
    CHECK(st.num_crossings == 0);
    CHECK(count_crossings(result) == 0);
    check_rank_permutations(ranked, result);
}
