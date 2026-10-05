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

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
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
    ps.unify_outputs = true;

    return mutable_rank_view{network_balancing<technology_network>(fanout_substitution<technology_network>(ntk), ps)};
}

/**
 * Balances with unified outputs and ranks the given network without substituting fanouts.
 */
template <typename Ntk>
mutable_rank_view<technology_network> rank_without_substitution(const Ntk& ntk)
{
    network_balancing_params ps{};
    ps.unify_outputs = true;

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
    ps.unify_outputs = true;

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
            CHECK(is_balanced(hybrid, {.unify_outputs = true}));
            CHECK((count_crossings(hybrid) == 0) == (st.num_crossing_levels == 0));
            CHECK((st.num_crossings == 0) == (st.num_crossing_levels == 0));

            // the crossing gates finish the job
            crossing_gate_planarization_params cg_ps{};
            cg_ps.xor_gates = xor_gates;

            crossing_gate_planarization_stats cg_st{};
            const auto                        planar = crossing_gate_planarization(hybrid, cg_ps, &cg_st);

            check_planar_and_equivalent(ntk, planar);
            CHECK(is_balanced(planar, {.unify_outputs = true}));
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
    ps.strategy              = node_duplication_planarization_params::planarization_strategy::HYBRID;
    ps.duplication_cost.base = 1e6;
    ps.cross_min             = node_duplication_planarization_params::crossing_minimization::BARYCENTER;

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
}
