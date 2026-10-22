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
 * @brief Tests Gray-code traversal and random-access iterator contracts.
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/utils/math/gray_code_iterator.hpp>

#include <array>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

using namespace fiction;
using namespace fiction::utils::math;

TEST_CASE("Gray-code indexing follows the current position", "[gray-code-iterator]")
{
    /** Gray codes for consecutive positions from zero. */
    constexpr std::array<uint64_t, 8> codes{0, 1, 3, 2, 6, 7, 5, 4};
    /** Iterator advanced independently of its construction position. */
    gray_code_iterator it{0};
    for (const auto code : codes)
    {
        CHECK(*it == code);
        CHECK(it[0] == code);
        ++it;
    }
    it = 3;
    CHECK(it[0] == 2);
    CHECK(it[1] == 6);
    CHECK(gray_code_iterator{2} < gray_code_iterator{3});
    CHECK(gray_code_iterator{2} <= gray_code_iterator{3});
    /** A copy can be assigned independently of its initial position. */
    gray_code_iterator copy{7};
    copy = it;
    CHECK(copy == it);
    CHECK(copy[0] == *it);
    STATIC_REQUIRE(std::is_same_v<decltype(*it), const uint64_t&>);
}

TEST_CASE("Gray-code arithmetic covers 64-bit positions", "[gray-code-iterator]")
{
    /** A valid one-step distance that crosses the signed representation boundary. */
    constexpr uint64_t boundary{uint64_t{1} << 63};
    STATIC_REQUIRE(gray_code_iterator{boundary} - gray_code_iterator{boundary - 1} == 1);
    STATIC_REQUIRE(gray_code_iterator{boundary - 1} - gray_code_iterator{boundary} == -1);
    /** Offset wider than int used by standard iterator advancement. */
    constexpr int64_t offset{int64_t{1} << 32};
    /** Iterator used for forward and backward arithmetic. */
    gray_code_iterator it{3};
    std::advance(it, offset);
    CHECK(it == static_cast<uint64_t>(offset + 3));
    CHECK(it - offset == gray_code_iterator{3});
    CHECK(gray_code_iterator{3} + offset == it);
    it -= offset;
    CHECK(it == 3);
    it += -2;
    CHECK(it == 1);
    it -= -2;
    CHECK(it == 3);
    --it;
    CHECK(it == 2);
    CHECK(*it++ == 3);
    CHECK(*it-- == 2);
    CHECK(it == 2);
    it = 0;
    --it;
    CHECK(it == std::numeric_limits<uint64_t>::max());
    ++it;
    CHECK(it == 0);
}
