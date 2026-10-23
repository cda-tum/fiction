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
 * @brief Tests for `fiction/technology/sidb/simulation/logic/verify_logic_match.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/verify_logic_match.hpp>

#include <kitty/dynamic_truth_table.hpp>

#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

TEST_CASE("Bestagon FO2 gate", "[verify-logic-match]")
{
    const auto lyt = blueprints::bestagon_fo2();

    const auto input_wires  = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);

    auto bii = bdl_input_iterator{lyt};

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.32}};

    // The fan-out has one Boolean input, encoded by patterns 0 and 1.
    SECTION("Input zero")
    {
        bii = 0;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{},
                                 std::vector<kitty::dynamic_truth_table>{create_fan_out_tt()}, 0, input_wires,
                                 output_wires) == operational_status::OPERATIONAL);
    }

    SECTION("Input one")
    {
        bii = 1;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        SECTION("Correct index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{},
                                     std::vector<kitty::dynamic_truth_table>{create_fan_out_tt()}, 1, input_wires,
                                     output_wires) == operational_status::OPERATIONAL);
        }
        SECTION("Wrong input index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{},
                                     std::vector<kitty::dynamic_truth_table>{create_fan_out_tt()}, 0, input_wires,
                                     output_wires) == operational_status::NON_OPERATIONAL);
        }
    }
}

TEST_CASE("AND gate mirrored on the x-axis on the H-Si 111 surface", "[verify-logic-match]")
{
    const auto lyt = blueprints::and_gate_111_mirrored_on_the_x_axis();

    const auto input_wires  = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);

    REQUIRE(input_wires.size() == 2);
    REQUIRE(output_wires.size() == 1);

    auto bii = bdl_input_iterator{lyt};

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.32}};

    SECTION("Left input one and right input zero")
    {
        bii = 2;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{},
                                 std::vector<kitty::dynamic_truth_table>{create_and_tt()}, 2, input_wires,
                                 output_wires) == operational_status::OPERATIONAL);
    }

    SECTION("Left input zero and right input one")
    {
        bii = 1;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        is_operational_params op_params{};
        op_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

        SECTION("Correct index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), op_params,
                                     std::vector<kitty::dynamic_truth_table>{create_and_tt()}, 1, input_wires,
                                     output_wires) == operational_status::OPERATIONAL);
        }
        SECTION("Wrong input index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), op_params,
                                     std::vector<kitty::dynamic_truth_table>{create_and_tt()}, 2, input_wires,
                                     output_wires) == operational_status::NON_OPERATIONAL);
        }
    }
}

TEST_CASE("Logic matching propagates invalid lattice basis errors", "[verify-logic-match]")
{
    layout       lyt{};
    lattice_site invalid{};
    invalid.z = 2;
    lyt.assign_sidb(invalid, dot_tag::NORMAL);
    is_operational_params params{};
    params.sim_params.base = 2;

    CHECK_THROWS_AS(verify_logic_match(lyt, charge_distribution{lyt}, params, std::vector{create_id_tt()}, 0, {}, {}),
                    std::out_of_range);
}

TEST_CASE("Logic matching rejects invalid specifications", "[verify-logic-match][invalid-spec]")
{
    /** @brief AND layout whose charge distribution is checked. */
    const auto lyt = blueprints::siqad_and_gate();
    /** @brief Charge distribution supplied to the logic check. */
    const charge_distribution cd{lyt};
    CHECK_THROWS_AS(verify_logic_match(lyt, cd, {}, {}, 0, {}, {}), std::invalid_argument);
    CHECK_THROWS_AS(verify_logic_match(lyt, cd, {}, {create_and_tt(), create_id_tt()}, 0, {}, {}),
                    std::invalid_argument);
}

TEST_CASE("Logic matching rejects an input pattern outside the specification", "[verify-logic-match][invalid-pattern]")
{
    /** @brief AND layout whose charge distribution is checked. */
    const auto lyt = blueprints::siqad_and_gate();
    CHECK_THROWS_AS(verify_logic_match(lyt, charge_distribution{lyt}, {}, {create_and_tt()}, 4, {}, {}),
                    std::out_of_range);
}
