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

#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/aspect_ratio_iterator.hpp>

#include <iterator>
#include <type_traits>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;

TEST_CASE("Aspect Ratio Iterator Traits", "[bdl-input-iterator]")
{
    CHECK(std::is_same_v<std::iterator_traits<aspect_ratio_iterator<layout_base::extent>>::iterator_category,
                         std::forward_iterator_tag>);

    CHECK(std::is_same_v<std::iterator_traits<aspect_ratio_iterator<layout_base::extent>>::value_type,
                         layout_base::extent>);
}

TEST_CASE("Aspect ratio iteration", "[aspect-ratio-iterator]")
{
    aspect_ratio_iterator<layout_base::extent> ari{1};

    for (auto i = 0; ari <= 4; ++ari, ++i)
    {
        switch (i)
        {
            case 0:
            {
                CHECK(*ari == layout_base::extent{1, 1});
                CHECK(ari == 1u);
                CHECK(ari ==
                      aspect_ratio_iterator<layout_base::extent>{1});  // equal since both point to the first element
                break;
            }
            case 1:
            {
                CHECK(*ari == layout_base::extent{1, 2});
                CHECK(ari == 2u);
                CHECK(ari ==
                      aspect_ratio_iterator<layout_base::extent>{2});  // equal since both point to the first element
                break;
            }
            case 2:
            {
                CHECK(*ari == layout_base::extent{2, 1});
                CHECK(ari == 2u);
                CHECK(ari != aspect_ratio_iterator<layout_base::extent>{
                                 2});  // not equal since ari points to the second element
                break;
            }
            case 3:
            {
                CHECK(*ari == layout_base::extent{1, 3});
                CHECK(ari == 3u);
                CHECK(ari ==
                      aspect_ratio_iterator<layout_base::extent>{3});  // equal since both point to the first element
                break;
            }
            case 4:
            {
                CHECK(*ari == layout_base::extent{3, 1});
                CHECK(ari == 3u);
                CHECK(ari != aspect_ratio_iterator<layout_base::extent>{
                                 3});  // not equal since ari points to the second element
                break;
            }
            case 5:
            {
                CHECK(*ari == layout_base::extent{1, 4});
                CHECK(ari == 4u);
                CHECK(ari ==
                      aspect_ratio_iterator<layout_base::extent>{4});  // equal since both point to the first element
                break;
            }
            case 6:
            {
                CHECK(*ari == layout_base::extent{4, 1});
                CHECK(ari == 4u);
                CHECK(ari != aspect_ratio_iterator<layout_base::extent>{
                                 4});  // not equal since ari points to the second element
                break;
            }
            case 7:
            {
                CHECK(*ari == layout_base::extent{2, 2});
                CHECK(ari == 4u);
                CHECK(ari != aspect_ratio_iterator<layout_base::extent>{
                                 4});  // not equal since ari points to the third element
                break;
            }
            default:
            {
                CHECK(false);
            }
        }
    }
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
