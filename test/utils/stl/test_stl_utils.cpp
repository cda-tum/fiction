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
 * @brief Tests for `fiction/utils/stl/stl_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/stl/stl_utils.hpp>

#include <array>
#include <iterator>
#include <vector>

using namespace fiction;
using namespace fiction::physical_design;
using namespace fiction::utils::stl;

TEST_CASE("Test find_first_two_of with vector input", "[find_first_two_of]")
{
    static const std::vector<int> v1{0, 1, 1, 2, 3, 3};
    static const std::vector<int> v2{1, 2, 3, 3};

    auto it = find_first_two_of(v1.begin(), v1.end(), v2.begin(), v2.end());
    CHECK(*it == 1);
    CHECK(*(it + 1) == 2);

    static const std::vector<int> v3{1, 2, 4, 3};
    it = find_first_two_of(v1.begin(), v1.end(), v3.begin(), v3.end());
    CHECK(it == std::next(v1.begin(), 2));
}

TEST_CASE("Test find_first_two_of with array input", "[find_first_two_of]")
{
    static constexpr const std::array a1{0, 1, 1, 2, 3, 3};
    static constexpr const std::array a2{1, 2, 3, 3};

    /** @brief First matching adjacent pair in the array. */
    // NOLINTNEXTLINE(readability-qualified-auto): MSVC Debug uses checked array iterators instead of pointers.
    auto it = find_first_two_of(std::begin(a1), std::end(a1), std::begin(a2), std::end(a2));
    CHECK(*it == 1);
    CHECK(*(std::next(it, 1)) == 2);

    static constexpr const std::array a3{1, 2, 4, 3};
    it = find_first_two_of(std::begin(a1), std::end(a1), std::begin(a3), std::end(a3));
    CHECK(it == std::next(std::begin(a1), 2));
}

TEST_CASE("Test find_first_two_of with different iterator types", "[find_first_two_of]")
{
    static const std::vector<int>     v1{0, 1, 1, 2, 3, 3};
    static constexpr const std::array a2{1, 2, 3, 3};

    auto it = find_first_two_of(v1.begin(), v1.end(), std::begin(a2), std::end(a2));
    CHECK(*it == 1);
    CHECK(*(it + 1) == 2);

    static constexpr const std::array a3{1, 2, 4, 3};
    it = find_first_two_of(v1.begin(), v1.end(), std::begin(a3), std::end(a3));
    CHECK(it == std::next(v1.begin(), 2));
}

TEST_CASE("Test find_first_two_of with layout_coordinate_paths", "[first_first_two_of]")
{
    static const layout_coordinate_path<cart_gate_clk_lyt> p1{{1, 1}, {2, 1}};
    static const layout_coordinate_path<cart_gate_clk_lyt> p2{{0, 1}, {1, 1}, {2, 1}};

    // regular iterators
    const auto it1 = find_first_two_of(p1.begin(), p1.end(), p2.begin(), p2.end());
    const auto it2 = find_first_two_of(p2.begin(), p2.end(), p1.begin(), p1.end());

    CHECK(it1 == p1.begin());
    CHECK(it2 == std::next(p2.begin(), 1));

    // const iterators via std::cbegin and std::cend
    const auto it3 = find_first_two_of(std::cbegin(p1), std::cend(p1), std::cbegin(p2), std::cend(p2));
    const auto it4 = find_first_two_of(std::cbegin(p2), std::cend(p2), std::cbegin(p1), std::cend(p1));

    CHECK(it3 == p1.begin());
    CHECK(it4 == std::next(p2.begin(), 1));
}

/** @brief Ranges without a pair have no shared two-element subsequence. */
TEST_CASE("find_first_two_of rejects ranges shorter than a pair", "[find_first_two_of]")
{
    /** @brief Candidate two-element subsequence. */
    const std::vector<int> pair{1, 2};
    for (const auto& short_range : {std::vector<int>{}, std::vector<int>{1}})
    {
        CHECK(find_first_two_of(short_range.cbegin(), short_range.cend(), pair.cbegin(), pair.cend()) ==
              short_range.cend());
        CHECK(find_first_two_of(pair.cbegin(), pair.cend(), short_range.cbegin(), short_range.cend()) == pair.cend());
        CHECK(find_first_two_of(short_range.cbegin(), short_range.cend(), short_range.cbegin(), short_range.cend()) ==
              short_range.cend());
    }
}
