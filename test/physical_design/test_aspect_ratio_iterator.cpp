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
 * @brief Tests for `fiction/physical_design/aspect_ratio_iterator.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/aspect_ratio_iterator.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;

TEST_CASE("Aspect Ratio Iterator Traits", "[aspect-ratio-iterator]")
{
    STATIC_REQUIRE(std::is_same_v<std::iterator_traits<aspect_ratio_iterator<layout_base::extent>>::iterator_category,
                                  std::forward_iterator_tag>);

    STATIC_REQUIRE(std::is_same_v<std::iterator_traits<aspect_ratio_iterator<layout_base::extent>>::value_type,
                                  layout_base::extent>);
}

TEST_CASE("Aspect ratio iteration", "[aspect-ratio-iterator]")
{
    /** @brief Documented factor order through four faces. */
    constexpr std::array<layout_base::extent, 8> expected{
        {{1, 1}, {1, 2}, {2, 1}, {1, 3}, {3, 1}, {1, 4}, {4, 1}, {2, 2}}};
    /** @brief Face count for each expected iterator position. */
    constexpr std::array<uint64_t, 8> faces{1, 2, 2, 3, 3, 4, 4, 4};
    /** @brief Iterator whose order and termination are checked against the expected sequence. */
    aspect_ratio_iterator<layout_base::extent> ari{1};
    for (std::size_t i{}; i < expected.size(); ++i, ++ari)
    {
        CAPTURE(i);
        CHECK(ari <= 4u);
        CHECK(*ari == expected[i]);
        CHECK(ari == faces[i]);
        CHECK((ari == aspect_ratio_iterator<layout_base::extent>{faces[i]}) == (i == 0 || faces[i] != faces[i - 1]));
    }
    CHECK_FALSE(ari <= 4u);
}

TEST_CASE("Aspect ratio iterator copies own their position", "[aspect-ratio-iterator]")
{
    aspect_ratio_iterator<layout_base::extent> current{2};
    auto                                       copy = current;
    ++current;
    ++current;
    CHECK(*copy == layout_base::extent{1, 2});
    CHECK(*(copy++) == layout_base::extent{1, 2});
    CHECK(*copy == layout_base::extent{2, 1});
    CHECK(copy < current);
    CHECK_FALSE(current <= copy);
}
