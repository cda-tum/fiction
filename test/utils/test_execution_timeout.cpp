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
 * @brief Tests shared execution deadlines and overflow-safe millisecond budgets.
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/utils/execution_timeout.hpp>

#include <chrono>
#include <cstdint>
#include <limits>

using namespace fiction;
using namespace fiction::utils;

TEST_CASE("Execution deadlines preserve enclosing budgets", "[execution-timeout]")
{
    const auto unlimited = std::numeric_limits<uint64_t>::max();
    const auto distant   = std::chrono::steady_clock::time_point::max();
    CHECK(make_deadline(unlimited) == distant);
    CHECK(make_deadline(unlimited - 1) == distant);
    CHECK_NOTHROW(check_deadline(distant));

    const auto expired = std::chrono::steady_clock::now();
    CHECK(make_deadline(unlimited, expired) == expired);
    CHECK(make_deadline(unlimited - 1, expired) == expired);
    CHECK(make_deadline(1000, expired) == expired);
    CHECK_THROWS_AS(check_deadline(expired), timeout_error);
    CHECK_THROWS_AS(check_deadline(make_deadline(0)), timeout_error);

    const auto start    = std::chrono::steady_clock::now();
    const auto deadline = make_deadline(1000);
    CHECK(deadline >= start + std::chrono::milliseconds{1000});
    CHECK(deadline <= std::chrono::steady_clock::now() + std::chrono::milliseconds{1000});
}
