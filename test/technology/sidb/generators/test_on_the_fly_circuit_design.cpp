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
 * @brief Tests for SiDB circuit design and bounded exception messages.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/execution_timeout.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <limits>
#include <string_view>

#if (FICTION_Z3_SOLVER)
#include <mockturtle/networks/aig.hpp>
#endif

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::sidb;
using namespace fiction::sidb::generators;

TEST_CASE("Circuit design deduces the gate layout type", "[on-the-fly-circuit-design]")
{
    hex_even_row_gate_clk_lyt gate_layout{{2, 2}};
    gate_layout.create_and(0, 1, {1, 2});

    on_the_fly_circuit_design_params params{};
    params.sidb_on_the_fly_gate_library_parameters.design_gate_params.number_of_canvas_sidbs = 0;

    CHECK_THROWS_AS(on_the_fly_circuit_design(gate_layout, params), unsuccessful_gate_design_error);
}

TEST_CASE("Circuit design honors both circuit and gate timeouts", "[on-the-fly-circuit-design]")
{
    hex_even_row_gate_clk_lyt gate_layout{{2, 2}, clocking::row<hex_even_row_gate_clk_lyt>()};
    const auto                first  = gate_layout.create_pi("a", {0, 0});
    const auto                second = gate_layout.create_pi("b", {1, 0});
    const auto                gate   = gate_layout.create_and(first, second, {1, 1});
    gate_layout.create_po(gate, "f", {0, 2});

    on_the_fly_circuit_design_params params{};
    auto&                            gates = params.sidb_on_the_fly_gate_library_parameters.design_gate_params;
    CHECK(params.timeout == std::numeric_limits<uint64_t>::max());
    CHECK(gates.timeout == std::numeric_limits<uint64_t>::max());

    SECTION("Zero circuit budget expires with unlimited gate budgets")
    {
        params.timeout = 0;
    }
    SECTION("Zero gate budget expires within a positive circuit budget")
    {
        params.timeout = 10'000;
        gates.timeout  = 0;
    }
    SECTION("Unlimited gate budgets cannot override a finite circuit budget")
    {
        params.timeout               = 1;
        gates.canvas                 = {site_at_row(0, 0), site_at_row(1'000, 1'000)};
        gates.number_of_canvas_sidbs = 0;
    }

    const auto start = std::chrono::steady_clock::now();
    CHECK_THROWS_AS(on_the_fly_circuit_design(gate_layout, params), utils::timeout_error);
    CHECK(std::chrono::steady_clock::now() - start < std::chrono::seconds{10});
    CHECK(gate_layout.num_pis() == 2);
    CHECK(gate_layout.num_pos() == 1);
    CHECK(gate_layout.is_and(gate_layout.get_node({1, 1})));
}

#if (FICTION_Z3_SOLVER)
TEST_CASE("Defect-aware circuit design propagates gate timeouts", "[on-the-fly-circuit-design]")
{
    mockturtle::aig_network network{};
    const auto              first  = network.create_pi();
    const auto              second = network.create_pi();
    network.create_po(network.create_and(first, second));

    const hex_even_row_gate_clk_lyt tiling{{2, 2}, clocking::row<hex_even_row_gate_clk_lyt>()};
    const sidb::layout              surface{};

    on_the_fly_circuit_design_on_defective_surface_params params{};
    params.exact_design_parameters.scheme                                     = "Row";
    params.exact_design_parameters.upper_bound_x                              = 3;
    params.exact_design_parameters.upper_bound_y                              = 3;
    params.exact_design_parameters.fixed_size                                 = true;
    params.exact_design_parameters.timeout                                    = 10'000;
    params.sidb_on_the_fly_gate_library_parameters.design_gate_params.timeout = 0;

    CHECK_THROWS_AS(on_the_fly_circuit_design_on_defective_surface(network, tiling, surface, params),
                    utils::timeout_error);
}
#endif

TEMPLATE_TEST_CASE("Circuit-design exceptions copy the supplied message view", "[on-the-fly-circuit-design]",
                   unsuccessful_pr_error, unsuccessful_gate_design_error)
{
    const std::array<char, 7> message{'e', 'r', 'r', 'o', 'r', '!', '\0'};
    const TestType            error{std::string_view{message.data(), 5}};
    CHECK(std::string_view{error.what()} == "error");

    const TestType empty{std::string_view{}};
    CHECK(std::string_view{empty.what()}.empty());
}
