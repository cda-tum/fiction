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
 * @brief Tests for bounded SiDB circuit-design exception messages.
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp>

#include <array>
#include <string_view>

using namespace fiction;
using namespace fiction::sidb::generators;

TEMPLATE_TEST_CASE("Circuit-design exceptions copy the supplied message view", "[on-the-fly-circuit-design]",
                   unsuccessful_pr_error, unsuccessful_gate_design_error)
{
    const std::array<char, 7> message{'e', 'r', 'r', 'o', 'r', '!', '\0'};
    const TestType            error{std::string_view{message.data(), 5}};
    CHECK(std::string_view{error.what()} == "error");

    const TestType empty{std::string_view{}};
    CHECK(std::string_view{empty.what()}.empty());
}
