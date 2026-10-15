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
 * @brief Tests for `fiction/physical_design/path_finding/cost.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/path_finding/cost.hpp>

#include <utility>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design::path_finding;

TEST_CASE("Unit cost", "[cost]")
{
    const auto [source, target] =
        GENERATE(std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}},
                 std::pair{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}},
                 std::pair{layout_base::coordinate{1, 2}, layout_base::coordinate{3, 3}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{4, 4}},
                 std::pair{layout_base::coordinate{4, 4}, layout_base::coordinate{0, 0}},
                 std::pair{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 0}},
                 std::pair{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 1}},
                 std::pair{layout_base::coordinate{9, 1}, layout_base::coordinate{6, 2}},
                 std::pair{layout_base::coordinate{6, 2}, layout_base::coordinate{0, 4}},
                 std::pair{layout_base::coordinate{0, 4}, layout_base::coordinate{9, 1}},
                 std::pair{layout_base::coordinate{6, 2, 1}, layout_base::coordinate{0, 4, 0}},
                 std::pair{layout_base::coordinate{6, 2, 0}, layout_base::coordinate{0, 4, 1}},
                 std::pair{layout_base::coordinate{0, 4, 1}, layout_base::coordinate{9, 1, 1}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{-1, -1}},
                 std::pair{layout_base::coordinate{-4, -3}, layout_base::coordinate{1, -1}},
                 std::pair{layout_base::coordinate{-2, -8}, layout_base::coordinate{-6, -4}});
    SECTION("Direct function")
    {
        const auto value = unit_cost<>(source, target);
        CHECK(value == 1);
    }
    SECTION("Functor")
    {
        const unit_cost_functor<> cost{};
        const auto                value = cost(source, target);
        CHECK(value == 1);
    }
}

TEST_CASE("Random cost", "[cost]")
{
    const auto [source, target] =
        GENERATE(std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}},
                 std::pair{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}},
                 std::pair{layout_base::coordinate{1, 2}, layout_base::coordinate{3, 3}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{4, 4}},
                 std::pair{layout_base::coordinate{4, 4}, layout_base::coordinate{0, 0}},
                 std::pair{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 0}},
                 std::pair{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 1}},
                 std::pair{layout_base::coordinate{9, 1}, layout_base::coordinate{6, 2}},
                 std::pair{layout_base::coordinate{6, 2}, layout_base::coordinate{0, 4}},
                 std::pair{layout_base::coordinate{0, 4}, layout_base::coordinate{9, 1}},
                 std::pair{layout_base::coordinate{6, 2, 1}, layout_base::coordinate{0, 4, 0}},
                 std::pair{layout_base::coordinate{6, 2, 0}, layout_base::coordinate{0, 4, 1}},
                 std::pair{layout_base::coordinate{0, 4, 1}, layout_base::coordinate{9, 1, 1}},
                 std::pair{layout_base::coordinate{0, 0}, layout_base::coordinate{-1, -1}},
                 std::pair{layout_base::coordinate{-4, -3}, layout_base::coordinate{1, -1}},
                 std::pair{layout_base::coordinate{-2, -8}, layout_base::coordinate{-6, -4}});
    SECTION("Direct function")
    {
        const auto value = random_cost<>(source, target);
        CHECK((value >= 0 && value <= 1));
    }
    SECTION("Functor")
    {
        const random_cost_functor<> cost{};
        const auto                  value = cost(source, target);
        CHECK((value >= 0 && value <= 1));
    }
}
