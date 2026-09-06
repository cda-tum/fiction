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
 * @brief Tests for `fiction/technology/sidb/generators/design_gates.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/generators/design_gates.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <mockturtle/utils/stopwatch.hpp>

#include <cstddef>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::generators;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

TEST_CASE("Gate design propagates worker failures", "[design-sidb-gates]")
{
    auto         lyt = blueprints::two_input_one_output_skeleton_west_west();
    lattice_site invalid{100, 100, 0};
    invalid.z = 2;
    lyt.assign_defect(invalid, defect{defect_type::SI_VACANCY, -1, 5.6, 5.0});

    design_gates_params params{};
    params.canvas                 = {site_at_row(27, 12), site_at_row(28, 12)};
    params.number_of_canvas_sidbs = 1;

    for (const auto mode :
         {design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
          design_gates_params::design_gates_mode::QUICKCELL, design_gates_params::design_gates_mode::PRUNING_ONLY,
          design_gates_params::design_gates_mode::RANDOM})
    {
        params.design_mode = mode;
        CHECK_THROWS_AS(design_gates(lyt, std::vector{create_and_tt()}, params), std::out_of_range);
    }

    params.design_mode = design_gates_params::design_gates_mode::RANDOM;
    CHECK_THROWS_AS(design_gates(lyt, std::vector{create_id_tt()}, params), std::invalid_argument);
}

TEST_CASE("Reject an empty gate specification", "[design-sidb-gates]")
{
    CHECK_THROWS_AS(design_gates(layout{}, std::vector<tt>{}), std::invalid_argument);
}

TEST_CASE("Design AND gate with skeleton, where one input wire and the output wire are orientated to the east.",
          "[design-sidb-gates]")
{
    const auto lyt = blueprints::two_input_one_output_skeleton_west_west();

    design_gates_params params{
        .operational_params =
            is_operational_params{.sim_params                = simulation_parameters{2, -0.31},
                                  .sim_engine                = engine::QUICKEXACT,
                                  .input_bdl_iterator_params = bdl_input_iterator_params{},
                                  .op_condition = is_operational_params::operational_condition::REJECT_KINKS},
        .design_mode            = design_gates_params::design_gates_mode::QUICKCELL,
        .canvas                 = {{27, 6, 0}, {30, 8, 0}},
        .number_of_canvas_sidbs = 3,
        .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

    SECTION("QuickCell")
    {
        design_gates_stats design_gates_stats{};
        const auto         found_gate_layouts =
            design_gates(lyt, std::vector<tt>{create_and_tt()}, params, &design_gates_stats);
        REQUIRE(found_gate_layouts.size() == 10);
        const auto& first_gate = found_gate_layouts.front();
        CHECK(is_operational(first_gate, std::vector<tt>{create_and_tt()}, params.operational_params).first ==
              operational_status::OPERATIONAL);

        CHECK(design_gates_stats.number_of_layouts == 1140);
        CHECK(design_gates_stats.number_of_layouts_after_first_pruning == 167);
        CHECK(design_gates_stats.number_of_layouts_after_second_pruning == 46);
        CHECK(design_gates_stats.number_of_layouts_after_third_pruning == 11);
        CHECK(design_gates_stats.time_total.count() > 0);
    }

    SECTION("Automatic Exhaustive Gate Designer")
    {
        params.design_mode      = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
        params.termination_cond = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED;

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(found_gate_layouts.size() == 10);
        const auto& first_gate = found_gate_layouts.front();
        CHECK(is_operational(first_gate, std::vector<tt>{create_and_tt()}, params.operational_params).first ==
              operational_status::OPERATIONAL);
    }
}

TEST_CASE("Use SiQAD XNOR skeleton and generate SiQAD XNOR gate, exhaustive", "[design-sidb-gates]")
{
    layout lyt{};

    lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

    lyt.assign_sidb({20, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({18, 1, 0}, dot_tag::INPUT);

    lyt.assign_sidb({6, 3, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 3, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({4, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 2, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 6, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({10, 7, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 9, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({10, 10, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({10, 12, 1}, dot_tag::NORMAL);

    CHECK(lyt.num_dots() == 13);

    SECTION("One cell in canvas")
    {
        const auto params = design_gates_params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 4, 0}, {10, 4, 0}},
            .number_of_canvas_sidbs = 1,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_xnor_tt()}, params);

        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts[0].num_dots() == 14);
        CHECK(found_gate_layouts[0].get_dot_tag({10, 4, 0}) == dot_tag::LOGIC);
    }
    SECTION("Four cells in canvas, design all gates with one SiDB in the canvas")
    {
        const auto params = design_gates_params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 4, 0}, {13, 4, 0}},
            .number_of_canvas_sidbs = 1,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_xnor_tt()}, params);

        REQUIRE(found_gate_layouts.size() == 4);
    }
    SECTION("Occupied canvas sites are excluded from exhaustive combinations")
    {
        auto occupied_lyt = lyt;
        occupied_lyt.assign_sidb({10, 4, 0}, dot_tag::LOGIC);

        const auto params = design_gates_params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 4, 0}, {13, 4, 0}},
            .number_of_canvas_sidbs = 1,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        design_gates_stats stats{};
        const auto found_gate_layouts = design_gates(occupied_lyt, std::vector<tt>{create_xnor_tt()}, params, &stats);

        CHECK(stats.number_of_layouts == 3);
        REQUIRE(!found_gate_layouts.empty());

        for (const auto& gate : found_gate_layouts)
        {
            CHECK(gate.num_dots() == occupied_lyt.num_dots() + 1);
        }
        // This canvas yields at most three candidates; use digest buckets if the case grows.
        for (std::size_t i = 0; i < found_gate_layouts.size(); ++i)
        {
            for (std::size_t j = i + 1; j < found_gate_layouts.size(); ++j)
            {
                CHECK(found_gate_layouts[i] != found_gate_layouts[j]);
            }
        }
    }
    SECTION("Random design samples only available canvas sites")
    {
        auto sparse_lyt = lyt;
        sparse_lyt.assign_defect({11, 4, 0}, defect{defect_type::DB, 0});
        sparse_lyt.assign_defect({12, 4, 0}, defect{defect_type::DB, 0});
        sparse_lyt.assign_defect({13, 4, 0}, defect{defect_type::DB, 0});

        const auto params = design_gates_params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode                    = design_gates_params::design_gates_mode::RANDOM,
            .canvas                         = {{10, 4, 0}, {13, 4, 0}},
            .number_of_canvas_sidbs         = 1,
            .maximal_random_design_attempts = 1};

        design_gates_stats stats{};
        const auto found_gate_layouts = design_gates(sparse_lyt, std::vector<tt>{create_xnor_tt()}, params, &stats);

        CHECK(stats.number_of_layouts == 1);
        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts.front().get_dot_tag({10, 4, 0}) == dot_tag::LOGIC);
        CHECK(found_gate_layouts.front().defects() == sparse_lyt.defects());
    }
    SECTION("one SiDB in the canvas, terminate after first solution is found, QuickExact")
    {
        const auto params = design_gates_params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 4, 0}, {10, 4, 0}},
            .number_of_canvas_sidbs = 1,
            .termination_cond       = design_gates_params::termination_condition::AFTER_FIRST_SOLUTION};

        design_gates_stats stats{};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_xnor_tt()}, params, &stats);

        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts[0].num_dots() == 14);
        CHECK(found_gate_layouts[0].get_dot_tag({10, 4, 0}) == dot_tag::LOGIC);
        CHECK(mockturtle::to_seconds(stats.time_total) > 0.0);
        CHECK(stats.sim_engine == engine::QUICKEXACT);
    }
    SECTION("one SiDB in the canvas, terminate after first solution is found, QuickSim")
    {
        const auto params = design_gates_params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKSIM},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 4, 0}, {10, 4, 0}},
            .number_of_canvas_sidbs = 1,
            .termination_cond       = design_gates_params::termination_condition::AFTER_FIRST_SOLUTION};

        design_gates_stats stats{};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_xnor_tt()}, params, &stats);

        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts[0].num_dots() == 14);
        CHECK(found_gate_layouts[0].get_dot_tag({10, 4, 0}) == dot_tag::LOGIC);
        CHECK(mockturtle::to_seconds(stats.time_total) > 0.0);
        CHECK(stats.sim_engine == engine::QUICKSIM);
    }
}

TEST_CASE("Use SiQAD's AND gate skeleton to generate all possible AND gates", "[design-sidb-gates]")
{
    layout lyt{};

    lyt.assign_sidb({0, 0, 1}, dot_tag::INPUT);
    lyt.assign_sidb({2, 1, 1}, dot_tag::INPUT);

    lyt.assign_sidb({20, 0, 1}, dot_tag::INPUT);
    lyt.assign_sidb({18, 1, 1}, dot_tag::INPUT);

    lyt.assign_sidb({4, 2, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 3, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({14, 3, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 2, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 6, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({10, 7, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({10, 9, 1}, dot_tag::NORMAL);

    design_gates_params params{
        .operational_params =
            is_operational_params{
                .sim_params                = simulation_parameters{2, -0.28},
                .sim_engine                = engine::EXGS,
                .input_bdl_iterator_params = {.bdl_wire_params =
                                                  detect_bdl_wires_params{.threshold_bdl_interdistance = 2.0}}},
        .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        .canvas                 = {{4, 4, 0}, {14, 5, 1}},
        .number_of_canvas_sidbs = 1};

    SECTION("Exhaustive Generation")
    {
        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        CHECK(!found_gate_layouts.empty());
    }

    SECTION("Random Generation")
    {
        params.design_mode            = design_gates_params::design_gates_mode::RANDOM;
        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        CHECK(!found_gate_layouts.empty());
    }

    SECTION("no canvas")
    {
        params.canvas                 = {{4, 4, 0}, {4, 4, 0}};
        params.number_of_canvas_sidbs = 0;
        params.design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
        const auto found_gate_layouts_exhaustive = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        CHECK(found_gate_layouts_exhaustive.empty());
        params.design_mode                      = design_gates_params::design_gates_mode::QUICKCELL;
        const auto found_gate_layouts_quickcell = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        CHECK(found_gate_layouts_quickcell.empty());
    }

    SECTION("canvas hits wires and defect lies inside the canvas")
    {
        params.canvas                 = {{4, 2, 0}, {14, 8, 1}};
        params.number_of_canvas_sidbs = 2;
        params.design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;

        layout defect_layout{lyt};
        defect_layout.assign_defect({10, 6, 0},
                                    defect{defect_type::DB, -1, params.operational_params.sim_params.epsilon_r,
                                           params.operational_params.sim_params.lambda_tf});

        const auto found_gate_layouts_exhaustive =
            design_gates(defect_layout, std::vector<tt>{create_and_tt()}, params);
        CHECK(!found_gate_layouts_exhaustive.empty());
        params.design_mode                      = design_gates_params::design_gates_mode::QUICKCELL;
        const auto found_gate_layouts_quickcell = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        CHECK(!found_gate_layouts_quickcell.empty());
    }
}

TEST_CASE("Use FO2 Bestagon gate without SiDB at {17, 11, 0} and generate original one", "[design-sidb-gates]")
{
    layout lyt{};

    lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

    lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({12, 4, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({18, 6, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({19, 7, 0}, dot_tag::NORMAL);

    // canvas SiDBs
    // SiDB, originally part of the Bestagon fo2 gate, is excluded.
    // lyt.assign_sidb({17, 11, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({21, 11, 1}, dot_tag::LOGIC);
    lyt.assign_sidb({18, 13, 0}, dot_tag::LOGIC);
    // ----------------------------

    lyt.assign_sidb({24, 15, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({26, 16, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({12, 16, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 15, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({8, 17, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({6, 18, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({30, 17, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({32, 18, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({36, 19, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({2, 19, 0}, dot_tag::NORMAL);

    SECTION("generate original FO2")
    {
        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{17, 11, 0}, {17, 11, 0}},
            .number_of_canvas_sidbs = 1};

        CHECK(lyt.get_dot_tag({17, 11, 0}) == dot_tag::EMPTY);

        // generate gate by placing one SiDB
        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_fan_out_tt()}, params);

        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts[0].num_dots() == 21);
        CHECK(found_gate_layouts[0].get_dot_tag({17, 11, 0}) == dot_tag::LOGIC);
    }

#if (FICTION_ALGLIB_ENABLED)
    SECTION("replace the output perturbers by equivalent negatively charged defects")
    {
        design_gates_params params{
            .operational_params     = is_operational_params{.sim_params = simulation_parameters{2, -0.32},
                                                            .sim_engine = engine::CLUSTERCOMPLETE},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{17, 11, 0}, {17, 11, 0}},
            .number_of_canvas_sidbs = 1};

        layout defect_layout{lyt};
        defect_layout.assign_sidb({36, 19, 0}, dot_tag::EMPTY);
        defect_layout.assign_sidb({2, 19, 0}, dot_tag::EMPTY);
        CHECK(defect_layout.get_dot_tag({36, 19, 0}) == dot_tag::EMPTY);
        CHECK(defect_layout.get_dot_tag({2, 19, 0}) == dot_tag::EMPTY);

        defect_layout.assign_defect({36, 19, 0},
                                    defect{defect_type::DB, -1, params.operational_params.sim_params.epsilon_r,
                                           params.operational_params.sim_params.lambda_tf});
        defect_layout.assign_defect({2, 19, 0},
                                    defect{defect_type::DB, -1, params.operational_params.sim_params.epsilon_r,
                                           params.operational_params.sim_params.lambda_tf});

        const auto found_gate_layouts_exhaustive =
            design_gates(defect_layout, std::vector<tt>{create_fan_out_tt()}, params);

        REQUIRE(found_gate_layouts_exhaustive.size() == 1);
        CHECK(found_gate_layouts_exhaustive[0].num_dots() == 19);
        CHECK(found_gate_layouts_exhaustive[0].get_dot_tag({17, 11, 0}) == dot_tag::LOGIC);

        params.design_mode = design_gates_params::design_gates_mode::QUICKCELL;

        const auto found_gate_layouts_quickcell =
            design_gates(defect_layout, std::vector<tt>{create_fan_out_tt()}, params);
        REQUIRE(found_gate_layouts_quickcell.size() == 1);
        CHECK(found_gate_layouts_quickcell[0].num_dots() == 19);
        CHECK(found_gate_layouts_quickcell[0].get_dot_tag({17, 11, 0}) == dot_tag::LOGIC);
    }
#endif  // FICTION_ALGLIB_ENABLED
}

TEST_CASE("Design AND Bestagon shaped gate", "[design-sidb-gates]")
{
    const auto lyt = blueprints::two_input_one_output_bestagon_skeleton();

    SECTION("Random Generation")
    {
        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                      .sim_engine                = engine::QUICKEXACT,
                                      .input_bdl_iterator_params = bdl_input_iterator_params{},
                                      .op_condition = is_operational_params::operational_condition::TOLERATE_KINKS},
            .design_mode            = design_gates_params::design_gates_mode::RANDOM,
            .canvas                 = {{14, 6, 0}, {24, 12, 0}},
            .number_of_canvas_sidbs = 3};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(!found_gate_layouts.empty());
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
    }

    SECTION("Random generation attempt limit")
    {
        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode                    = design_gates_params::design_gates_mode::RANDOM,
            .canvas                         = {{14, 6, 0}, {24, 12, 0}},
            .number_of_canvas_sidbs         = 3,
            .maximal_random_design_attempts = 0};

        CHECK(design_gates(lyt, std::vector<tt>{create_and_tt()}, params).empty());
    }

    SECTION("Random and QuickCell with defects")
    {
        layout defect_layout{lyt};

        design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                      .sim_engine                = engine::QUICKEXACT,
                                      .input_bdl_iterator_params = bdl_input_iterator_params{},
                                      .op_condition = is_operational_params::operational_condition::REJECT_KINKS},
            .design_mode            = design_gates_params::design_gates_mode::RANDOM,
            .canvas                 = {{18, 8, 1}, {22, 12, 0}},
            .number_of_canvas_sidbs = 2};

        defect_layout.assign_defect({14, 10, 0},
                                    defect{defect_type::DB, -1, params.operational_params.sim_params.epsilon_r,
                                           params.operational_params.sim_params.lambda_tf});

        const auto found_gate_layouts = design_gates(defect_layout, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(!found_gate_layouts.empty());
        CHECK(found_gate_layouts.front().num_defects() == 1);
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 2);

        found_gate_layouts.front().foreach_dot([](const auto& cell) { CHECK(cell != lattice_site{14, 10, 0}); });

        params.design_mode                      = design_gates_params::design_gates_mode::QUICKCELL;
        const auto found_gate_layouts_quickcell = design_gates(defect_layout, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(!found_gate_layouts_quickcell.empty());
        CHECK(found_gate_layouts_quickcell.front().num_defects() == 1);
        CHECK(found_gate_layouts_quickcell.front().num_dots() == lyt.num_dots() + 2);

        found_gate_layouts_quickcell.front().foreach_dot([](const auto& cell)
                                                         { CHECK(cell != lattice_site{14, 10, 0}); });
    }

    SECTION("QuickCell with defect blocking canvas SiDB placement")
    {
        layout defect_layout{lyt};

        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                      .sim_engine                = engine::QUICKEXACT,
                                      .input_bdl_iterator_params = bdl_input_iterator_params{},
                                      .op_condition = is_operational_params::operational_condition::REJECT_KINKS},
            .design_mode            = design_gates_params::design_gates_mode::RANDOM,
            .canvas                 = {{14, 10, 0}, {14, 10, 0}},
            .number_of_canvas_sidbs = 1};

        defect_layout.assign_defect({14, 10, 0},
                                    defect{defect_type::DB, -1, params.operational_params.sim_params.epsilon_r,
                                           params.operational_params.sim_params.lambda_tf});

        const auto found_gate_layouts = design_gates(defect_layout, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(found_gate_layouts.empty());
    }
}

TEST_CASE("Design NOR Bestagon shaped gate on H-Si 111", "[design-sidb-gates]")
{
    auto lyt = blueprints::and_gate_111();

    // delete canvas SiDBs
    for (const auto& c : lyt.dots_with_tag(dot_tag::LOGIC))
    {
        lyt.assign_sidb(c, dot_tag::EMPTY);
    }

    SECTION("Random Generation")
    {
        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::RANDOM,
            .canvas                 = {{10, 11, 0}, {14, 15, 0}},
            .number_of_canvas_sidbs = 3};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
        REQUIRE(!found_gate_layouts.empty());
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
    }

    SECTION("Exhaustive Generation, allowing kinks")
    {
        design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 13, 0}, {14, 17, 0}},
            .number_of_canvas_sidbs = 3,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};
// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
        SECTION("all design")
        {
            const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
            REQUIRE(found_gate_layouts.size() == 14);
            CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
        }
#endif

        SECTION("terminate after first solution is found")
        {
            params.termination_cond       = design_gates_params::termination_condition::AFTER_FIRST_SOLUTION;
            const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
            REQUIRE(found_gate_layouts.size() <= std::thread::hardware_concurrency());
            CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
        }
    }

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
    SECTION("Exhaustive Generation, forbidding kinks")
    {
        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                      .sim_engine                = engine::QUICKEXACT,
                                      .input_bdl_iterator_params = bdl_input_iterator_params{},
                                      .op_condition = is_operational_params::operational_condition::REJECT_KINKS},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 13, 0}, {15, 17, 0}},
            .number_of_canvas_sidbs = 3,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
        REQUIRE(found_gate_layouts.size() == 3);
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
    }
#endif

    SECTION("Exhaustive Generation, QuickCell")
    {
        const design_gates_params params{
            .operational_params     = is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                                            .sim_engine                = engine::QUICKEXACT,
                                                            .input_bdl_iterator_params = bdl_input_iterator_params{}},
            .design_mode            = design_gates_params::design_gates_mode::QUICKCELL,
            .canvas                 = {{10, 13, 0}, {15, 17, 0}},
            .number_of_canvas_sidbs = 3,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
        REQUIRE(found_gate_layouts.size() == 3);
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
    }
    SECTION("Stop after first gate design is finished, QuickCell")
    {
        const design_gates_params params{
            .operational_params     = is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                                            .sim_engine                = engine::QUICKEXACT,
                                                            .input_bdl_iterator_params = bdl_input_iterator_params{}},
            .design_mode            = design_gates_params::design_gates_mode::QUICKCELL,
            .canvas                 = {{8, 13, 0}, {17, 17, 0}},
            .number_of_canvas_sidbs = 3,
            .termination_cond       = design_gates_params::termination_condition::AFTER_FIRST_SOLUTION};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
        REQUIRE(found_gate_layouts.size() <= std::thread::hardware_concurrency());
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
    }
}

TEST_CASE("Design hexagonal CX gate with pruning only", "[design-sidb-gates]")
{
    const auto lyt = blueprints::two_input_two_output_bestagon_skeleton();

    const design_gates_params params{
        .operational_params     = is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                                        .sim_engine                = engine::QUICKEXACT,
                                                        .input_bdl_iterator_params = bdl_input_iterator_params{}},
        .design_mode            = design_gates_params::design_gates_mode::PRUNING_ONLY,
        .canvas                 = {{16, 8, 0}, {22, 14, 0}},
        .number_of_canvas_sidbs = 3};

    const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_crossing_wire_tt()}, params);
    REQUIRE(found_gate_layouts.size() == 3);
    CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("Design Bestagon shaped CX gate with QuickCell", "[design-sidb-gates]")
{
    const auto lyt = blueprints::two_input_two_output_bestagon_skeleton();

    SECTION("Exhaustive Generation, QuickCell")
    {
        const design_gates_params params{
            .operational_params     = is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                                            .sim_engine                = engine::QUICKEXACT,
                                                            .input_bdl_iterator_params = bdl_input_iterator_params{}},
            .design_mode            = design_gates_params::design_gates_mode::QUICKCELL,
            .canvas                 = {{16, 8, 0}, {22, 14, 0}},
            .number_of_canvas_sidbs = 3,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_crossing_wire_tt()}, params);
        REQUIRE(found_gate_layouts.size() == 3);
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
    }
}

TEST_CASE("Design Bestagon shaped CX gate with QuickCell (flipped)", "[design-sidb-gates]")
{
    const auto lyt = blueprints::two_input_two_output_bestagon_skeleton_input_down_output_up();

    SECTION("Exhaustive Generation, QuickCell")
    {
        const design_gates_params params{
            .operational_params     = is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                                            .sim_engine                = engine::QUICKEXACT,
                                                            .input_bdl_iterator_params = bdl_input_iterator_params{}},
            .design_mode            = design_gates_params::design_gates_mode::QUICKCELL,
            .canvas                 = {{16, 7, 0}, {22, 15, 0}},
            .number_of_canvas_sidbs = 3,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        const auto found_gate_layouts = design_gates(lyt, create_crossing_wire_tt(), params);
        REQUIRE(found_gate_layouts.size() == 3);
        CHECK(found_gate_layouts.front().num_dots() == lyt.num_dots() + 3);
    }
}

TEST_CASE("Design AND gate with input left and output top-right with QuickCell (flipped)", "[design-sidb-gates]")
{
    const auto lyt = blueprints::two_input_left_one_output_right_top_skeleton();

    SECTION("Exhaustive Generation, QuickCell")
    {
        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                      .sim_engine                = engine::QUICKEXACT,
                                      .input_bdl_iterator_params = bdl_input_iterator_params{},
                                      .op_condition = is_operational_params::operational_condition::REJECT_KINKS},
            .design_mode            = design_gates_params::design_gates_mode::QUICKCELL,
            .canvas                 = {{17, 5, 0}, {24, 8, 0}},
            .number_of_canvas_sidbs = 3,
            .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(found_gate_layouts.size() == 234);
        const auto& first_gate = found_gate_layouts.front();
        CHECK(is_operational(first_gate, std::vector<tt>{create_and_tt()}, params.operational_params).first ==
              operational_status::OPERATIONAL);
    }
}

#endif

TEST_CASE("Gate design propagates worker simulation errors", "[design-sidb-gates]")
{
    auto lyt = blueprints::two_input_one_output_skeleton_west_west();
    lyt.assign_defect({100, 100}, defect{defect_type::DB, -1});
    design_gates_params params{};
    params.operational_params.sim_engine      = engine::QUICKSIM;
    params.operational_params.sim_params.base = 2;
    params.design_mode                        = decltype(params)::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
    params.canvas                             = {{27, 6, 0}, {27, 6, 0}};
    params.number_of_canvas_sidbs             = 1;
    CHECK_THROWS_AS(design_gates(lyt, std::vector<tt>{create_and_tt()}, params), std::invalid_argument);
}

TEST_CASE("Random gate design bounds work without enumerating canvas layouts", "[design-sidb-gates]")
{
    auto                lyt = blueprints::two_input_one_output_bestagon_skeleton();
    design_gates_params params{.design_mode                    = design_gates_params::design_gates_mode::RANDOM,
                               .canvas                         = {{0, 0, 0}, {49, 0, 0}},
                               .number_of_canvas_sidbs         = 25,
                               .maximal_random_design_attempts = 0};
    design_gates_stats  stats{};

    CHECK(design_gates(lyt, std::vector<tt>{create_and_tt()}, params, &stats).empty());
    CHECK(stats.number_of_layouts == 30'957'699'535'776ULL);

    params.canvas                 = {{0, 0, 0}, {67, 0, 0}};
    params.number_of_canvas_sidbs = 34;
    CHECK(design_gates(lyt, std::vector<tt>{create_and_tt()}, params, &stats).empty());
    CHECK(stats.number_of_layouts == 7'007'092'303'604'022'630ULL);

    params.canvas                 = {{100, 100, 0}, {103, 100, 0}};
    params.number_of_canvas_sidbs = 2;
    lyt.assign_sidb({100, 100, 0}, dot_tag::LOGIC);
    lyt.assign_defect({101, 100, 0}, defect{defect_type::DB, -1, 5.6, 5.0});
    CHECK(design_gates(lyt, std::vector<tt>{create_and_tt()}, params, &stats).empty());
    CHECK(stats.number_of_layouts == 1);

    params.number_of_canvas_sidbs         = 3;
    params.maximal_random_design_attempts = 1;
    CHECK(design_gates(lyt, std::vector<tt>{create_and_tt()}, params, &stats).empty());
    CHECK(stats.number_of_layouts == 0);
}

TEST_CASE("Gate design reports progress", "[design-sidb-gates]")
{
    const auto lyt = blueprints::two_input_one_output_skeleton_west_west();

    progress_recorder rec{};

    design_gates_params params{
        .operational_params =
            is_operational_params{.sim_params                = simulation_parameters{2, -0.31},
                                  .sim_engine                = engine::QUICKEXACT,
                                  .input_bdl_iterator_params = bdl_input_iterator_params{},
                                  .op_condition = is_operational_params::operational_condition::REJECT_KINKS},
        .canvas                 = {{27, 6, 0}, {30, 8, 0}},
        .number_of_canvas_sidbs = 3,
        .termination_cond       = design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED};
    params.on_progress = rec.callback();

    SECTION("QuickCell")
    {
        params.design_mode = design_gates_params::design_gates_mode::QUICKCELL;

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);

        CHECK(found_gate_layouts.size() == 10);

        // every canvas layout is pruned, and every survivor is simulated
        CHECK(rec.is_consistent("pruning"));
        CHECK(rec.final_count("pruning") == 1140);
        CHECK(rec.is_consistent("candidates"));
        CHECK(rec.final_count("candidates") == 11);
    }

    SECTION("Automatic Exhaustive Gate Designer")
    {
        params.design_mode = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);

        CHECK(found_gate_layouts.size() == 10);

        CHECK(rec.is_consistent("canvas layouts"));
        CHECK(rec.final_count("canvas layouts") == 1140);
    }

    SECTION("Random")
    {
        params.design_mode      = design_gates_params::design_gates_mode::RANDOM;
        params.termination_cond = design_gates_params::termination_condition::AFTER_FIRST_SOLUTION;

        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_and_tt()}, params);

        CHECK(!found_gate_layouts.empty());

        // the number of attempts is unknown in advance
        CHECK(rec.is_consistent("attempts"));
        CHECK(rec.final_count("attempts") > 0);
    }
}
