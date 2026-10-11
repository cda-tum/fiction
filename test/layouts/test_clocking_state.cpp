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
 * @brief Allocation-failure contracts for clocking and obstruction state.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/clocking_state.hpp>
#include <fiction/layouts/obstructions.hpp>
#include <fiction/layouts/tile_clocking.hpp>
#include <fiction/technology/qca/layout.hpp>

#include <cstddef>
#include <new>
#include <utility>

using namespace fiction;
using namespace fiction::test;
using namespace fiction::layouts;

TEST_CASE("Clocking state moves without allocating", "[clocking-state]")
{
    require_allocation_failure_support();
    /** State with owned clock overrides and synchronization delays. */
    clocking::state source{clocking::twoddwave()};
    source.assign_clock_number({1, 0}, 3);
    source.assign_synchronization_element({1, 0}, 2);
    /** Destination whose previous state is replaced by the move. */
    clocking::state destination{clocking::open()};
    allocation_budget = 0;
    /** Move construction must not allocate while transferring synchronization storage. */
    auto moved  = std::move(source);
    destination = std::move(moved);
    allocation_budget.reset();
    CHECK(destination.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    CHECK(destination.get_clock_number({1, 0}) == 3);
    CHECK(destination.get_synchronization_element({1, 0}) == 2);
}

namespace
{
/**
 * @brief Exercises each allocating update failure before checking a successful update.
 * @tparam State Mutable capability value.
 * @tparam Update Mutation callable.
 * @tparam Check Callable checking the unchanged value.
 * @param original Value before mutation.
 * @param update Mutation under allocation control.
 * @param unchanged Check performed after an allocation failure.
 * @return Successfully updated value.
 */
template <typename State, typename Update, typename Check>
State check_allocation_failures(const State& original, const Update& update, const Check& unchanged)
{
    require_allocation_failure_support();
    bool failed{};
    for (std::size_t failure = 0;; ++failure)
    {
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        auto candidate = original;
        try
        {
            allocation_budget = failure;
            update(candidate);
            allocation_budget.reset();
            CHECK(failed);
            return candidate;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
            failed = true;
            unchanged(candidate);
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
    }
}
}  // namespace

TEST_CASE("Clocking updates preserve state on allocation failure", "[clocking-allocation]")
{
    clocking::state original{clocking::open()};
    original.assign_synchronization_element({1, 1}, 2);

    SECTION("Clock scheme replacement")
    {
        auto replacement =
            clocking::scheme{"Replacement with an allocated scheme name", {{0, 1, 2}, {1, 2, 3}}, 4, 3, 3};
        replacement.override_clock_number(7, 8, 2);
        const auto result = check_allocation_failures(
            original, [&](auto& candidate) { candidate.replace_clocking_scheme(replacement); },
            [&](const auto& candidate)
            {
                CHECK(candidate.get_clocking_scheme() == original.get_clocking_scheme());
                CHECK(candidate.get_synchronization_element({1, 1}) == 2);
            });
        CHECK(result.get_clocking_scheme() == replacement);
        CHECK(result.get_synchronization_element({1, 1}) == 2);
    }
    SECTION("Clock number override")
    {
        const auto result = check_allocation_failures(
            original, [](auto& candidate) { candidate.assign_clock_number({2, 2}, 3); },
            [&](const auto& candidate) { CHECK(candidate.get_clocking_scheme() == original.get_clocking_scheme()); });
        CHECK(result.get_clock_number({2, 2}) == 3);
    }
    SECTION("Synchronization insertion")
    {
        const auto result = check_allocation_failures(
            original, [](auto& candidate) { candidate.assign_synchronization_element({2, 2}, 3); },
            [](const auto& candidate)
            {
                CHECK(candidate.num_se() == 1);
                CHECK(candidate.get_synchronization_element({1, 1}) == 2);
                CHECK(candidate.get_synchronization_element({2, 2}) == 0);
            });
        CHECK(result.num_se() == 2);
        CHECK(result.get_synchronization_element({2, 2}) == 3);
    }
}

TEST_CASE("Obstruction insertion propagates allocation failure", "[clocking-allocation]")
{
    SECTION("Coordinate")
    {
        const auto result = check_allocation_failures(
            obstructions{}, [](auto& candidate) { candidate.obstruct_coordinate({2, 2}); },
            [](const auto& candidate) { CHECK_FALSE(candidate.is_obstructed_coordinate({2, 2})); });
        CHECK(result.is_obstructed_coordinate({2, 2}));
    }
    SECTION("Directed connection")
    {
        const auto result = check_allocation_failures(
            obstructions{}, [](auto& candidate) { candidate.obstruct_connection({2, 2}, {3, 2}); },
            [](const auto& candidate) { CHECK_FALSE(candidate.is_obstructed_connection({2, 2}, {3, 2})); });
        CHECK(result.is_obstructed_connection({2, 2}, {3, 2}));
        CHECK_FALSE(result.is_obstructed_connection({3, 2}, {2, 2}));
    }
}

TEST_CASE("Clocking copy assignment preserves values on allocation failure", "[clocking-assignment]")
{
    auto source_scheme = clocking::scheme{"Allocated replacement scheme name", {{0, 1, 2}, {1, 2, 3}}, 4, 3, 3};
    source_scheme.override_clock_number(7, 8, 2);
    SECTION("Scheme copy assignment")
    {
        const auto original = clocking::open();
        const auto result   = check_allocation_failures(
            original, [&](auto& candidate) { candidate = source_scheme; },
            [&](const auto& candidate) { CHECK(candidate == original); });
        CHECK(result == source_scheme);
        CHECK(result(7, 8) == 2);
    }
    SECTION("State copy assignment")
    {
        clocking::state original{clocking::open()};
        original.assign_synchronization_element({1, 1}, 2);
        clocking::state source{source_scheme};
        source.assign_synchronization_element({7, 8}, 3);
        const auto result = check_allocation_failures(
            original, [&](auto& candidate) { candidate = source; },
            [&](const auto& candidate)
            {
                CHECK(candidate.get_clocking_scheme() == original.get_clocking_scheme());
                CHECK(candidate.num_se() == 1);
                CHECK(candidate.get_synchronization_element({1, 1}) == 2);
                CHECK(candidate.get_synchronization_element({7, 8}) == 0);
            });
        CHECK(result.get_clocking_scheme() == source_scheme);
        CHECK(result.num_se() == 1);
        CHECK(result.get_synchronization_element({7, 8}) == 3);
    }
}

TEST_CASE("Cell clocking wrappers propagate allocation failure", "[clocking-allocation]")
{
    const tile_clocking original{};
    SECTION("Scheme replacement")
    {
        const auto replacement = clocking::twoddwave();
        const auto result      = check_allocation_failures(
            original, [&](auto& candidate) { candidate.replace_clocking_scheme(replacement); },
            [&](const auto& candidate) { CHECK(candidate.get_clocking_scheme() == original.get_clocking_scheme()); });
        CHECK(result.get_clocking_scheme() == replacement);
    }
    SECTION("Clock number override")
    {
        const auto result = check_allocation_failures(
            original, [](auto& candidate) { candidate.assign_clock_number({2, 2}, 3); },
            [&](const auto& candidate) { CHECK(candidate.get_clocking_scheme() == original.get_clocking_scheme()); });
        CHECK(result.get_clock_number({2, 2}) == 3);
    }
    SECTION("Scheme copy")
    {
        tile_clocking patterned{};
        patterned.replace_clocking_scheme(clocking::twoddwave());
        const auto result = check_allocation_failures(
            patterned, [](const auto& candidate) { static_cast<void>(candidate.get_clocking_scheme()); },
            [&](const auto& candidate) { CHECK(candidate.get_clocking_scheme() == patterned.get_clocking_scheme()); });
        CHECK(result.get_clocking_scheme() == patterned.get_clocking_scheme());
    }
    SECTION("QCA synchronization insertion")
    {
        const auto result = check_allocation_failures(
            qca::layout{}, [](auto& candidate) { candidate.assign_synchronization_element({2, 2}, 3); },
            [](const auto& candidate)
            {
                CHECK(candidate.num_se() == 0);
                CHECK(candidate.get_synchronization_element({2, 2}) == 0);
            });
        CHECK(result.num_se() == 1);
        CHECK(result.get_synchronization_element({2, 2}) == 3);
    }
}
