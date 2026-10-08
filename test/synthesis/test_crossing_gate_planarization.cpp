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
 * @brief Tests for `fiction/synthesis/crossing_gate_planarization.hpp`.
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/crossing_gate_planarization.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/utils/graph/mincross.hpp>
#include <fiction/verification/virtual_miter.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>

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
 * Checks the contract: balanced with unified outputs, crossing-free, equivalent.
 */
template <typename Spec, typename Impl>
void check_contract(const Spec& spec, const Impl& impl)
{
    CHECK(is_balanced(impl, {.unify_outputs = true, .buffer_constant_outputs = false}));
    CHECK(count_crossings(impl) == 0);
    check_equivalent(spec, impl);
}

/**
 * A network with a single crossing between its inputs and its gates.
 */
technology_network single_crossing_network()
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    // gate order g1, g2 while x1 feeds g2 and x2 feeds g1
    const auto g1 = tec.create_not(x2);
    const auto g2 = tec.create_buf(x1);

    tec.create_po(g1);
    tec.create_po(g2);

    return tec;
}

}  // namespace

TEST_CASE("Unbalanced networks are rejected", "[crossing-gate-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    tec.create_po(tec.create_and(x1, tec.create_buf(x2)));

    const mutable_rank_view ranked{tec};

    CHECK_THROWS_AS(crossing_gate_planarization(ranked), std::invalid_argument);
}

TEST_CASE("A single crossing becomes one gadget", "[crossing-gate-planarization]")
{
    const auto tec    = single_crossing_network();
    const auto ranked = rank(tec);
    REQUIRE(count_crossings(ranked) == 1);

    SECTION("AND, OR, and NOT gates")
    {
        crossing_gate_planarization_stats st{};
        const auto                        planar = crossing_gate_planarization(ranked, {}, &st);

        check_contract(tec, planar);
        CHECK(st.num_crossings == 1);
        CHECK(planar.depth() == ranked.depth() + 14);
    }
    SECTION("XOR gates")
    {
        crossing_gate_planarization_params ps{};
        ps.xor_gates = true;

        crossing_gate_planarization_stats st{};
        const auto                        planar = crossing_gate_planarization(ranked, ps, &st);

        check_contract(tec, planar);
        CHECK(st.num_crossings == 1);
        CHECK(planar.depth() == ranked.depth() + 4);

        std::ostringstream os{};
        st.report(os);
        CHECK(os.str().find("crossings") != std::string::npos);
    }
}

TEST_CASE("Planar networks are copied without gadgets", "[crossing-gate-planarization]")
{
    const auto ntk    = blueprints::full_adder_network<technology_network>();
    const auto planar = node_duplication_planarization(rank(ntk));
    REQUIRE(count_crossings(planar) == 0);

    crossing_gate_planarization_stats st{};
    const auto                        result = crossing_gate_planarization(planar, {}, &st);

    check_contract(ntk, result);
    CHECK(st.num_crossings == 0);
    CHECK(result.size() == planar.size());
    CHECK(result.num_virtual_pis() == planar.num_virtual_pis());
}

TEST_CASE("Blueprint networks with several crossings", "[crossing-gate-planarization]")
{
    for (const auto& ntk : {blueprints::full_adder_network<technology_network>(),
                            blueprints::mux21_network<technology_network>(), blueprints::clpl<technology_network>()})
    {
        const auto ranked = rank(ntk);
        REQUIRE(count_crossings(ranked) > 0);

        for (const bool xor_gates : {false, true})
        {
            crossing_gate_planarization_params ps{};
            ps.xor_gates = xor_gates;

            crossing_gate_planarization_stats st{};
            const auto                        planar = crossing_gate_planarization(ranked, ps, &st);

            check_contract(ntk, planar);
            CHECK(st.num_crossings >= count_crossings(ranked));
            CHECK(planar.size() > ranked.size());
        }
    }
}

TEST_CASE("Ranks with too many crossings are rejected", "[crossing-gate-planarization]")
{
    const auto ranked = rank(blueprints::clpl<technology_network>());
    REQUIRE(count_crossings(ranked) > 1);

    crossing_gate_planarization_params ps{};
    ps.max_crossings_per_rank = 0;

    CHECK_THROWS_AS(crossing_gate_planarization(ranked, ps), std::runtime_error);
}

TEST_CASE("Progress is reported", "[crossing-gate-planarization]")
{
    const auto tec    = single_crossing_network();
    const auto ranked = rank(tec);

    std::vector<std::size_t> done{};
    std::size_t              total = 0;

    crossing_gate_planarization_params ps{};
    ps.on_progress = [&done, &total](std::string_view, const std::size_t d, const std::size_t t)
    {
        done.push_back(d);
        total = t;
    };

    const auto planar = crossing_gate_planarization(ranked, ps);

    check_contract(tec, planar);
    CHECK(total == ranked.depth());
    REQUIRE(!done.empty());
    CHECK(done.back() == total);
    // the reporter may throttle intermediate values; only the total and the final count are promised
}
