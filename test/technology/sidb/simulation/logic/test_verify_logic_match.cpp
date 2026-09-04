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
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/verify_logic_match.hpp>
#include <fiction/types.hpp>

#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

TEST_CASE("Bestagon FO2 gate", "[does-charge-distribution-match-logic-for-given-input-pattern]")
{
    const auto lyt = blueprints::bestagon_fo2();

    const auto input_wires  = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);

    auto bii = bdl_input_iterator{lyt};

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.32}};

    SECTION("The index is 2, which means that the left input is set to one and the right input is set to zero.")
    {
        bii = 2;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{}, std::vector<tt>{create_fan_out_tt()}, 2,
                                 input_wires, output_wires) == operational_status::OPERATIONAL);
    }

    SECTION("Index is 1, which means that the left input is set to zero and the right input is set to one.")
    {
        bii = 1;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        SECTION("Correct index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{}, std::vector<tt>{create_fan_out_tt()}, 1,
                                     input_wires, output_wires) == operational_status::OPERATIONAL);
        }
        SECTION("Wrong input index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{}, std::vector<tt>{create_fan_out_tt()}, 2,
                                     input_wires, output_wires) == operational_status::NON_OPERATIONAL);
        }
    }
}

TEST_CASE("AND gate mirrored on the x-axis on the H-Si 111 surface",
          "[does-charge-distribution-match-logic-for-given-input-pattern]")
{
    const auto lyt = blueprints::and_gate_111_mirrored_on_the_x_axis();

    const auto input_wires  = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);

    REQUIRE(input_wires.size() == 2);
    REQUIRE(output_wires.size() == 1);

    auto bii = bdl_input_iterator{lyt};

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.32}};

    SECTION("The index is 2, which means that the left input is set to one and the right input is set to zero.")
    {
        bii = 2;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        CHECK(verify_logic_match(*bii, gs.front(), is_operational_params{}, std::vector<tt>{create_and_tt()}, 2,
                                 input_wires, output_wires) == operational_status::OPERATIONAL);
    }

    SECTION("Index is 1, which means that the left input is set to zero and the right input is set to one.")
    {
        bii = 1;

        const auto simulation_results = quickexact(*bii, params);

        const auto gs = simulation_results.groundstates();

        REQUIRE(!gs.empty());

        is_operational_params op_params{};
        op_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

        SECTION("Correct index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), op_params, std::vector<tt>{create_and_tt()}, 1, input_wires,
                                     output_wires) == operational_status::OPERATIONAL);
        }
        SECTION("Wrong input index")
        {
            CHECK(verify_logic_match(*bii, gs.front(), op_params, std::vector<tt>{create_and_tt()}, 2, input_wires,
                                     output_wires) == operational_status::NON_OPERATIONAL);
        }
    }
}
