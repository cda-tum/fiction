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
 * @brief Tests for `fiction/synthesis/node_duplication_planarization.hpp`.
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/synthesis/crossing_gate_planarization.hpp>
#include <fiction/synthesis/delete_virtual_pis.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/utils/graph/mincross.hpp>
#include <fiction/verification/virtual_miter.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
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
 * Substitutes fanouts, balances with unified outputs, and ranks the given network.
 */
template <typename Ntk>
mutable_rank_view<technology_network> prepare(const Ntk& ntk)
{
    network_balancing_params ps{};
    ps.unify_outputs           = true;
    ps.buffer_constant_outputs = false;

    return mutable_rank_view{network_balancing<technology_network>(fanout_substitution<technology_network>(ntk), ps)};
}

/**
 * Balances with unified outputs and ranks the given network without substituting fanouts.
 */
template <typename Ntk>
mutable_rank_view<technology_network> rank_without_substitution(const Ntk& ntk)
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
 * Checks that the planarized network is crossing-free and computes the same functions as the original.
 */
template <typename Spec, typename Impl>
void check_planar_and_equivalent(const Spec& spec, const Impl& impl)
{
    CHECK(count_crossings(impl) == 0);
    check_equivalent(spec, impl);
}

}  // namespace

TEST_CASE("Unbalanced networks are rejected", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto b1 = tec.create_buf(x2);
    const auto f1 = tec.create_and(x1, b1);

    tec.create_po(f1);

    const mutable_rank_view ranked{tec};

    CHECK_THROWS_AS(node_duplication_planarization(ranked), std::invalid_argument);
}

TEST_CASE("Planarize a technology network with a shared fanin", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();
    const auto f1 = tec.create_not(x2);
    const auto f2 = tec.create_nary_and({x1, x2, x3, x4});
    const auto f3 = tec.create_nary_or({x3, x4, x5});
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    const auto ranked = prepare(tec);

    CHECK(count_crossings(ranked) > 0);

    node_duplication_planarization_stats st{};
    const auto                           planar = node_duplication_planarization(ranked, {}, &st);

    check_planar_and_equivalent(tec, planar);
    CHECK(planar.num_virtual_pis() > 0);
    CHECK(st.num_duplications == planar.size() - ranked.size());
}

TEST_CASE("Planarize an AIG with a majority gate", "[node-duplication-planarization]")
{
    mockturtle::aig_network aig{};

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    const auto x4 = aig.create_pi();
    const auto x5 = aig.create_pi();
    const auto f1 = aig.create_not(x2);
    const auto f2 = aig.create_nary_and({x1, x2, x3, x4});
    const auto f3 = aig.create_nary_or({x3, x4, x5});
    const auto f4 = aig.create_maj(x1, x2, f3);
    aig.create_po(f1);
    aig.create_po(f2);
    aig.create_po(f3);
    aig.create_po(f4);

    const auto planar = node_duplication_planarization(prepare(aig));

    check_planar_and_equivalent(aig, planar);
}

TEST_CASE("Planarize a two-output AIG", "[node-duplication-planarization]")
{
    mockturtle::aig_network aig{};

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto f1 = aig.create_and(x1, x2);
    const auto f2 = aig.create_or(x1, x2);
    aig.create_po(f1);
    aig.create_po(f2);

    const auto planar = node_duplication_planarization(prepare(aig));

    check_planar_and_equivalent(aig, planar);
}

TEST_CASE("Planarize a network whose outputs share one gate", "[node-duplication-planarization]")
{
    mockturtle::aig_network aig{};

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto a1 = aig.create_and(x1, x2);

    aig.create_po(a1);
    aig.create_po(a1);
    aig.create_po(a1);
    aig.create_po(a1);

    const auto planar = node_duplication_planarization(prepare(aig));

    check_planar_and_equivalent(aig, planar);
}

TEST_CASE("Planarize blueprint networks", "[node-duplication-planarization]")
{
    SECTION("full adder")
    {
        const auto ntk    = blueprints::full_adder_network<mockturtle::aig_network>();
        const auto planar = node_duplication_planarization(prepare(ntk));

        check_planar_and_equivalent(ntk, planar);
    }
    SECTION("multiplexer")
    {
        const auto ntk    = blueprints::mux21_network<technology_network>();
        const auto planar = node_duplication_planarization(prepare(ntk));

        check_planar_and_equivalent(ntk, planar);
    }
    SECTION("parity")
    {
        const auto ntk    = blueprints::parity_network<technology_network>();
        const auto planar = node_duplication_planarization(prepare(ntk));

        check_planar_and_equivalent(ntk, planar);
    }
}

