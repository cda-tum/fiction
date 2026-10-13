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
 * @brief Tests for `fiction/synthesis/planarization.hpp`.
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/synthesis/planarization.hpp>
#include <fiction/utils/graph/mincross.hpp>
#include <fiction/verification/virtual_miter.hpp>

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::utils::graph;
using namespace fiction::verification;

namespace
{

/**
 * Balances with unified outputs and ranks the given network without substituting fanouts.
 */
template <typename Ntk>
mutable_rank_view<technology_network> rank(const Ntk& ntk)
{
    network_balancing_params ps{};
    ps.unify_outputs           = true;
    ps.buffer_constant_outputs = false;

    return mutable_rank_view{network_balancing<technology_network>(ntk, ps)};
}

/**
 * Counts the crossings of a ranked network without reordering it.
 */
template <typename Ntk>
uint64_t count_crossings(const Ntk& ntk)
{
    mincross_params ps{};
    ps.optimize = false;
    mincross_stats st{};

    mincross(ntk, ps, &st);

    return st.num_crossings;
}

/**
 * Checks that the network computes the same functions as the original, virtual primary inputs included.
 */
template <typename Spec, typename Impl>
void check_equivalent(const Spec& spec, const Impl& impl)
{
    const auto miter = virtual_miter<technology_network>(spec, impl);
    REQUIRE(miter.has_value());

    if (miter.has_value())  // the optional-access check does not model REQUIRE
    {
        mockturtle::equivalence_checking_stats st{};
        const auto                             cec = mockturtle::equivalence_checking(*miter, {}, &st);

        REQUIRE(cec.has_value());
        CHECK(cec.value_or(false));
    }
}

/**
 * Checks the contract of the pipeline: planar, balanced with unified outputs, fanout-substituted, no dangling nodes,
 * equivalent.
 */
template <typename Spec, typename Impl>
void check_contract(const Spec& spec, const Impl& impl, const uint32_t degree = 2)
{
    CHECK(count_crossings(impl) == 0);
    CHECK(is_balanced(impl, {.unify_outputs = true, .buffer_constant_outputs = false}));
    CHECK(is_fanout_substituted(impl, {.degree = degree}));
    CHECK(mockturtle::cleanup_dangling(impl).size() == impl.size());
    check_equivalent(spec, impl);
}

}  // namespace

TEST_CASE("Unbalanced networks are rejected", "[planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    tec.create_po(tec.create_and(x1, tec.create_buf(x2)));

    const mutable_rank_view ranked{tec};

    CHECK_THROWS_AS(planarization(ranked), std::invalid_argument);
}

TEST_CASE("Every strategy yields a planar, balanced, fanout-substituted network", "[planarization]")
{
    using params = node_duplication_planarization_params;

    for (const auto& ntk : {blueprints::full_adder_network<technology_network>(),
                            blueprints::parity_network<technology_network>(), blueprints::clpl<technology_network>()})
    {
        const auto ranked = rank(ntk);

        for (const auto strategy :
             {params::planarization_strategy::DUPLICATION, params::planarization_strategy::HYBRID})
        {
            for (const auto criterion :
                 {params::decision_criterion::WEIGHTED_CONE, params::decision_criterion::LOOKAHEAD})
            {
                for (const bool xor_gates : {false, true})
                {
                    planarization_params ps{};
                    ps.duplication.strategy  = strategy;
                    ps.duplication.criterion = criterion;
                    ps.duplication.xor_gates = xor_gates;

                    planarization_stats st{};
                    const auto          result = planarization(ranked, ps, &st);

                    check_contract(ntk, result);
                    CHECK(st.num_nodes == result.size());
                    CHECK((st.crossing_gates.num_crossings > 0) == (st.duplication.num_crossing_levels > 0));
                }
            }
        }
    }
}

TEST_CASE("The fanout degree is respected", "[planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = rank(ntk);

    for (const uint32_t degree : {2u, 3u})
    {
        planarization_params ps{};
        ps.fanout_degree = degree;

        const auto result = planarization(ranked, ps);

        check_contract(ntk, result, degree);
    }
}

