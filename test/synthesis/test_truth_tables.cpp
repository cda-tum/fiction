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
 * @brief Tests for Boolean comparison truth tables.
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/synthesis/truth_tables.hpp>

#include <kitty/bit_operations.hpp>

#include <cstdint>

using namespace fiction;
using namespace fiction::synthesis;

TEST_CASE("Comparison truth tables include equality", "[truth-tables]")
{
    const auto less_equal    = create_le_tt();
    const auto greater_equal = create_ge_tt();

    for (uint64_t input = 0; input < 4; ++input)
    {
        const auto lhs = input >> 1;
        const auto rhs = input & 1;

        CHECK(static_cast<bool>(kitty::get_bit(less_equal, input)) == (lhs <= rhs));
        CHECK(static_cast<bool>(kitty::get_bit(greater_equal, input)) == (lhs >= rhs));
    }
}