TEST_CASE("Random primary output order with a fixed seed", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::full_adder_network<mockturtle::aig_network>();
    const auto ranked = prepare(ntk);

    node_duplication_planarization_params ps{};
    ps.po_order = node_duplication_planarization_params::output_order::RANDOM_PO_ORDER;
    ps.seed     = 42;

    const auto planar_a = node_duplication_planarization(ranked, ps);
    const auto planar_b = node_duplication_planarization(ranked, ps);

    check_planar_and_equivalent(ntk, planar_a);
    CHECK(planar_a.size() == planar_b.size());
}

TEST_CASE("Progress is reported once per level", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::full_adder_network<mockturtle::aig_network>();
    const auto ranked = prepare(ntk);

    std::vector<std::size_t> done_values{};
    std::size_t              total = 0;

    node_duplication_planarization_params ps{};
    ps.on_progress = [&done_values, &total](std::string_view, const std::size_t done, const std::size_t t)
    {
        done_values.push_back(done);
        total = t;
    };

    const auto planar = node_duplication_planarization(ranked, ps);

    check_planar_and_equivalent(ntk, planar);
    CHECK(total == ranked.depth() + 1);
    REQUIRE(!done_values.empty());
    CHECK(done_values.back() == total);
}

TEST_CASE("Asymmetric gates keep their fanin order", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();

    // every gate is asymmetric, so swapped fanins change the function
    const auto f1 = tec.create_lt(x1, x2);
    const auto f2 = tec.create_gt(x2, x3);
    const auto f3 = tec.create_lt(x3, x4);
    const auto f4 = tec.create_gt(x4, x1);
    const auto g1 = tec.create_lt(f1, f3);
    const auto g2 = tec.create_gt(f2, f4);
    const auto g3 = tec.create_lt(f4, f1);
    const auto h1 = tec.create_ite(g1, g2, g3);

    tec.create_po(h1);
    tec.create_po(g3);
    tec.create_po(g1);

    const auto ranked = prepare(tec);

    CHECK(count_crossings(ranked) > 0);

    const auto planar = node_duplication_planarization(ranked);

    check_planar_and_equivalent(tec, planar);
}

TEST_CASE("Consecutive consumers share one copy", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();

    // one primary input feeds three gates that end up next to each other
    const auto f1 = tec.create_not(x1);
    const auto f2 = tec.create_buf(x1);
    const auto f3 = tec.create_not(x1);

    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    network_balancing_params ps{};
    ps.unify_outputs           = true;
    ps.buffer_constant_outputs = false;

    // balance without fanout substitution: the input keeps nodes with fanout greater than one
    const mutable_rank_view ranked{network_balancing<technology_network>(tec, ps)};

    node_duplication_planarization_stats st{};
    const auto                           planar = node_duplication_planarization(ranked, {}, &st);

    check_planar_and_equivalent(tec, planar);

    // all three consumers share the one primary input: no duplicate, no virtual input
    CHECK(planar.num_virtual_pis() == 0);
    CHECK(st.num_duplications == 0);
    CHECK(planar.size() == ranked.size());

    // the result is not fanout-substituted on purpose
    CHECK(!is_fanout_substituted(planar));
}

TEST_CASE("Networks with virtual primary inputs are rejected", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::full_adder_network<mockturtle::aig_network>();
    const auto planar = node_duplication_planarization(prepare(ntk));
    REQUIRE(planar.num_virtual_pis() > 0);

    CHECK_THROWS_AS(node_duplication_planarization(planar), std::invalid_argument);

    // deleting the virtual inputs gives a plain network equivalent to the original
    const auto flat = delete_virtual_pis(planar);
    CHECK(flat.num_pis() == ntk.num_pis());
    check_equivalent(ntk, flat);
}

TEST_CASE("Statistics report the number of duplications", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = prepare(ntk);

    node_duplication_planarization_stats st{};
    const auto                           planar = node_duplication_planarization(ranked, {}, &st);

    CHECK(st.num_duplications == planar.size() - ranked.size());
    CHECK(st.num_duplications >= planar.num_virtual_pis());

    std::ostringstream os{};
    st.report(os);
    CHECK(os.str().find("duplications") != std::string::npos);
}

