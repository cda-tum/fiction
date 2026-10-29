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
 * @brief Allocation-failure propagation and recovery for network balance checks.
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/network_balancing.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operators.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/traits.hpp>

#include <cstdint>
#include <new>
#include <tuple>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::test;

/** @brief Balance checks preserve the source and recover after allocation failures. */
TEMPLATE_TEST_CASE("Balance checks propagate allocation failures and recover", "[network-balancing]",
                   technology_network, mutable_rank_view<technology_network>)
{
    require_allocation_failure_support();
    const auto unify_outputs = GENERATE(false, true);
    CAPTURE(unify_outputs);

    bool canary_failed{};
    try
    {
        allocation_budget  = 0;
        auto* const memory = ::operator new(1);
        ::operator delete(memory);
    }
    catch (const std::bad_alloc&)
    {
        canary_failed = true;
    }
    catch (...)
    {
        allocation_budget.reset();
        throw;
    }
    allocation_budget.reset();
    REQUIRE(canary_failed);

    technology_network network{};
    const auto         a    = network.create_pi();
    const auto         b    = network.create_pi();
    const auto         gate = network.create_and(a, b);
    network.create_po(gate);
    network.create_po(gate);
    const TestType source{network};

    const auto observe = [&source]
    {
        std::vector<std::vector<typename TestType::signal>> edges{};
        std::vector<std::pair<uint32_t, uint32_t>>          ranks{};
        source.foreach_node(
            [&](const auto node)
            {
                edges.emplace_back();
                source.foreach_fanin(node, [&](const auto signal) { edges.back().push_back(signal); });
                if constexpr (mockturtle::has_rank_position_v<TestType>)
                {
                    if (!source.is_constant(node))
                    {
                        ranks.emplace_back(source.level(node), source.rank_position(node));
                    }
                }
            });
        edges.emplace_back();
        source.foreach_po([&](const auto signal) { edges.back().push_back(signal); });
        return std::tuple{source.num_pis(),
                          source.num_pos(),
                          source.num_gates(),
                          edges,
                          ranks,
                          mockturtle::simulate<kitty::dynamic_truth_table>(
                              source, mockturtle::default_simulator<kitty::dynamic_truth_table>{2})};
    };
    const auto                 before   = observe();
    const auto                 handlers = source.events().on_add.size();
    kitty::dynamic_truth_table expected{2};
    kitty::create_from_hex_string(expected, "8");
    REQUIRE(std::get<5>(before) == std::vector{expected, expected});

    bool succeeded{};
    for (auto budget = 0u; budget < ALLOCATION_FAILURE_ATTEMPT_LIMIT; ++budget)
    {
        bool failed{};
        bool balanced{};
        try
        {
            allocation_budget = budget;
            balanced          = is_balanced(source, {.unify_outputs = unify_outputs});
        }
        catch (const std::bad_alloc&)
        {
            failed = true;
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();

        CAPTURE(budget);
        CHECK(observe() == before);
        CHECK(source.events().on_add.size() == handlers);
        CHECK(is_balanced(source, {.unify_outputs = unify_outputs}));
        if (!failed)
        {
            CHECK(balanced);
            succeeded = true;
            break;
        }
    }
    CHECK(succeeded);
}