TEST_CASE("Fanout-substituted inputs are accepted", "[planarization]")
{
    const auto ntk = blueprints::full_adder_network<mockturtle::aig_network>();

    const auto ranked = mutable_rank_view{network_balancing<technology_network>(
        fanout_substitution<technology_network>(ntk), {.unify_outputs = true, .buffer_constant_outputs = false})};

    planarization_stats st{};
    const auto          result = planarization(ranked, {}, &st);

    check_contract(ntk, result);

    std::ostringstream os{};
    st.report(os);
    CHECK(os.str().find("num. nodes") != std::string::npos);
}

TEST_CASE("Progress is reported for every stage", "[planarization]")
{
    const auto ntk    = blueprints::full_adder_network<technology_network>();
    const auto ranked = rank(ntk);

    std::vector<std::string> tasks{};

    planarization_params ps{};
    ps.on_progress = [&tasks](const std::string_view task, const std::size_t, const std::size_t)
    {
        if (tasks.empty() || tasks.back() != task)
        {
            tasks.emplace_back(task);
        }
    };

    const auto result = planarization(ranked, ps);

    check_contract(ntk, result);
    CHECK(!tasks.empty());
}

TEST_CASE("Statistics report the runtime and progress reaches every stage", "[planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = rank(ntk);

    std::vector<std::string> tasks{};

    planarization_params ps{};
    ps.duplication.strategy    = node_duplication_planarization_params::planarization_strategy::HYBRID;
    ps.duplication.xor_gates   = true;
    ps.duplication.on_progress = [&tasks](const std::string_view task, const std::size_t, const std::size_t)
    {
        if (tasks.empty() || tasks.back() != task)
        {
            tasks.emplace_back(task);
        }
    };

    planarization_stats st{};
    const auto          result = planarization(ranked, ps, &st);

    check_contract(ntk, result);
    // the callback on the duplication stage is used for the stages
    CHECK(!tasks.empty());
}

TEST_CASE("Kept crossings run through the crossing gates for both gadget types", "[planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = rank(ntk);

    for (const bool xor_gates : {false, true})
    {
        planarization_params ps{};
        ps.duplication.strategy  = node_duplication_planarization_params::planarization_strategy::HYBRID;
        ps.duplication.criterion = node_duplication_planarization_params::decision_criterion::WEIGHTED_CONE;
        ps.duplication.xor_gates = xor_gates;
        // a prohibitive duplication cost makes the hybrid strategy keep every crossing it may keep
        ps.duplication.duplication_cost.node_weight = 1e6;

        planarization_stats st{};
        const auto          result = planarization(ranked, ps, &st);

        REQUIRE(st.duplication.num_crossing_levels > 0);
        CHECK(st.crossing_gates.num_crossings > 0);
        check_contract(ntk, result);
    }
}

TEST_CASE("The lookahead budget fallback and the crossing limit are honored", "[planarization]")
{
    const auto ntk    = blueprints::clpl<technology_network>();
    const auto ranked = rank(ntk);

    SECTION("a tiny lookahead budget falls back to the cone model")
    {
        planarization_params ps{};
        ps.duplication.lookahead_budget = 1;

        const auto result = planarization(ranked, ps);
        check_contract(ntk, result);
    }
    SECTION("a crossing limit of zero duplicates every level")
    {
        planarization_params ps{};
        ps.duplication.max_crossings_per_rank = 0;

        planarization_stats st{};
        const auto          result = planarization(ranked, ps, &st);

        CHECK(st.duplication.num_crossing_levels == 0);
        check_contract(ntk, result);
    }
}

