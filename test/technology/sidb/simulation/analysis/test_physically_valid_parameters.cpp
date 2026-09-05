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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::sidb::simulation::logic;

TEST_CASE("Reject charge distributions from another layout", "[determine-physically-valid-parameters]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    layout other{};
    other.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::NORMAL);

    CHECK(physically_valid_parameters(lyt, charge_distribution{other}).empty());
}

TEST_CASE("Determine physical parameters for CDS of SiQAD Y-shaped AND gate, 10 input combination",
          "[determine-physically-valid-parameters]")
{

    layout lyt{lattice::si_100_2x1(), "AND gate"};

    lyt.assign_cell_type({-2, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 7, 1}, sidb_technology::cell_type::NORMAL);

    simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution cd{lyt};

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;

    op_domain_params.sweep_dimensions = {
        operational_domain_value_range{.dimension = sweep_parameter::EPSILON_R, .min = 4.1, .max = 6.0, .step = 0.1},
        operational_domain_value_range{.dimension = sweep_parameter::LAMBDA_TF, .min = 4.1, .max = 6.0, .step = 0.1}};

    SECTION("Using the typical ground state as given CDS")
    {
        op_domain_params.sweep_dimensions[0].step = 0.3;
        op_domain_params.sweep_dimensions[1].step = 0.3;

        cd.assign_charge_state({-2, -1, 1}, charge_state::NEGATIVE);
        cd.assign_charge_state({0, 0, 1}, charge_state::NEUTRAL);
        cd.assign_charge_state({12, 0, 1}, charge_state::NEGATIVE);
        cd.assign_charge_state({2, 1, 1}, charge_state::NEGATIVE);
        cd.assign_charge_state({10, 1, 1}, charge_state::NEUTRAL);
        cd.assign_charge_state({6, 4, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({6, 5, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({6, 7, 1}, charge_state::NEGATIVE);

        const auto valid_parameters = physically_valid_parameters(lyt, cd, op_domain_params);
        CHECK(valid_parameters.size() == 43);

        op_domain_params.operational_params.sim_engine = engine::EXGS;
        const auto valid_parameters_exgs               = physically_valid_parameters(lyt, cd, op_domain_params);
        CHECK(valid_parameters_exgs.size() == 43);

        op_domain_params.operational_params.sim_engine = engine::QUICKSIM;
        const auto valid_parameters_quicksim           = physically_valid_parameters(lyt, cd, op_domain_params);
        CHECK(valid_parameters_quicksim.size() == 43);
    }

    SECTION("Using the 2nd excited charge distribution for default physical parameters as given CDS")
    {
        cd.assign_charge_state({-2, -1, 1}, charge_state::NEGATIVE);
        cd.assign_charge_state({0, 0, 1}, charge_state::NEUTRAL);
        cd.assign_charge_state({12, 0, 1}, charge_state::NEGATIVE);
        cd.assign_charge_state({2, 1, 1}, charge_state::NEGATIVE);
        cd.assign_charge_state({10, 1, 1}, charge_state::NEGATIVE);
        cd.assign_charge_state({6, 4, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({6, 5, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({6, 7, 1}, charge_state::NEGATIVE);

        const auto valid_parameters = physically_valid_parameters(lyt, cd, op_domain_params);
        CHECK(valid_parameters.size() == 98);

        const auto p1 = valid_parameters.contains(parameter_point{{5.9, 5.5}});
        REQUIRE(p1.has_value());
        CHECK(std::get<0>(p1.value_or(decltype(p1)::value_type{})) == 1);

        const auto p2 = valid_parameters.contains(parameter_point{{5.8, 4.4}});
        REQUIRE(p2.has_value());
        CHECK(std::get<0>(p2.value_or(decltype(p2)::value_type{})) == 0);

        const auto p3 = valid_parameters.contains(parameter_point{{5.8, 4.4}});
        REQUIRE(p3.has_value());
        CHECK(std::get<0>(p3.value_or(decltype(p3)::value_type{})) == 0);

        const auto p4 = valid_parameters.contains(parameter_point{{6.0, 6.0}});
        REQUIRE(p4.has_value());
        CHECK(std::get<0>(p4.value_or(decltype(p4)::value_type{})) == 1);
    }
}

TEST_CASE(
    "Determine physical parameters for CDS (default physical parameters) of Bestagon AND gate, 10 input combination",
    "[determine-physically-valid-parameters], [quality]")
{
    auto bestagon_and = to_sidb_layout(blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>());

    bestagon_and.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::EMPTY);
    bestagon_and.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::EMPTY);

    simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution cd{bestagon_and};

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;

    SECTION("Using the ground state of default physical parameters as given CDS, two dimensional sweep")
    {
        op_domain_params.sweep_dimensions = {operational_domain_value_range{.dimension = sweep_parameter::EPSILON_R,
                                                                            .min       = 5.0,
                                                                            .max       = 5.9,
                                                                            .step      = 0.1},
                                             operational_domain_value_range{.dimension = sweep_parameter::LAMBDA_TF,
                                                                            .min       = 5.0,
                                                                            .max       = 5.9,
                                                                            .step      = 0.1}};

        cd.assign_charge_state({38, 0, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({2, 1, 0}, charge_state::NEGATIVE);

        cd.assign_charge_state({6, 2, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({32, 2, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({8, 3, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({30, 3, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({12, 4, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({26, 4, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({14, 5, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({24, 5, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({19, 8, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({18, 9, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({23, 9, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({18, 11, 1}, charge_state::NEUTRAL);

        cd.assign_charge_state({19, 13, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({20, 14, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({24, 15, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({26, 16, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({30, 17, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({32, 18, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({36, 19, 0}, charge_state::NEGATIVE);

        const auto valid_parameters = physically_valid_parameters(bestagon_and, cd, op_domain_params);
        REQUIRE(valid_parameters.size() == 100);

        const auto p1 = valid_parameters.contains(parameter_point{{5.6, 5.0}});
        REQUIRE(p1.has_value());
        CHECK(std::get<0>(p1.value_or(decltype(p1)::value_type{})) == 0);

        const auto p2 = valid_parameters.contains(parameter_point{{5.0, 5.9}});
        REQUIRE(p2.has_value());
        CHECK(std::get<0>(p2.value_or(decltype(p2)::value_type{})) == 2);

        const auto p3 = valid_parameters.contains(parameter_point{{5.4, 5.3}});
        REQUIRE(p3.has_value());
        CHECK(std::get<0>(p3.value_or(decltype(p3)::value_type{})) == 1);

        const auto p4 = valid_parameters.contains(parameter_point{{5.8, 5.3}});
        REQUIRE(p4.has_value());
        CHECK(std::get<0>(p4.value_or(decltype(p4)::value_type{})) == 0);
    }

    SECTION("Using the ground state of default physical parameters as given CDS, three dimensional sweep")
    {
        op_domain_params.sweep_dimensions = {operational_domain_value_range{.dimension = sweep_parameter::EPSILON_R,
                                                                            .min       = 5.5,
                                                                            .max       = 5.7,
                                                                            .step      = 0.1},
                                             operational_domain_value_range{.dimension = sweep_parameter::LAMBDA_TF,
                                                                            .min       = 5.0,
                                                                            .max       = 5.2,
                                                                            .step      = 0.1},
                                             operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.33,
                                                                            .max       = -0.31,
                                                                            .step      = 0.01}};

        cd.assign_charge_state({38, 0, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({2, 1, 0}, charge_state::NEGATIVE);

        cd.assign_charge_state({6, 2, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({32, 2, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({8, 3, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({30, 3, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({12, 4, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({26, 4, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({14, 5, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({24, 5, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({19, 8, 0}, charge_state::NEUTRAL);
        cd.assign_charge_state({18, 9, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({23, 9, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({18, 11, 1}, charge_state::NEUTRAL);

        cd.assign_charge_state({19, 13, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({20, 14, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({24, 15, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({26, 16, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({30, 17, 0}, charge_state::NEGATIVE);
        cd.assign_charge_state({32, 18, 0}, charge_state::NEUTRAL);

        cd.assign_charge_state({36, 19, 0}, charge_state::NEGATIVE);

        const auto valid_parameters = physically_valid_parameters(bestagon_and, cd, op_domain_params);
        REQUIRE(valid_parameters.size() == 27);
        const auto p1 = valid_parameters.contains(parameter_point{{5.6, 5.0, -0.32}});
        REQUIRE(p1.has_value());
        CHECK(std::get<0>(p1.value_or(decltype(p1)::value_type{})) == 0);

        const auto p2 = valid_parameters.contains(parameter_point{{5.6, 5.0, -0.33}});
        REQUIRE(p2.has_value());
        CHECK(std::get<0>(p2.value_or(decltype(p2)::value_type{})) == 0);

        const auto p3 = valid_parameters.contains(parameter_point{{5.6, 5.0, -0.31}});
        REQUIRE(p3.has_value());
        CHECK(std::get<0>(p3.value_or(decltype(p3)::value_type{})) == 1);

        const auto p4 = valid_parameters.contains(parameter_point{{5.7, 5.2, -0.33}});
        REQUIRE(p4.has_value());
        CHECK(std::get<0>(p4.value_or(decltype(p4)::value_type{})) == 0);
    }
}
