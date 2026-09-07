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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/critical_temperature.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/critical_temperature.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

TEST_CASE("Test critical_temperature function", "[critical-temperature]")
{
    layout lyt{};

    critical_temperature_params params{};
    simulation_parameters       sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    SECTION("No physically valid charge distribution could be found")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
        lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);
        lyt.assign_sidb({6, 1, 0}, dot_tag::OUTPUT);
        lyt.assign_sidb({8, 1, 0}, dot_tag::OUTPUT);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKSIM;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 0;
        params.alpha                         = 0.0;

        const auto ct = critical_temperature_gate_based(lyt, std::vector{create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct == 0.0);
    }

    SECTION("No SiDB")
    {
        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = critical_temperature_gate_based(lyt, std::vector{tt{}}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct_qe == 0.0);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_gate_based(lyt, std::vector{tt{}}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct_cc == 0.0);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Not working diagonal wire where positively charged SiDBs can occur")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
        lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

        lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 4, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);

        // canvas SiDB
        lyt.assign_sidb({14, 6, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({14, 6, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({15, 6, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({24, 15, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({26, 16, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({30, 17, 0}, dot_tag::OUTPUT);
        lyt.assign_sidb({32, 18, 0}, dot_tag::OUTPUT);

        lyt.assign_sidb({36, 19, 0}, dot_tag::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct = critical_temperature_gate_based(lyt, std::vector{create_id_tt()}, params, &critical_stats);

        CHECK(ct == 0.0);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_gate_based(lyt, std::vector{create_id_tt()}, params, &critical_stats);

        CHECK(ct_cc == 0.0);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("four SiDBs with two valid charge distributions, QuickExact")
    {
        lyt.assign_sidb({0, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({2, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({4, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({2, 2}, dot_tag::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 350);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 350);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Y-shaped SiQAD AND gate")
    {
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

        sim_params.mu_minus = -0.28;

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 350);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 350);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Y-shaped SiQAD OR gate")
    {
        const auto lyt_or_gate = blueprints::siqad_or_gate();

        sim_params.mu_minus = -0.28;

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.operational_params.input_bdl_iterator_params.input_bdl_config =
            bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED;
        params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5;

        const auto ct_qe =
            critical_temperature_gate_based(lyt_or_gate, std::vector{create_or_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 400);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            critical_temperature_gate_based(lyt_or_gate, std::vector{create_or_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 400);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon AND gate, QuickExact")
    {
        lyt.assign_sidb({36, 1, 0}, dot_tag::INPUT);
        lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

        lyt.assign_sidb({38, 0, 0}, dot_tag::INPUT);
        lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);

        lyt.assign_sidb({23, 9, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({18, 11, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({18, 9, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({19, 8, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({20, 14, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({19, 13, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({26, 16, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({24, 15, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({32, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({30, 3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({26, 4, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({24, 5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 4, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({32, 18, 0}, dot_tag::OUTPUT);
        lyt.assign_sidb({30, 17, 0}, dot_tag::OUTPUT);

        lyt.assign_sidb({36, 19, 0}, dot_tag::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        SECTION("Kinks are allowed")
        {
            const auto ct_qe =
                critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(26.02, 0.01));
            CHECK_THAT(std::abs(ct_qe - 57.24), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

            const auto ct_cc =
                critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(26.02, 0.01));
            CHECK_THAT(std::abs(ct_cc - 57.24), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
        SECTION("Kinks are not allowed")
        {
            params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 2.5;
            const auto ct_qe =
                critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(5.1153718076, 0.01));
            CHECK_THAT(std::abs(ct_qe - 12.95), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

            const auto ct_cc =
                critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(5.1153718076, 0.01));
            CHECK_THAT(std::abs(ct_cc - 12.95), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
    }

    SECTION("Bestagon AND gate, QuickSim")
    {
        lyt.assign_sidb({36, 1, 0}, dot_tag::INPUT);
        lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

        lyt.assign_sidb({38, 0, 0}, dot_tag::INPUT);
        lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);

        lyt.assign_sidb({23, 9, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({18, 11, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({18, 9, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({19, 8, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({20, 14, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({19, 13, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({26, 16, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({24, 15, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({32, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({30, 3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({26, 4, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({24, 5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 4, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({32, 18, 0}, dot_tag::OUTPUT);
        lyt.assign_sidb({30, 17, 0}, dot_tag::OUTPUT);

        lyt.assign_sidb({36, 19, 0}, dot_tag::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKSIM;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 500;
        params.alpha                         = 0.6;

        const auto ct_qs = critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

        CHECK(ct_qs > 0);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_gate_based(lyt, std::vector{create_and_tt()}, params, &critical_stats);

        CHECK(ct_cc > 0);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon FO2 gate")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
        lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

        lyt.assign_sidb({21, 11, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({17, 11, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 4, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({18, 13, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({19, 7, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({18, 6, 0}, dot_tag::NORMAL);
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

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        SECTION("Kinks are allowed")
        {
            const auto ct_qe =
                critical_temperature_gate_based(lyt, std::vector{create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_qe - 1.41), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

            const auto ct_cc =
                critical_temperature_gate_based(lyt, std::vector{create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_cc - 1.41), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
        SECTION("Kinks are not allowed")
        {
            params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
            const auto ct_qe =
                critical_temperature_gate_based(lyt, std::vector{create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_qe - 1.42), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

            const auto ct_cc =
                critical_temperature_gate_based(lyt, std::vector{create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_cc - 1.42), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
    }

    SECTION("Bestagon CX gate")
    {
        const auto crossing_lyt = blueprints::bestagon_crossing();

        params.operational_params.sim_params = sim_params;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = critical_temperature_gate_based(crossing_lyt, std::vector{create_crossing_wire_tt()}, params,
                                                           &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 0.8199), Catch::Matchers::WithinAbs(0.000000, 0.001));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_gate_based(crossing_lyt, std::vector{create_crossing_wire_tt()}, params,
                                                           &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 0.81999), Catch::Matchers::WithinAbs(0.000000, 0.0001));

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("SiQAD OR gate")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
        lyt.assign_sidb({26, 0, 0}, dot_tag::INPUT);

        lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);
        lyt.assign_sidb({24, 1, 0}, dot_tag::INPUT);

        lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({20, 2, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({18, 3, 0}, dot_tag::NORMAL);

        // three canvas SiDBs
        lyt.assign_sidb({12, 6, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 7, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({15, 11, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({18, 13, 0}, dot_tag::OUTPUT);
        lyt.assign_sidb({20, 14, 0}, dot_tag::OUTPUT);

        lyt.assign_sidb({24, 15, 0}, dot_tag::NORMAL);

        sim_params.mu_minus = -0.25;

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        SECTION("Kinks are allowed")
        {
            const auto ct_qe =
                critical_temperature_gate_based(lyt, std::vector{create_or_tt()}, params, &critical_stats);

            CHECK(ct_qe < 350);

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

            const auto ct_cc =
                critical_temperature_gate_based(lyt, std::vector{create_or_tt()}, params, &critical_stats);

            CHECK(ct_cc < 350);

#endif  // FICTION_ALGLIB_ENABLED
        }
        SECTION("Kinks are not allowed")
        {
            params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
            const auto ct_qe =
                critical_temperature_gate_based(lyt, std::vector{create_or_tt()}, params, &critical_stats);

            CHECK(ct_qe < 350);

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

            const auto ct_cc =
                critical_temperature_gate_based(lyt, std::vector{create_or_tt()}, params, &critical_stats);

            CHECK(ct_cc < 350);

#endif  // FICTION_ALGLIB_ENABLED
        }
    }

    SECTION("Not working diagonal Wire")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
        lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);

        lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 4, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);

        // canvas SiDB
        lyt.assign_sidb({14, 6, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({24, 15, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({26, 16, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({30, 17, 0}, dot_tag::OUTPUT);
        lyt.assign_sidb({32, 18, 0}, dot_tag::OUTPUT);

        lyt.assign_sidb({36, 19, 0}, dot_tag::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = critical_temperature_gate_based(lyt, std::vector{create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(305.95, 0.01));
        CHECK_THAT(std::abs(ct_qe), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_gate_based(lyt, std::vector{create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "ClusterComplete");

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(305.95, 0.01));
        CHECK_THAT(std::abs(ct_cc), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("nine SiDBs, QuickSim, non-gate-based")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({3, 0, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({9, 0, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 0, 0}, dot_tag::NORMAL);

        lyt.assign_sidb({3, 1, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({6, 1, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({9, 1, 1}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 1, 1}, dot_tag::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = engine::QUICKSIM;
        params.confidence_level              = 0.99;
        params.max_temperature               = 750;
        params.iteration_steps               = 500;
        params.alpha                         = 0.6;

        const auto ct_qe = critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickSim");

        CHECK_THAT(std::abs(ct_qe), Catch::Matchers::WithinAbs(11.17, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc = critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "ClusterComplete");

        CHECK_THAT(std::abs(ct_cc), Catch::Matchers::WithinAbs(11.17, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEST_CASE("Pre-generated input pattern layouts match the layout-based overload", "[critical-temperature]")
{
    const layout lat{blueprints::siqad_and_gate()};

    for (const auto input_config : {bdl_input_iterator_params::input_bdl_configuration::PERTURBER_DISTANCE_ENCODED,
                                    bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED})
    {
        for (const auto condition : {is_operational_params::operational_condition::TOLERATE_KINKS,
                                     is_operational_params::operational_condition::REJECT_KINKS})
        {
            critical_temperature_params params{};
            params.operational_params.sim_params                                 = simulation_parameters{2, -0.32};
            params.operational_params.sim_engine                                 = engine::QUICKEXACT;
            params.operational_params.op_condition                               = condition;
            params.operational_params.input_bdl_iterator_params.input_bdl_config = input_config;

            critical_temperature_stats expected_stats{};

            const auto expected_ct =
                critical_temperature_gate_based(lat, std::vector<tt>{create_and_tt()}, params, &expected_stats);

            const auto input_wires = detect_bdl_wires(
                lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::INPUT);
            const auto output_wires = detect_bdl_wires(
                lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::OUTPUT);
            const auto output_pairs =
                detect_bdl_pairs(lat, dot_tag::OUTPUT,
                                 params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

            const auto input_pattern_layouts = generate_bdl_input_pattern_layouts(
                lat, params.operational_params.input_bdl_iterator_params, input_wires);

            critical_temperature_stats stats{};

            const auto ct = critical_temperature_gate_based(input_pattern_layouts, std::vector<tt>{create_and_tt()},
                                                            params, output_pairs, input_wires, output_wires, &stats);

            // the two overloads run the same computation, so the results must be bit-identical
            CHECK_THAT(ct, Catch::Matchers::WithinULP(expected_ct, 0));
            CHECK(stats.num_valid_lyt == expected_stats.num_valid_lyt);
            CHECK(stats.energy_between_ground_state_and_first_erroneous ==
                  expected_stats.energy_between_ground_state_and_first_erroneous);
        }
    }
}

TEST_CASE("Pre-generated input pattern layouts reject mismatched BDL data", "[critical-temperature]")
{
    const layout lat{blueprints::siqad_and_gate()};

    const critical_temperature_params params{};

    const auto input_wires  = detect_bdl_wires(lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                               bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                               bdl_wire_selection::OUTPUT);
    const auto output_pairs = detect_bdl_pairs(
        lat, dot_tag::OUTPUT, params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

    const auto input_pattern_layouts =
        generate_bdl_input_pattern_layouts(lat, params.operational_params.input_bdl_iterator_params, input_wires);

    SECTION("empty specification")
    {
        CHECK_THROWS_AS(critical_temperature_gate_based(input_pattern_layouts, std::vector<tt>{}, params, output_pairs,
                                                        input_wires, output_wires),
                        std::invalid_argument);
    }
    SECTION("too few input pattern layouts")
    {
        const std::vector<layout> too_few{input_pattern_layouts.front()};

        CHECK_THROWS_AS(critical_temperature_gate_based(too_few, std::vector<tt>{create_and_tt()}, params, output_pairs,
                                                        input_wires, output_wires),
                        std::invalid_argument);
    }
    SECTION("more output BDL pairs than truth tables")
    {
        auto too_many = output_pairs;
        too_many.push_back(output_pairs.front());

        CHECK_THROWS_AS(critical_temperature_gate_based(input_pattern_layouts, std::vector<tt>{create_and_tt()}, params,
                                                        too_many, input_wires, output_wires),
                        std::invalid_argument);
    }
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("Critical temperature of Bestagon CX, QuickExact", "[critical-temperature], [quality]")
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

    critical_temperature_params params{};
    const simulation_parameters sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    params.operational_params.sim_params = sim_params;
    params.operational_params.sim_engine = engine::QUICKEXACT;
    params.confidence_level              = 0.99;
    params.max_temperature               = 350;

    const auto ct_qe = critical_temperature_gate_based(lyt, create_crossing_wire_tt(), params, &critical_stats);

    CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
               Catch::Matchers::WithinAbs(0.00, 0.01));

    CHECK_THAT(std::abs(ct_qe - 0.82), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

    params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

    const auto ct_cc = critical_temperature_gate_based(lyt, create_crossing_wire_tt(), params, &critical_stats);

    CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
               Catch::Matchers::WithinAbs(0.00, 0.01));
    CHECK_THAT(std::abs(ct_cc - 0.82), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
}

TEST_CASE("Critical temperature of Bestagon double wire, QuickExact", "[critical-temperature], [quality]")
{
    const auto lyt_double_wire_gate = blueprints::bestagon_double_wire();

    critical_temperature_params params{};
    const simulation_parameters sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    params.operational_params.sim_params = sim_params;
    params.operational_params.sim_engine = engine::QUICKEXACT;
    params.confidence_level              = 0.99;
    params.max_temperature               = 350;

    SECTION("Kinks are allowed")
    {
        const auto ct_qe =
            critical_temperature_gate_based(lyt_double_wire_gate, create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 23.38), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            critical_temperature_gate_based(lyt_double_wire_gate, create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 23.38), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
    SECTION("Kinks are not allowed")
    {
        params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
        const auto ct_qe =
            critical_temperature_gate_based(lyt_double_wire_gate, create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 26.18), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            critical_temperature_gate_based(lyt_double_wire_gate, create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 26.18), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEST_CASE("Critical temperature of Bestagon half adder gate, QuickExact", "[critical-temperature], [quality]")
{
    const auto lyt_half_adder_gate = blueprints::bestagon_ha();

    critical_temperature_params params{};
    const simulation_parameters sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    params.operational_params.sim_params = sim_params;
    params.operational_params.sim_engine = engine::QUICKEXACT;
    params.confidence_level              = 0.99;
    params.max_temperature               = 350;

    SECTION("Kinks are allowed")
    {
        const auto ct_qe =
            critical_temperature_gate_based(lyt_half_adder_gate, create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            critical_temperature_gate_based(lyt_half_adder_gate, create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
    SECTION("Kinks are not allowed")
    {
        params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
        const auto ct_qe =
            critical_temperature_gate_based(lyt_half_adder_gate, create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            critical_temperature_gate_based(lyt_half_adder_gate, create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
}
#endif

TEST_CASE("Critical temperature reports progress", "[critical-temperature]")
{
    progress_recorder rec{};

    critical_temperature_params params{};
    params.operational_params.sim_params = simulation_parameters{2, -0.32};
    params.operational_params.sim_engine = engine::QUICKEXACT;
    params.on_progress                   = rec.callback();

    SECTION("gate-based")
    {
        const layout lat{blueprints::siqad_and_gate()};

        critical_temperature_stats stats{};

        const auto ct = critical_temperature_gate_based(lat, std::vector<tt>{create_and_tt()}, params, &stats);

        CHECK(ct > 0.0);

        // one step per input pattern of the two-input gate
        CHECK(rec.is_consistent("input patterns"));
        CHECK(rec.final_count("input patterns") == 4);
    }

    SECTION("non-gate-based")
    {
        layout lyt{};
        lyt.assign_dot_tag({0, 0, 0}, dot_tag::NORMAL);
        lyt.assign_dot_tag({4, 0, 0}, dot_tag::NORMAL);
        lyt.assign_dot_tag({6, 0, 0}, dot_tag::NORMAL);

        critical_temperature_stats stats{};

        const auto ct = critical_temperature_non_gate_based(lyt, params, &stats);

        CHECK(ct > 0.0);

        // the physical simulation engine reports through the callback
        CHECK(rec.is_consistent("charge configurations"));
    }
}