TEST_CASE("Invalid parameters are rejected before any stage runs", "[planarization]")
{
    const auto ranked = rank(blueprints::full_adder_network<technology_network>());

    SECTION("fanout degree below two")
    {
        planarization_params ps{};
        ps.fanout_degree = 1;

        CHECK_THROWS_AS(planarization(ranked, ps), std::invalid_argument);
    }
    SECTION("lookahead budget of zero")
    {
        planarization_params ps{};
        ps.duplication.lookahead_budget = 0;

        CHECK_THROWS_AS(planarization(ranked, ps), std::invalid_argument);
    }
    SECTION("negative cost weight")
    {
        planarization_params ps{};
        ps.duplication.duplication_cost.buffer_weight = -1.0;

        CHECK_THROWS_AS(planarization(ranked, ps), std::invalid_argument);
    }
}

TEST_CASE("Balanced networks without unified outputs are rejected", "[planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto a  = tec.create_and(x1, x2);
    tec.create_po(a);
    tec.create_po(tec.create_buf(a));

    const mutable_rank_view ranked{tec};
    REQUIRE(is_balanced(ranked));

    CHECK_THROWS_AS(planarization(ranked), std::invalid_argument);
}

TEST_CASE("Outputs driven by inputs, complemented outputs, and unused inputs pass through", "[planarization]")
{
    SECTION("outputs that are inputs")
    {
        technology_network tec{};

        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        tec.create_po(x1);
        tec.create_po(tec.create_not(x2));
        tec.create_po(x1);

        const auto ranked = rank(tec);
        const auto result = planarization(ranked);

        check_contract(tec, result);
        CHECK(result.num_pos() == 3);
    }
    SECTION("complemented outputs of an AIG")
    {
        mockturtle::aig_network aig{};

        const auto a = aig.create_pi();
        const auto b = aig.create_pi();
        const auto c = aig.create_pi();
        const auto f = aig.create_and(a, b);
        aig.create_po(!aig.create_and(f, c));
        aig.create_po(!f);

        const auto ranked = rank(aig);
        const auto result = planarization(ranked);

        check_contract(aig, result);
    }
    SECTION("an input that drives nothing is kept")
    {
        technology_network tec{};

        const auto unused = tec.create_pi();
        const auto x1     = tec.create_pi();
        const auto x2     = tec.create_pi();
        const auto x3     = tec.create_pi();
        tec.create_po(tec.create_and(x1, x2));
        tec.create_po(tec.create_or(x2, x3));
        tec.create_po(tec.create_and(x1, x3));

        const auto ranked = rank(tec);
        REQUIRE(ranked.rank_position(ranked.get_node(unused)) == 0);

        const auto result = planarization(ranked);

        check_contract(tec, result);
        CHECK(result.num_real_pis() == 4);
    }
}

TEST_CASE("Gates with a repeated fanin and shared output nodes are planarized", "[planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto m  = tec.create_maj(x1, x2, x3);
    // a gate that reads one node twice; the fanout node this needs has both outputs on one gate
    const auto a = tec.create_and(m, m);
    const auto o = tec.create_or(m, x3);
    tec.create_po(a);
    tec.create_po(o);
    // two outputs on one node
    tec.create_po(o);

    const auto ranked = rank(tec);
    const auto result = planarization(ranked);

    check_contract(tec, result);
    CHECK(result.num_pos() == 3);
}

/**
 * Builds a random balanced network of a few levels. Gates read nodes of the level below or, with `long_edges`, of
 * any lower level, which gives buffer chains after balancing. With `repeated_fanins`, a gate may read one node
 * twice and an output node may drive two outputs. Every gate except the outputs has a consumer; inputs may be
 * unused.
 */
