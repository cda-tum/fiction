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

#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
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
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <mockturtle/utils/stopwatch.hpp>

#include <thread>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::generators;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

TEST_CASE("Design AND gate with skeleton, where one input wire and the output wire are orientated to the east.",
          "[design-sidb-gates]")
{
    const auto lyt = to_sidb_layout(blueprints::two_input_one_output_skeleton_west_west<sidb_100_cell_clk_lyt_siqad>());

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
    using offset_layout = sidb_100_cell_clk_lyt;
    using siqad_layout  = sidb_100_cell_clk_lyt_siqad;
    using cube_layout   = sidb_100_cell_clk_lyt_cube;

    siqad_layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 9, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 10, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 12, 1}, sidb_technology::cell_type::NORMAL);

    CHECK(lyt.num_cells() == 13);

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
        CHECK(found_gate_layouts[0].num_cells() == 14);
        CHECK(found_gate_layouts[0].get_cell_type({10, 4, 0}) == siqad_layout::technology::cell_type::LOGIC);

        // using cube coordinates
        const auto                lyt_in_cube_coord = convert_layout_to_fiction_coordinates<cube_layout>(lyt);
        const design_gates_params params_cube{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 4, 0}, {10, 4, 0}},
            .number_of_canvas_sidbs = 1};

        const auto found_gate_layouts_cube =
            design_gates(lyt_in_cube_coord, std::vector<tt>{create_xnor_tt()}, params_cube);

        REQUIRE(found_gate_layouts_cube.size() == 1);
        CHECK(found_gate_layouts_cube[0].num_cells() == 14);
        CHECK(found_gate_layouts_cube[0].get_cell_type({10, 8}) == siqad_layout::technology::cell_type::LOGIC);

        // using offset coordinates
        const auto                lyt_in_offset_coord = convert_layout_to_fiction_coordinates<offset_layout>(lyt);
        const design_gates_params params_offset{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{10, 4, 0}, {10, 4, 0}},
            .number_of_canvas_sidbs = 1};

        const auto found_gate_layouts_offset =
            design_gates(lyt_in_offset_coord, std::vector<tt>{create_xnor_tt()}, params_offset);

        REQUIRE(found_gate_layouts_offset.size() == 1);
        CHECK(found_gate_layouts_offset[0].num_cells() == 14);
        CHECK(found_gate_layouts_offset[0].get_cell_type({10, 8}) == offset_layout::technology::cell_type::LOGIC);
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
        CHECK(found_gate_layouts[0].num_cells() == 14);
        CHECK(found_gate_layouts[0].get_cell_type({10, 4, 0}) == siqad_layout::technology::cell_type::LOGIC);
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
        CHECK(found_gate_layouts[0].num_cells() == 14);
        CHECK(found_gate_layouts[0].get_cell_type({10, 4, 0}) == siqad_layout::technology::cell_type::LOGIC);
        CHECK(mockturtle::to_seconds(stats.time_total) > 0.0);
        CHECK(stats.sim_engine == engine::QUICKSIM);
    }
}

TEST_CASE("Use SiQAD's AND gate skeleton to generate all possible AND gates", "[design-sidb-gates]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

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

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 7, 0}, sidb_technology::cell_type::NORMAL);

    // canvas SiDBs
    // SiDB, originally part of the Bestagon fo2 gate, is excluded.
    // lyt.assign_cell_type({17, 11, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({21, 11, 1}, sidb_technology::cell_type::LOGIC);
    lyt.assign_cell_type({18, 13, 0}, sidb_technology::cell_type::LOGIC);
    // ----------------------------

    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("generate original FO2")
    {
        const design_gates_params params{
            .operational_params =
                is_operational_params{.sim_params = simulation_parameters{2, -0.32}, .sim_engine = engine::QUICKEXACT},
            .design_mode            = design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            .canvas                 = {{17, 11, 0}, {17, 11, 0}},
            .number_of_canvas_sidbs = 1};

        CHECK(lyt.get_cell_type({17, 11, 0}) == sidb_technology::cell_type::EMPTY);

        // generate gate by placing one SiDB
        const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_fan_out_tt()}, params);

        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts[0].num_cells() == 21);
        CHECK(found_gate_layouts[0].get_cell_type({17, 11, 0}) == sidb_technology::cell_type::LOGIC);
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
        defect_layout.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::EMPTY);
        defect_layout.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::EMPTY);
        CHECK(defect_layout.get_cell_type({36, 19, 0}) == sidb_technology::cell_type::EMPTY);
        CHECK(defect_layout.get_cell_type({2, 19, 0}) == sidb_technology::cell_type::EMPTY);

        defect_layout.assign_defect({36, 19, 0},
                                    defect{defect_type::DB, -1, params.operational_params.sim_params.epsilon_r,
                                           params.operational_params.sim_params.lambda_tf});
        defect_layout.assign_defect({2, 19, 0},
                                    defect{defect_type::DB, -1, params.operational_params.sim_params.epsilon_r,
                                           params.operational_params.sim_params.lambda_tf});

        const auto found_gate_layouts_exhaustive =
            design_gates(defect_layout, std::vector<tt>{create_fan_out_tt()}, params);

        REQUIRE(found_gate_layouts_exhaustive.size() == 1);
        CHECK(found_gate_layouts_exhaustive[0].num_cells() == 19);
        CHECK(found_gate_layouts_exhaustive[0].get_cell_type({17, 11, 0}) == sidb_technology::cell_type::LOGIC);

        params.design_mode = design_gates_params::design_gates_mode::QUICKCELL;

        const auto found_gate_layouts_quickcell =
            design_gates(defect_layout, std::vector<tt>{create_fan_out_tt()}, params);
        REQUIRE(found_gate_layouts_quickcell.size() == 1);
        CHECK(found_gate_layouts_quickcell[0].num_cells() == 19);
        CHECK(found_gate_layouts_quickcell[0].get_cell_type({17, 11, 0}) == sidb_technology::cell_type::LOGIC);
    }