TEST_CASE("Hybrid strategy keeps crossings only where gadgets are cheaper", "[node-duplication-planarization]")
{
    for (const auto& ntk : {blueprints::full_adder_network<technology_network>(),
                            blueprints::parity_network<technology_network>(), blueprints::clpl<technology_network>()})
    {
        const auto ranked = rank_without_substitution(ntk);

        for (const bool xor_gates : {false, true})
        {
            node_duplication_planarization_params ps{};
            ps.strategy  = node_duplication_planarization_params::planarization_strategy::HYBRID;
            ps.xor_gates = xor_gates;

            node_duplication_planarization_stats st{};
            const auto                           hybrid = node_duplication_planarization(ranked, ps, &st);

            // the hybrid result is equivalent and balanced, and crossing-free iff no level kept its crossings
            check_equivalent(ntk, hybrid);
            CHECK(is_balanced(hybrid, {.unify_outputs = true, .buffer_constant_outputs = false}));
            CHECK((count_crossings(hybrid) == 0) == (st.num_crossing_levels == 0));
            CHECK((st.num_crossings == 0) == (st.num_crossing_levels == 0));

            // the crossing gates finish the job
            crossing_gate_planarization_params cg_ps{};
            cg_ps.xor_gates = xor_gates;

            crossing_gate_planarization_stats cg_st{};
            const auto                        planar = crossing_gate_planarization(hybrid, cg_ps, &cg_st);

            check_planar_and_equivalent(ntk, planar);
            CHECK(is_balanced(planar, {.unify_outputs = true, .buffer_constant_outputs = false}));
            CHECK(cg_st.num_crossings >= st.num_crossings);
        }
    }
}

TEST_CASE("Hybrid strategy with an infinite crossing cost duplicates everything", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = rank_without_substitution(ntk);

    node_duplication_planarization_stats dup_st{};
    const auto                           duplicated = node_duplication_planarization(ranked, {}, &dup_st);

    node_duplication_planarization_params ps{};
    ps.strategy               = node_duplication_planarization_params::planarization_strategy::HYBRID;
    ps.max_crossings_per_rank = 0;

    node_duplication_planarization_stats st{};
    const auto                           hybrid = node_duplication_planarization(ranked, ps, &st);

    check_planar_and_equivalent(ntk, hybrid);
    CHECK(st.num_crossing_levels == 0);
    CHECK(hybrid.size() == duplicated.size());
    CHECK(st.num_duplications == dup_st.num_duplications);
}

TEST_CASE("Hybrid strategy with a prohibitive duplication cost keeps crossings", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = rank_without_substitution(ntk);

    node_duplication_planarization_params ps{};
    ps.strategy                     = node_duplication_planarization_params::planarization_strategy::HYBRID;
    ps.criterion                    = node_duplication_planarization_params::decision_criterion::WEIGHTED_CONE;
    ps.duplication_cost.node_weight = 1e6;

    node_duplication_planarization_stats st{};
    const auto                           hybrid = node_duplication_planarization(ranked, ps, &st);

    check_equivalent(ntk, hybrid);
    CHECK(st.num_crossing_levels > 0);
    CHECK(st.num_crossings > 0);
    CHECK(count_crossings(hybrid) == st.num_crossings);

    std::ostringstream os{};
    st.report(os);
    CHECK(os.str().find("crossing levels") != std::string::npos);
}

TEST_CASE("The duplication limit aborts the planarization", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = prepare(ntk);

    node_duplication_planarization_params ps{};
    ps.max_duplications = 1;

    CHECK_THROWS_AS(node_duplication_planarization(ranked, ps), std::runtime_error);

    ps.max_duplications = 0;
    CHECK_NOTHROW(node_duplication_planarization(ranked, ps));

    // the limit is checked at every duplicate, so a level cannot overshoot it: one below the actual count aborts
    node_duplication_planarization_stats st{};
    const auto                           planar = node_duplication_planarization(ranked, ps, &st);
    REQUIRE(st.num_duplications > 1);

    ps.max_duplications = st.num_duplications - 1;
    CHECK_THROWS_AS(node_duplication_planarization(ranked, ps), std::runtime_error);

    ps.max_duplications = st.num_duplications;
    CHECK_NOTHROW(node_duplication_planarization(ranked, ps));
    CHECK(planar.size() == ranked.size() + st.num_duplications);
}

