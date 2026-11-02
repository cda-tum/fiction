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
 * @brief Tests for `fiction/technology/fcn/cell_ports.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/fcn/cell_ports.hpp>

#include <type_traits>
#include <unordered_set>
#include <utility>

using namespace fiction;
using namespace fiction::fcn;

TEST_CASE("Port positions", "[cell-ports]")
{
    port_position p_00{0, 0}, p_10{1, 0}, p_01{0, 1};

    CHECK(p_00 == port_position{});
    CHECK(p_10 == port_position{1, 0});
    CHECK(p_01 == port_position{0, 1});
    CHECK_FALSE(p_00 == p_01);
    CHECK_FALSE(p_00 == p_10);
    CHECK_FALSE(p_10 == p_01);
    CHECK(p_00 < p_10);
    CHECK(p_00 < p_01);
    CHECK(p_01 < p_10);

    std::unordered_set<port_position> port_set{};

    port_set.insert(p_00);
    port_set.insert(p_10);
    port_set.insert(p_01);

    CHECK(port_set.size() == 3);

    port_set.insert(p_00);
    port_set.insert(p_10);
    port_set.insert(p_01);

    CHECK(port_set.size() == 3);
}

TEST_CASE("Port directions", "[cell-ports]")
{
    port_direction p_n{port_direction::cardinal::NORTH}, p_e{port_direction::cardinal::EAST},
        p_se{port_direction::cardinal::SOUTH_EAST};

    CHECK(p_n == port_direction{});
    CHECK(p_e == port_direction{port_direction::cardinal::EAST});
    CHECK(p_se == port_direction{port_direction::cardinal::SOUTH_EAST});
    CHECK_FALSE(p_n == p_e);
    CHECK_FALSE(p_n == p_se);
    CHECK_FALSE(p_se == p_e);
    CHECK(p_n < p_se);
    CHECK(p_n < p_e);
    CHECK(p_e < p_se);

    std::unordered_set<port_direction> port_set{};

    port_set.insert(p_n);
    port_set.insert(p_se);
    port_set.insert(p_e);

    CHECK(port_set.size() == 3);

    port_set.insert(p_n);
    port_set.insert(p_se);
    port_set.insert(p_e);

    CHECK(port_set.size() == 3);
}

/** @brief Port lists distinguish input ports from output ports for both port representations. */
TEMPLATE_TEST_CASE("Port lists", "[cell-ports]", port_position, port_direction)
{
    /** @brief Distinct ports for the selected representation. */
    const auto [first, second] = []
    {
        if constexpr (std::is_same_v<TestType, port_position>)
        {
            return std::pair{port_position{0, 2}, port_position{2, 4}};
        }
        else
        {
            return std::pair{port_direction{port_direction::cardinal::SOUTH_WEST},
                             port_direction{port_direction::cardinal::WEST}};
        }
    }();
    /** @brief One input and one output. */
    const port_list<TestType> ports{{first}, {second}};
    /** @brief Reversed input and output roles. */
    const port_list<TestType> reversed{{second}, {first}};

    CHECK_FALSE(ports == reversed);
    CHECK_FALSE(port_list<TestType>{} == reversed);
    CHECK_FALSE(ports == port_list<TestType>{});
    CHECK(ports == port_list<TestType>{{first}, {second}});
    CHECK(reversed == port_list<TestType>{{second}, {first}});
}