#endif  // FICTION_ALGLIB_ENABLED
}

TEST_CASE("Design AND Bestagon shaped gate", "[design-sidb-gates]")
{
    const auto lyt = to_sidb_layout(blueprints::two_input_one_output_bestagon_skeleton<sidb_cell_clk_lyt_siqad>());

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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 2);

        found_gate_layouts.front().foreach_cell([](const auto& cell) { CHECK(cell != lattice_site{14, 10, 0}); });

        params.design_mode                      = design_gates_params::design_gates_mode::QUICKCELL;
        const auto found_gate_layouts_quickcell = design_gates(defect_layout, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(!found_gate_layouts_quickcell.empty());
        CHECK(found_gate_layouts_quickcell.front().num_defects() == 1);
        CHECK(found_gate_layouts_quickcell.front().num_cells() == lyt.num_cells() + 2);

        found_gate_layouts_quickcell.front().foreach_cell([](const auto& cell)
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
    auto lyt = to_sidb_layout(blueprints::and_gate_111<sidb_111_cell_clk_lyt_siqad>(), lattice::si_111_1x1());

    // delete canvas SiDBs
    for (const auto& c : lyt.cells_of_type(sidb_technology::cell_type::LOGIC))
    {
        lyt.assign_cell_type(c, sidb_technology::cell_type::EMPTY);
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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
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
            CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
        }
#endif

        SECTION("terminate after first solution is found")
        {
            params.termination_cond       = design_gates_params::termination_condition::AFTER_FIRST_SOLUTION;
            const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
            REQUIRE(found_gate_layouts.size() <= std::thread::hardware_concurrency());
            CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
    }
}

TEST_CASE("Design hexagonal CX gate with pruning only", "[design-sidb-gates]")
{
    const auto lyt = to_sidb_layout(blueprints::two_input_two_output_bestagon_skeleton<sidb_100_cell_clk_lyt_siqad>());

    const design_gates_params params{
        .operational_params     = is_operational_params{.sim_params                = simulation_parameters{2, -0.32},
                                                        .sim_engine                = engine::QUICKEXACT,
                                                        .input_bdl_iterator_params = bdl_input_iterator_params{}},
        .design_mode            = design_gates_params::design_gates_mode::PRUNING_ONLY,
        .canvas                 = {{16, 8, 0}, {22, 14, 0}},
        .number_of_canvas_sidbs = 3};

    const auto found_gate_layouts = design_gates(lyt, std::vector<tt>{create_crossing_wire_tt()}, params);
    REQUIRE(found_gate_layouts.size() == 3);
    CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("Design Bestagon shaped CX gate with QuickCell", "[design-sidb-gates]")
{
    const auto lyt = to_sidb_layout(blueprints::two_input_two_output_bestagon_skeleton<sidb_100_cell_clk_lyt_siqad>());

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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
    }
}

TEST_CASE("Design Bestagon shaped CX gate with QuickCell (flipped)", "[design-sidb-gates]")
{
    const auto lyt = to_sidb_layout(
        blueprints::two_input_two_output_bestagon_skeleton_input_down_output_up<sidb_100_cell_clk_lyt_siqad>());

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
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
    }
}

TEST_CASE("Design AND gate with input left and output top-right with QuickCell (flipped)", "[design-sidb-gates]")
{
    const auto lyt =
        to_sidb_layout(blueprints::two_input_left_one_output_right_top_skeleton<sidb_100_cell_clk_lyt_siqad>());

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
