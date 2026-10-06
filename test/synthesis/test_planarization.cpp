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
 * Balances with unified outputs and ranks the given network without substituting fanouts.
 */
template <typename Ntk>
mutable_rank_view<technology_network> rank(const Ntk& ntk)
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
 * Checks the contract of the pipeline: planar, balanced with unified outputs, fanout-substituted, no dangling nodes,
 * equivalent.
 */
template <typename Spec, typename Impl>
void check_contract(const Spec& spec, const Impl& impl, const uint32_t degree = 2)
{
    CHECK(count_crossings(impl) == 0);
    CHECK(is_balanced(impl, {.unify_outputs = true}));
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

    const auto ranked = mutable_rank_view{
        network_balancing<technology_network>(fanout_substitution<technology_network>(ntk), {.unify_outputs = true})};

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
    CHECK(tasks.size() >= 3);
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
    CHECK(st.time_total.count() > 0);
    // the callback on the duplication stage is used for every stage
    CHECK(tasks.size() >= 3);
}

TEST_CASE("Networks with virtual primary inputs are rejected", "[planarization]")
{
    const auto ntk    = blueprints::full_adder_network<technology_network>();
    const auto planar = planarization(rank(ntk));
    REQUIRE(planar.num_virtual_pis() > 0);

    CHECK_THROWS_AS(planarization(planar), std::invalid_argument);
}
