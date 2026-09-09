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

#include <catch2/catch_test_macros.hpp>

#if (FICTION_Z3_SOLVER)

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp>
#include <fiction/types.hpp>

#include <array>
#include <string_view>

using namespace fiction;
using namespace fiction::sidb::generators;

TEST_CASE("Circuit design deduces the gate layout type", "[on-the-fly-circuit-design]")
{
    hex_even_row_gate_clk_lyt gate_layout{{2, 2}};
    gate_layout.create_and(0, 1, {1, 2});

    on_the_fly_circuit_design_params params{};
    params.sidb_on_the_fly_gate_library_parameters.design_gate_params.number_of_canvas_sidbs = 0;

    CHECK_THROWS_AS(on_the_fly_circuit_design(gate_layout, params), unsuccessful_gate_design_error);
}

TEMPLATE_TEST_CASE("Circuit-design exceptions copy the supplied message view", "[on-the-fly-circuit-design]",
                   unsuccessful_pr_error, unsuccessful_gate_design_error)
{
    const std::array<char, 7> message{'e', 'r', 'r', 'o', 'r', '!', '\0'};
    const TestType            error{std::string_view{message.data(), 5}};
    CHECK(std::string_view{error.what()} == "error");

    const TestType empty{std::string_view{}};
    CHECK(std::string_view{empty.what()}.empty());
}

#else  // FICTION_Z3_SOLVER

TEST_CASE("Circuit-design exception messages require the Z3-dependent header", "[on-the-fly-circuit-design]")
{
    SKIP("Z3 support is disabled");
}

#endif  // FICTION_Z3_SOLVER
