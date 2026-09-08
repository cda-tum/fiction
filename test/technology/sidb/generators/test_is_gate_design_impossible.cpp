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
 * @brief Tests for `fiction/technology/sidb/generators/is_gate_design_impossible.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "catch2/catch_test_macros.hpp"
#include "fiction/technology/sidb/generators/is_gate_design_impossible.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::generators;
using namespace fiction::sidb::model;
using namespace fiction::synthesis;

TEST_CASE("Reject an empty gate specification", "[is-gate-design-impossible]")
{
    CHECK_THROWS_AS(is_gate_design_impossible(layout{}, std::vector<tt>{}), std::invalid_argument);
}

TEST_CASE("SiQAD's AND gate with input BDL pairs of different size", "[is-gate-design-impossible]")
{

    layout lyt{};

    lyt.assign_sidb({0, 0, 1}, dot_tag::INPUT);
    lyt.assign_sidb({2, 1, 1}, dot_tag::INPUT);

    lyt.assign_sidb({20, 0, 1}, dot_tag::INPUT);
    lyt.assign_sidb({19, 1, 1}, dot_tag::INPUT);

    lyt.assign_sidb({4, 2, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 3, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({14, 3, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 2, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 6, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({10, 7, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({10, 9, 1}, dot_tag::NORMAL);

    SECTION("without defect")
    {
        CHECK(!is_gate_design_impossible(
            lyt, std::vector<tt>{create_and_tt()},
            is_gate_design_impossible_params{.sim_params = simulation_parameters{2, -0.28}}));
    }

    SECTION("with defect")
    {
        lyt.assign_defect({12, 6, 0}, defect{defect_type::SI_VACANCY, -1, 10, 5});
        lyt.assign_defect({11, 6, 0}, defect{defect_type::SI_VACANCY, -1, 10, 5});
        CHECK(
            is_gate_design_impossible(lyt, std::vector<tt>{create_and_tt()},
                                      is_gate_design_impossible_params{.sim_params = simulation_parameters{2, -0.28}}));
    }
}

TEST_CASE("Bestagon CROSSING gate", "[is-gate-design-impossible]")
{

    layout lyt{};

    lyt.assign_sidb({36, 1, 0}, dot_tag::INPUT);
    lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

    lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({38, 0, 0}, dot_tag::INPUT);

    lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({20, 12, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 11, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({12, 4, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 15, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({26, 4, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({14, 9, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({24, 15, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({12, 16, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({18, 9, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({26, 16, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({24, 13, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({24, 5, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({30, 3, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 13, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({32, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({20, 8, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({30, 17, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({6, 18, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({32, 18, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({8, 17, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({2, 19, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({36, 19, 0}, dot_tag::NORMAL);

    SECTION("without defect")
    {
        CHECK(!is_gate_design_impossible(
            lyt, create_crossing_wire_tt(),
            is_gate_design_impossible_params{.sim_params = simulation_parameters{2, -0.32}}));
    }

    SECTION("with defect")
    {
        lyt.assign_defect({34, 18, 0}, defect{defect_type::SI_VACANCY, -1, 5, 5});
        lyt.assign_defect({34, 18, 1}, defect{defect_type::SI_VACANCY, -1, 5, 5});
        CHECK(
            is_gate_design_impossible(lyt, create_crossing_wire_tt(),
                                      is_gate_design_impossible_params{.sim_params = simulation_parameters{2, -0.32}}));
    }
}