TEST_CASE("Invalid parameters are rejected", "[node-duplication-planarization]")
{
    const auto ranked = prepare(blueprints::full_adder_network<technology_network>());

    SECTION("lookahead budget of zero")
    {
        node_duplication_planarization_params ps{};
        ps.lookahead_budget = 0;

        CHECK_THROWS_AS(node_duplication_planarization(ranked, ps), std::invalid_argument);
    }
    SECTION("negative, infinite, or undefined cost weights")
    {
        for (const double weight :
             {-1.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()})
        {
            node_duplication_planarization_params ps{};
            ps.duplication_cost.node_weight = weight;
            CHECK_THROWS_AS(node_duplication_planarization(ranked, ps), std::invalid_argument);

            ps                                = {};
            ps.duplication_cost.buffer_weight = weight;
            CHECK_THROWS_AS(node_duplication_planarization(ranked, ps), std::invalid_argument);
        }
    }
    SECTION("depth growth that is not positive")
    {
        node_duplication_planarization_params ps{};
        ps.duplication_cost.depth_growth = 0.0;

        CHECK_THROWS_AS(node_duplication_planarization(ranked, ps), std::invalid_argument);
    }
}

TEST_CASE("Outputs driven by inputs pass through unchanged", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    tec.create_po(x1);
    tec.create_po(tec.create_not(x2));
    tec.create_po(x1);

    const auto ranked = rank_without_substitution(tec);
    REQUIRE(ranked.depth() == 1);

    for (const auto strategy : {node_duplication_planarization_params::planarization_strategy::DUPLICATION,
                                node_duplication_planarization_params::planarization_strategy::HYBRID})
    {
        node_duplication_planarization_params ps{};
        ps.strategy = strategy;

        node_duplication_planarization_stats st{};
        const auto                           planar = node_duplication_planarization(ranked, ps, &st);

        check_planar_and_equivalent(tec, planar);
        CHECK(st.num_duplications == 0);
        CHECK(planar.num_pos() == 3);
        CHECK(planar.num_virtual_pis() == 0);
    }

    // an AIG puts its complemented outputs on the inputs themselves
    mockturtle::aig_network aig{};

    const auto a = aig.create_pi();
    const auto b = aig.create_pi();
    aig.create_po(a);
    aig.create_po(!b);
    aig.create_po(!a);

    const auto planar = node_duplication_planarization(rank_without_substitution(aig));

    check_planar_and_equivalent(aig, planar);
    CHECK(planar.num_pos() == 3);
}

TEST_CASE("Primary inputs without fanout are kept in rank 0", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto unused_1 = tec.create_pi();
    const auto x1       = tec.create_pi();
    const auto x2       = tec.create_pi();
    const auto unused_2 = tec.create_pi();
    const auto x3       = tec.create_pi();
    tec.create_po(tec.create_and(x1, x2));
    tec.create_po(tec.create_or(x2, x3));
    tec.create_po(tec.create_and(x1, x3));

    const auto ranked = rank_without_substitution(tec);

    node_duplication_planarization_stats st{};
    const auto                           planar = node_duplication_planarization(ranked, {}, &st);

    check_planar_and_equivalent(tec, planar);
    CHECK(planar.check_validity());
    CHECK(planar.num_real_pis() == 5);
    CHECK(planar.rank_width(0) == planar.num_pis());
    planar.foreach_pi([&planar](const auto& n) { CHECK(planar.has_level(n)); });

    // the unused inputs stay real inputs that drive nothing
    uint32_t unused = 0;
    planar.foreach_pi(
        [&planar, &unused](const auto& n)
        {
            if (planar.fanout_size(n) == 0)
            {
                ++unused;
            }
        });
    CHECK(unused == 2);
    CHECK(tec.fanout_size(tec.get_node(unused_1)) == 0);
    CHECK(tec.fanout_size(tec.get_node(unused_2)) == 0);
}

