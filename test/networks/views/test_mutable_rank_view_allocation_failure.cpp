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

#include "utils/allocation_failure.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>

#include <cstddef>
#include <new>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::test;

TEST_CASE("Rank insertion propagates allocation failure and permits recomputation", "[mutable-rank-view]")
{
    require_allocation_failure_support();
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
