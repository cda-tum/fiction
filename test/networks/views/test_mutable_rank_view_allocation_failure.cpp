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
 * @brief Allocation-failure recovery tests for mutable network ranks.
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/synthesis/delete_virtual_pis.hpp>

#include <kitty/bit_operations.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operators.hpp>
#include <mockturtle/algorithms/simulation.hpp>

#include <cstddef>
#include <new>
#include <optional>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::test;

TEST_CASE("Rank insertion propagates allocation failure and permits recomputation", "[mutable-rank-view]")
{
    require_allocation_failure_support();
    bool injected{};
    allocation_budget = 0;
    try
    {
        // An explicit allocator call tests injection without requiring rank insertion to allocate.
        auto* const allocation = ::operator new(1);
        allocation_budget.reset();
        ::operator delete(allocation);
    }
    catch (const std::bad_alloc&)
    {
        allocation_budget.reset();
        injected = true;
    }
    allocation_budget.reset();
    REQUIRE(injected);
    for (std::size_t failure{};; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        technology_network ntk{};
        const auto         input = ntk.create_pi();
        ntk.create_po(input);
        mutable_rank_view ranked{ntk};
        const auto        gate = ranked.create_not(input);
        ranked.create_po(gate);

        bool inserted{};
        allocation_budget = failure;
        try
        {
            ranked.on_add(ranked.get_node(gate));
            inserted = true;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();

        if (inserted)
        {
            CHECK(ranked.check_validity());
            CHECK(
                ranked.get_all_ranks() ==
                std::vector<std::vector<technology_network::node>>{{ranked.get_node(input)}, {ranked.get_node(gate)}});
        }

        ranked.update_ranks();
        CHECK(ranked.check_validity());
        CHECK(ranked.get_all_ranks() ==
              std::vector<std::vector<technology_network::node>>{{ranked.get_node(input)}, {ranked.get_node(gate)}});
        CHECK(ranked.depth() == 1);
        CHECK(ranked.width() == 1);
        CHECK(ranked.rank_position(ranked.get_node(input)) == 0);
        CHECK(ranked.rank_position(ranked.get_node(gate)) == 0);

        const auto next = ranked.create_not(gate);
        ranked.create_po(next);
        ranked.on_add(ranked.get_node(next));
        CHECK(ranked.check_validity());
        CHECK(ranked.get_all_ranks() == std::vector<std::vector<technology_network::node>>{{ranked.get_node(input)},
                                                                                           {ranked.get_node(gate)},
                                                                                           {ranked.get_node(next)}});
        CHECK(ranked.depth() == 2);
        CHECK(ranked.width() == 1);
        CHECK(ranked.rank_position(ranked.get_node(next)) == 0);
        if (inserted)
        {
            break;
        }
    }
}

TEST_CASE("Rank copies propagate allocation failure and preserve the source", "[mutable-rank-view]")
{
    require_allocation_failure_support();
    const bool all_ranks = GENERATE(false, true);
    CAPTURE(all_ranks);
    for (std::size_t failure{};; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        technology_network ntk{};
        const auto         input = ntk.create_pi();
        const auto         gate  = ntk.create_not(input);
        ntk.create_po(gate);
        const mutable_rank_view                                  ranked{ntk};
        const std::vector<std::vector<technology_network::node>> expected{{ntk.get_node(input)}, {ntk.get_node(gate)}};
        std::vector<technology_network::node>                    rank{};
        std::vector<std::vector<technology_network::node>>       ranks{};
        bool                                                     copied{};
        allocation_budget = failure;
        try
        {
            if (all_ranks)
            {
                ranks = ranked.get_all_ranks();
            }
            else
            {
                rank = ranked.get_ranks(1);
            }
            copied = true;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        if (copied)
        {
            if (all_ranks)
            {
                CHECK(ranks == expected);
                ranks.clear();
            }
            else
            {
                CHECK(rank == expected[1]);
                rank.clear();
            }
        }
        CHECK(ranked.check_validity());
        CHECK(ranked.get_all_ranks() == expected);
        CHECK(ranked.get_ranks(1) == expected[1]);
        CHECK(ranked.width() == 1);
        CHECK(ranked.depth() == 1);
        if (copied)
        {
            break;
        }
    }
}

TEST_CASE("Ranked virtual-PI removal propagates allocation failure and preserves the source", "[delete-virtual-pis]")
{
    require_allocation_failure_support();
    kitty::dynamic_truth_table source_function{3};
    kitty::create_from_hex_string(source_function, "c0");
    for (std::size_t failure{};; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        virtual_pi_network<technology_network> ntk{};
        const auto                             a     = ntk.create_pi();
        const auto                             b     = ntk.create_pi();
        const auto                             alias = ntk.create_virtual_pi(a);
        const auto                             gate  = ntk.create_and(alias, b);
        ntk.create_po(gate);
        mutable_rank_view                                        ranked{ntk};
        const std::vector<std::vector<technology_network::node>> expected{
            {ntk.get_node(a), ntk.get_node(b), ntk.get_node(alias)},
            {ntk.get_node(gate)}};
        std::optional<decltype(ranked)> result{};
        allocation_budget = failure;
        try
        {
            result.emplace(delete_virtual_pis(ranked));
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        CHECK(ranked.check_validity());
        CHECK(ranked.get_all_ranks() == expected);
        CHECK(ranked.num_pis() == 3);
        CHECK(ranked.num_virtual_pis() == 1);
        CHECK(ranked.num_pos() == 1);
        CHECK(ranked.po_at(0) == gate);
        CHECK(ranked.get_real_pi(ntk.get_node(alias)) == ntk.get_node(a));
        CHECK(mockturtle::simulate<kitty::dynamic_truth_table>(
                  ranked, mockturtle::default_simulator<kitty::dynamic_truth_table>{3}) ==
              std::vector<kitty::dynamic_truth_table>{source_function});
        if (result.has_value())
        {
            result->update_ranks();
            CHECK(result->check_validity());
            CHECK(result->num_pis() == 2);
            CHECK(result->num_virtual_pis() == 0);
            CHECK(result->rank_width(0) == 2);
            CHECK(result->rank_width(1) == 1);
            CHECK(result->depth() == 1);
            const auto tables = mockturtle::simulate<kitty::dynamic_truth_table>(
                *result, mockturtle::default_simulator<kitty::dynamic_truth_table>{2});
            REQUIRE(tables.size() == 1);
            CHECK(kitty::get_bit(tables.front(), 3));
            CHECK(kitty::count_ones(tables.front()) == 1);
            break;
        }
    }
}