TEST_CASE("The shortest path through the H-graph places the fewest nodes", "[node-duplication-planarization]")
{
    // for a network of one gate level, the width of rank 0 after the duplication is the number of placements on the
    // shortest path; a brute force over every fanin ordering of every output gives the minimum to compare with
    std::mt19937 generator{42};
    const auto   draw = [&generator](const uint32_t lo, const uint32_t hi)
    { return std::uniform_int_distribution<uint32_t>{lo, hi}(generator); };

    for (uint32_t trial = 0; trial < 60; ++trial)
    {
        technology_network tec{};

        const auto                                          num_pis = draw(2, 5);
        std::vector<mockturtle::signal<technology_network>> pis{};
        for (uint32_t i = 0; i < num_pis; ++i)
        {
            pis.push_back(tec.create_pi());
        }

        // outputs with one to three distinct inputs each
        const auto                         num_pos = draw(1, 5);
        std::vector<std::vector<uint32_t>> fanins_of_po{};
        for (uint32_t i = 0; i < num_pos; ++i)
        {
            std::vector<uint32_t> indices{};
            const auto            count = std::min(draw(1, 3), num_pis);
            while (indices.size() < count)
            {
                const auto idx = draw(0, num_pis - 1);
                if (std::find(indices.cbegin(), indices.cend(), idx) == indices.cend())
                {
                    indices.push_back(idx);
                }
            }

            if (indices.size() == 1)
            {
                tec.create_po(tec.create_not(pis[indices[0]]));
            }
            else if (indices.size() == 2)
            {
                tec.create_po(tec.create_xor(pis[indices[0]], pis[indices[1]]));
            }
            else
            {
                tec.create_po(tec.create_maj(pis[indices[0]], pis[indices[1]], pis[indices[2]]));
            }

            fanins_of_po.push_back(std::move(indices));
        }

        const mutable_rank_view ranked{tec};
        REQUIRE(ranked.depth() == 1);

        // the outputs are ordered in rank order, which is their creation order here
        std::vector<std::vector<uint32_t>> orders{};
        ranked.foreach_node_in_rank(
            1,
            [&ranked, &tec, &pis, &orders](const auto& n)
            {
                std::vector<uint32_t> indices{};
                ranked.foreach_fanin(n,
                                     [&tec, &pis, &indices](const auto& f)
                                     {
                                         const auto it =
                                             std::find_if(pis.cbegin(), pis.cend(), [&tec, &f](const auto& pi)
                                                          { return tec.get_node(pi) == tec.get_node(f); });
                                         indices.push_back(static_cast<uint32_t>(std::distance(pis.cbegin(), it)));
                                     });
                std::sort(indices.begin(), indices.end());
                orders.push_back(std::move(indices));
            });
        REQUIRE(orders.size() == num_pos);

        // brute force: every permutation of every output's fanins; consecutive outputs share one placement when
        // the last fanin of one equals the first of the next
        uint32_t best = std::numeric_limits<uint32_t>::max();

        const auto search = [&orders, &best](auto& self, const std::size_t po, const uint32_t placed,
                                             const uint32_t last) -> void
        {
            if (po == orders.size())
            {
                best = std::min(best, placed);
                return;
            }

            auto perm = orders[po];
            do
            {
                const uint32_t shared = (po > 0 && perm.front() == last) ? 1 : 0;
                self(self, po + 1, placed + static_cast<uint32_t>(perm.size()) - shared, perm.back());
            } while (std::next_permutation(perm.begin(), perm.end()));
        };
        search(search, 0, 0, std::numeric_limits<uint32_t>::max());

        // inputs that no output reads are placed once as well
        std::vector<bool> used(num_pis, false);
        for (const auto& fanins : fanins_of_po)
        {
            for (const auto idx : fanins)
            {
                used[idx] = true;
            }
        }
        const auto unused = static_cast<uint32_t>(std::count(used.cbegin(), used.cend(), false));

        const auto planar = node_duplication_planarization(ranked);

        check_planar_and_equivalent(tec, planar);
        CHECK(planar.rank_width(0) == best + unused);
    }
}

TEST_CASE("Hybrid strategy with lookahead", "[node-duplication-planarization]")
{
    for (const auto& ntk : {blueprints::full_adder_network<technology_network>(),
                            blueprints::parity_network<technology_network>(), blueprints::clpl<technology_network>()})
    {
        const auto ranked = rank_without_substitution(ntk);

        node_duplication_planarization_params ps{};
        ps.strategy  = node_duplication_planarization_params::planarization_strategy::HYBRID;
        ps.criterion = node_duplication_planarization_params::decision_criterion::LOOKAHEAD;
        ps.xor_gates = true;

        node_duplication_planarization_stats st{};
        const auto                           hybrid = node_duplication_planarization(ranked, ps, &st);

        check_equivalent(ntk, hybrid);
        CHECK((count_crossings(hybrid) == 0) == (st.num_crossing_levels == 0));

        crossing_gate_planarization_params cg_ps{};
        cg_ps.xor_gates = true;

        const auto planar = crossing_gate_planarization(hybrid, cg_ps);

        check_planar_and_equivalent(ntk, planar);
        CHECK(is_balanced(planar, {.unify_outputs = true, .buffer_constant_outputs = false}));
    }
}
TEST_CASE("Balanced networks without unified outputs are rejected", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto a1 = tec.create_and(x1, x2);

    tec.create_po(a1);
    tec.create_po(tec.create_not(a1));

    const mutable_rank_view ranked{network_balancing<technology_network>(tec)};
    REQUIRE(is_balanced(ranked));
    REQUIRE(!is_balanced(ranked, {.unify_outputs = true, .buffer_constant_outputs = false}));

    CHECK_THROWS_AS(node_duplication_planarization(ranked), std::invalid_argument);
}

