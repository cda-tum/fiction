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
 * @brief Tests for `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <limits>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;
using namespace fiction::utils::math;

TEST_CASE("BDL wire operational domain computation", "[compute-operational-ratio]")
{

    layout lyt{lattice::si_100_2x1(), "BDL wire"};

    lyt.assign_dot_tag({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_dot_tag({3, 0, 0}, dot_tag::INPUT);

    lyt.assign_dot_tag({6, 0, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({8, 0, 0}, dot_tag::NORMAL);

    lyt.assign_dot_tag({12, 0, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({14, 0, 0}, dot_tag::NORMAL);

    lyt.assign_dot_tag({18, 0, 0}, dot_tag::OUTPUT);
    lyt.assign_dot_tag({20, 0, 0}, dot_tag::OUTPUT);

    // output perturber
    lyt.assign_dot_tag({24, 0, 0}, dot_tag::NORMAL);

    const auto& lat = lyt;

    simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};

    // set x-dimension
    op_domain_params.sweep_dimensions[0].min  = 5.5;
    op_domain_params.sweep_dimensions[0].max  = 5.5;
    op_domain_params.sweep_dimensions[0].step = 0.1;

    // set y-dimension
    op_domain_params.sweep_dimensions[1].min  = 5.0;
    op_domain_params.sweep_dimensions[1].max  = 5.0;
    op_domain_params.sweep_dimensions[1].step = 0.1;

    SECTION("Off-grid seeds map to the next grid point")
    {
        op_domain_params.sweep_dimensions[0].max = 5.6;
        op_domain_params.sweep_dimensions[1].max = 5.1;
        const operational_domain_ratio_params params{op_domain_params};
        const auto                            off_grid =
            operational_domain_ratio(lat, std::vector<tt>{create_id_tt()}, parameter_point{{5.55, 5.05}}, params);
        const auto on_grid =
            operational_domain_ratio(lat, std::vector<tt>{create_id_tt()}, parameter_point{{5.6, 5.1}}, params);
        CHECK_THAT(off_grid, Catch::Matchers::WithinAbs(on_grid, ERROR_MARGIN));
    }

    SECTION("Operational domain with one parameter point")
    {
        const auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                .min       = -0.32,
                                                                .max       = -0.32,
                                                                .step      = 0.01};

        op_domain_params.sweep_dimensions.push_back(z_dimension);

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lat, std::vector<tt>{create_id_tt()},
                                                              parameter_point({5.5, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size (1.0)
        CHECK_THAT(op_domain_ratio - 1.0, Catch::Matchers::WithinAbs(0.0, ERROR_MARGIN));
    }

    SECTION("semi-operational domain")
    {
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 0.5;
        op_domain_params.sweep_dimensions[0].max  = 4.25;
        op_domain_params.sweep_dimensions[0].step = 0.25;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 0.5;
        op_domain_params.sweep_dimensions[1].max  = 4.25;
        op_domain_params.sweep_dimensions[1].step = 0.25;

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lat, std::vector<tt>{create_id_tt()},
                                                              parameter_point({4.25, 4.25}), op_ratio_params);

        CHECK_THAT(op_domain_ratio - (80.0 / 256.0), Catch::Matchers::WithinAbs(0.0, ERROR_MARGIN));
    }
}

TEST_CASE("SiQAD NAND gate", "[compute-operational-ratio]")
{
    const auto lyt = to_sidb_layout(blueprints::siqad_nand_gate<sidb_100_cell_clk_lyt_siqad>());

    simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.operational_params.input_bdl_iterator_params.input_bdl_config =
        bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED;
    op_domain_params.sweep_dimensions = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};
    op_domain_params.operational_params.strategy_to_analyze_operational_status =
        is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION;
    op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
    op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5;

    // set x-dimension
    op_domain_params.sweep_dimensions[0].min  = 2.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.1;

    // set y-dimension
    op_domain_params.sweep_dimensions[1].min  = 2.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.1;

    operational_domain_ratio_params op_ratio_params{op_domain_params};

    // pruning and simulation to determine the operational status of the layout
    const auto op_domain_ratio_pruning_and_simulation =
        operational_domain_ratio(lyt, std::vector<tt>{create_nand_tt()}, parameter_point({5.6, 5.0}), op_ratio_params);

    // only pruning to determine the operational status of the layout
    op_ratio_params.op_domain_params.operational_params.strategy_to_analyze_operational_status =
        is_operational_params::operational_analysis_strategy::FILTER_ONLY;

    const auto op_domain_ratio_only_pruning =
        operational_domain_ratio(lyt, std::vector<tt>{create_nand_tt()}, parameter_point({5.6, 5.0}), op_ratio_params);

    CHECK_THAT(op_domain_ratio_pruning_and_simulation, Catch::Matchers::WithinAbs(0.11918914799573235, ERROR_MARGIN));

    CHECK_THAT(op_domain_ratio_only_pruning, Catch::Matchers::WithinAbs(0.11918914799573235, ERROR_MARGIN));
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("Bestagon AND gate", "[compute-operational-ratio]")
{
    const auto lyt = to_sidb_layout(blueprints::bestagon_and_gate<sidb_100_cell_clk_lyt_siqad>());

    simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};

    // set x-dimension
    op_domain_params.sweep_dimensions[0].min  = 5.0;
    op_domain_params.sweep_dimensions[0].max  = 6.0;
    op_domain_params.sweep_dimensions[0].step = 0.1;

    // set y-dimension
    op_domain_params.sweep_dimensions[1].min  = 5.0;
    op_domain_params.sweep_dimensions[1].max  = 6.0;
    op_domain_params.sweep_dimensions[1].step = 0.1;

    const auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                            .min       = -0.32,
                                                            .max       = -0.32,
                                                            .step      = 0.01};

    SECTION("semi-operational domain")
    {
        op_domain_params.sweep_dimensions.push_back(z_dimension);

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lyt, std::vector<tt>{create_and_tt()},
                                                              parameter_point({5.6, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size
        CHECK_THAT(op_domain_ratio - (23.0 / 121.0), Catch::Matchers::WithinAbs(0.0, ERROR_MARGIN));
    }

    SECTION("semi-operational domain, reject kinks")
    {
        op_domain_params.sweep_dimensions.push_back(z_dimension);
        op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lyt, std::vector<tt>{create_and_tt()},
                                                              parameter_point({5.6, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size
        CHECK_THAT(op_domain_ratio - (23.0 / 121.0), Catch::Matchers::WithinAbs(0.0, ERROR_MARGIN));
    }

    SECTION(
        "semi-operational domain, reject kinks, only pruning is used to determine the operational status of the layout")
    {
        op_domain_params.sweep_dimensions.push_back(z_dimension);
        op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
        op_domain_params.operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lyt, std::vector<tt>{create_and_tt()},
                                                              parameter_point({5.6, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size
        CHECK(op_domain_ratio >= (23.0 / 121.0));
    }
}
#endif

TEST_CASE("Operational-domain ratios reject invalid sweep steps", "[compute-operational-ratio]")
{
    operational_domain_ratio_params params{};
    params.op_domain_params.sweep_dimensions = {{sweep_parameter::EPSILON_R, 5.0, 5.1, 0.0}};
    CHECK_THROWS_AS(operational_domain_ratio(layout{}, std::vector<tt>{create_id_tt()}, parameter_point{{5.0}}, params),
                    std::invalid_argument);
}

TEST_CASE("Operational-domain ratios reject malformed seeds", "[compute-operational-ratio]")
{
    operational_domain_ratio_params params{};
    params.op_domain_params.sweep_dimensions = {{sweep_parameter::EPSILON_R, 5.0, 6.0, 1.0},
                                                {sweep_parameter::LAMBDA_TF, 5.0, 6.0, 1.0}};
    const auto nan                           = std::numeric_limits<double>::quiet_NaN();
    const auto inf                           = std::numeric_limits<double>::infinity();
    for (const auto& seed : std::vector<std::vector<double>>{{},
                                                             {5.0},
                                                             {5.0, 5.0, 5.0},
                                                             {nan, 5.0},
                                                             {5.0, inf},
                                                             {-inf, 5.0},
                                                             {4.9, 5.0},
                                                             {5.0, 6.1}})
    {
        CHECK_THROWS_AS(
            operational_domain_ratio(layout{}, std::vector<tt>{create_id_tt()}, parameter_point{seed}, params),
            std::invalid_argument);
    }
    params.op_domain_params.sweep_dimensions.resize(1);
    CHECK_THROWS_AS(operational_domain_ratio(layout{}, std::vector<tt>{create_id_tt()}, parameter_point{{5.0}}, params),
                    std::invalid_argument);
}
