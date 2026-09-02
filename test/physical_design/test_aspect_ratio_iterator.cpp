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

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/coordinates.hpp>
#include <fiction/physical_design/aspect_ratio_iterator.hpp>

#include <iterator>
#include <type_traits>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;

TEST_CASE("Aspect Ratio Iterator Traits", "[bdl-input-iterator]")
{
    CHECK(std::is_same_v<std::iterator_traits<aspect_ratio_iterator<coords::offset>>::iterator_category,
                         std::forward_iterator_tag>);

    CHECK(std::is_same_v<std::iterator_traits<aspect_ratio_iterator<coords::offset>>::value_type, coords::offset>);
}

TEST_CASE("Aspect ratio iteration", "[aspect-ratio-iterator]")
{
    aspect_ratio_iterator<coords::offset> ari{1};

    for (auto i = 0; ari <= 4; ++ari, ++i)
    {
        switch (i)
        {
            case 0:
            {
                CHECK(*ari == coords::offset{0, 0});
                CHECK(ari == 1u);
                CHECK(ari == aspect_ratio_iterator<coords::offset>{1});  // equal since both point to the first element
                break;
            }
            case 1:
            {
                CHECK(*ari == coords::offset{0, 1});
                CHECK(ari == 2u);
                CHECK(ari == aspect_ratio_iterator<coords::offset>{2});  // equal since both point to the first element
                break;
            }
            case 2:
            {
                CHECK(*ari == coords::offset{1, 0});
                CHECK(ari == 2u);
                CHECK(ari !=
                      aspect_ratio_iterator<coords::offset>{2});  // not equal since ari points to the second element
                break;
            }
            case 3:
            {
                CHECK(*ari == coords::offset{0, 2});
                CHECK(ari == 3u);
                CHECK(ari == aspect_ratio_iterator<coords::offset>{3});  // equal since both point to the first element
                break;
            }
            case 4:
            {
                CHECK(*ari == coords::offset{2, 0});
                CHECK(ari == 3u);
                CHECK(ari !=
                      aspect_ratio_iterator<coords::offset>{3});  // not equal since ari points to the second element
                break;
            }
            case 5:
            {
                CHECK(*ari == coords::offset{0, 3});
                CHECK(ari == 4u);
                CHECK(ari == aspect_ratio_iterator<coords::offset>{4});  // equal since both point to the first element
                break;
            }
            case 6:
            {
                CHECK(*ari == coords::offset{3, 0});
                CHECK(ari == 4u);
                CHECK(ari !=
                      aspect_ratio_iterator<coords::offset>{4});  // not equal since ari points to the second element
                break;
            }
            case 7:
            {
                CHECK(*ari == coords::offset{1, 1});
                CHECK(ari == 4u);
                CHECK(ari !=
                      aspect_ratio_iterator<coords::offset>{4});  // not equal since ari points to the third element
                break;
            }
            default:
            {
                CHECK(false);
            }
        }
    }
}