TEST_CASE("Gates with the same fanin twice", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto f1 = tec.create_and(x1, x1);
    const auto f2 = tec.create_or(f1, x2);

    tec.create_po(f2);
    tec.create_po(tec.create_not(f1));

    const auto ranked = rank_without_substitution(tec);
    const auto planar = node_duplication_planarization(ranked);

    check_planar_and_equivalent(tec, planar);
}

TEST_CASE("Hybrid strategy falls back to the cone model when the lookahead budget is exhausted",
          "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto ranked = rank_without_substitution(ntk);

    node_duplication_planarization_params ps{};
    ps.strategy         = node_duplication_planarization_params::planarization_strategy::HYBRID;
    ps.criterion        = node_duplication_planarization_params::decision_criterion::LOOKAHEAD;
    ps.lookahead_budget = 1;

    node_duplication_planarization_stats st{};
    const auto                           hybrid = node_duplication_planarization(ranked, ps, &st);

    check_equivalent(ntk, hybrid);
    CHECK((count_crossings(hybrid) == 0) == (st.num_crossing_levels == 0));

    ps.criterion = node_duplication_planarization_params::decision_criterion::WEIGHTED_CONE;
    node_duplication_planarization_stats cone_st{};
    const auto                           cone = node_duplication_planarization(ranked, ps, &cone_st);

    // with a budget of one node every lookahead is cut off, so the decisions are the cone model's
    CHECK(hybrid.size() == cone.size());
    CHECK(st.num_crossing_levels == cone_st.num_crossing_levels);
}

TEST_CASE("Random primary output order without a seed", "[node-duplication-planarization]")
{
    const auto ntk    = blueprints::full_adder_network<mockturtle::aig_network>();
    const auto ranked = prepare(ntk);

    node_duplication_planarization_params ps{};
    ps.po_order = node_duplication_planarization_params::output_order::RANDOM_PO_ORDER;

    const auto planar = node_duplication_planarization(ranked, ps);

    check_planar_and_equivalent(ntk, planar);
}

TEST_CASE("Tie-break among equally short paths copies the lightest cone", "[node-duplication-planarization]")
{
    // two consumers share a gate whose cone holds three nodes and an inverter whose cone holds two; one of the two is
    // placed once between the consumers, the other is copied, so both shortest paths duplicate one node on this
    // level. Balancing ranks the fanins in the order the first output's cone visits them, so the fanin order of the
    // consumers puts the inverter first or second in rank order; the copy must be the inverter either way.
    const auto build = [](const bool inverter_first)
    {
        technology_network tec{};

        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto x3 = tec.create_pi();
        const auto g  = tec.create_and(x1, x2);
        const auto s  = tec.create_not(x3);

        if (inverter_first)
        {
            tec.create_po(tec.create_and(s, g));
            tec.create_po(tec.create_or(s, g));
        }
        else
        {
            tec.create_po(tec.create_and(g, s));
            tec.create_po(tec.create_or(g, s));
        }

        return tec;
    };

    for (const bool inverter_first : {true, false})
    {
        const auto tec    = build(inverter_first);
        const auto ranked = rank_without_substitution(tec);

        node_duplication_planarization_stats st{};
        const auto                           planar = node_duplication_planarization(ranked, {}, &st);

        check_planar_and_equivalent(tec, planar);

        // the inverter and its input are copied, not the gate with its two inputs
        CHECK(st.num_duplications == 2);
        CHECK(planar.num_virtual_pis() == 1);
        CHECK(planar.size() == tec.size() + 2);
    }
}

TEST_CASE("Constant fanins are kept", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto a1 = tec.create_and(x1, tec.get_constant(true));
    const auto o1 = tec.create_or(a1, x2);
    tec.create_po(o1);
    tec.create_po(tec.create_and(x1, x2));

    const auto ranked = rank_without_substitution(tec);
    const auto planar = node_duplication_planarization(ranked);

    check_planar_and_equivalent(tec, planar);
}