technology_network random_network(const uint32_t seed, const bool long_edges, const bool repeated_fanins)
{
    std::mt19937 generator{seed};
    const auto   draw = [&generator](const uint32_t lo, const uint32_t hi)
    { return std::uniform_int_distribution<uint32_t>{lo, hi}(generator); };

    technology_network tec{};

    std::vector<std::vector<mockturtle::signal<technology_network>>> levels(1);
    for (uint32_t i = 0, n = draw(2, 5); i < n; ++i)
    {
        levels.back().push_back(tec.create_pi());
    }

    for (uint32_t l = 0, n = draw(1, 4); l < n; ++l)
    {
        const auto&       below = levels.back();
        std::vector<bool> consumed(below.size(), false);

        const auto pick = [&](const uint32_t count)
        {
            std::vector<mockturtle::signal<technology_network>> fanins{};

            for (uint32_t k = 0; k < count; ++k)
            {
                if (long_edges && levels.size() > 1 && draw(0, 2) == 0)
                {
                    const auto& lower = levels[draw(0, static_cast<uint32_t>(levels.size()) - 2)];
                    fanins.push_back(lower[draw(0, static_cast<uint32_t>(lower.size()) - 1)]);
                    continue;
                }

                auto idx = draw(0, static_cast<uint32_t>(below.size()) - 1);

                if (!repeated_fanins)
                {
                    for (uint32_t tries = 0;
                         tries < 8 && std::find(fanins.cbegin(), fanins.cend(), below[idx]) != fanins.cend(); ++tries)
                    {
                        idx = draw(0, static_cast<uint32_t>(below.size()) - 1);
                    }
                }

                consumed[idx] = true;
                fanins.push_back(below[idx]);
            }

            return fanins;
        };

        std::vector<mockturtle::signal<technology_network>> level{};

        for (uint32_t w = 0, width = draw(1, 5); w < width; ++w)
        {
            switch (const auto kind = draw(0, 4); kind)
            {
                case 0:
                {
                    const auto f = pick(2);
                    level.push_back(tec.create_and(f[0], f[1]));
                    break;
                }
                case 1:
                {
                    const auto f = pick(2);
                    level.push_back(tec.create_or(f[0], f[1]));
                    break;
                }
                case 2:
                {
                    const auto f = pick(2);
                    level.push_back(tec.create_xor(f[0], f[1]));
                    break;
                }
                case 3:
                {
                    level.push_back(tec.create_not(pick(1)[0]));
                    break;
                }
                default:
                {
                    if (below.size() >= 3)
                    {
                        const auto f = pick(3);
                        level.push_back(tec.create_maj(f[0], f[1], f[2]));
                    }
                    else
                    {
                        const auto f = pick(2);
                        level.push_back(tec.create_and(f[0], f[1]));
                    }
                    break;
                }
            }
        }

        // every gate of the level below needs a consumer; unused inputs are allowed
        if (levels.size() > 1)
        {
            for (std::size_t i = 0; i < consumed.size(); ++i)
            {
                if (!consumed[i])
                {
                    level.push_back(tec.create_buf(below[i]));
                }
            }
        }

        levels.push_back(std::move(level));
    }

    for (const auto& s : levels.back())
    {
        tec.create_po(s);
    }

    if (repeated_fanins)
    {
        tec.create_po(levels.back().front());
    }

    return tec;
}

TEST_CASE("Random networks keep the pipeline contract", "[planarization]")
{
    // long edges become buffer chains that the rebalancing removes and re-inserts; repeated fanins and shared output
    // nodes need one edge per fanin slot; both kinds also exercise the hybrid decisions on small levels
    for (const auto& [long_edges, repeated_fanins] :
         {std::pair{false, false}, std::pair{true, false}, std::pair{true, true}})
    {
        for (uint32_t seed = 0; seed < 40; ++seed)
        {
            const auto tec    = mockturtle::cleanup_dangling(random_network(seed, long_edges, repeated_fanins));
            const auto ranked = rank(tec);

            const auto result = planarization(ranked);

            check_contract(tec, result);
        }
    }
}

TEST_CASE("Networks with virtual primary inputs are rejected", "[planarization]")
{
    const auto ntk    = blueprints::full_adder_network<technology_network>();
    const auto planar = planarization(rank(ntk));
    REQUIRE(planar.num_virtual_pis() > 0);

    CHECK_THROWS_AS(planarization(planar), std::invalid_argument);
}
