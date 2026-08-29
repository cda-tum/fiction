//
// Created by Jan Drewniok on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/fcn/cell_technologies.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/critical_temperature.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <stdexcept>
#include <vector>

using namespace fiction;

TEMPLATE_TEST_CASE("Test critical_temperature function", "[critical-temperature]", sidb_100_cell_clk_lyt_siqad,
                   cds_sidb_100_cell_clk_lyt_siqad)
{
    TestType lyt{};

    sidb::simulation::analysis::critical_temperature_params params{};
    sidb::model::simulation_parameters                      sim_params{2, -0.32, 5.6, 5.0};

    sidb::simulation::analysis::critical_temperature_stats critical_stats{};

    SECTION("No physically valid charge distribution could be found")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({6, 1, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({8, 1, 0}, sidb::technology::cell_type::OUTPUT);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKSIM;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 0;
        params.alpha                         = 0.0;

        const auto ct = sidb::simulation::analysis::critical_temperature_gate_based<TestType>(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct == 0.0);
    }

    SECTION("No SiDB")
    {
        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based<TestType>(
            lyt, std::vector{tt{}}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct_qe == 0.0);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based<TestType>(
            lyt, std::vector{tt{}}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct_cc == 0.0);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Not working diagonal wire where positively charged SiDBs can occur")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb::technology::cell_type::NORMAL);

        // canvas SiDB
        lyt.assign_cell_type({14, 6, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 6, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 6, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({30, 17, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 18, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb::technology::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(ct == 0.0);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(ct_cc == 0.0);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("four SiDBs with two valid charge distributions, QuickExact")
    {
        lyt.assign_cell_type({0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 2}, TestType::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe =
            sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 350);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 350);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Y-shaped SiQAD AND gate")
    {
        lyt.assign_cell_type({0, 0, 1}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 1}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({20, 0, 1}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({18, 1, 1}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({4, 2, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 3, 1}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({14, 3, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({16, 2, 1}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({10, 6, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({10, 7, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({10, 9, 1}, sidb::technology::cell_type::NORMAL);

        sim_params.mu_minus = -0.28;

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 350);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 350);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Y-shaped SiQAD OR gate")
    {
        const auto lyt_or_gate = blueprints::siqad_or_gate<TestType>();

        sim_params.mu_minus = -0.28;

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.operational_params.input_bdl_iterator_params.input_bdl_config =
            sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED;
        params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5;

        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_or_gate, std::vector{synthesis::create_or_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 400);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_or_gate, std::vector{synthesis::create_or_tt()}, params, &critical_stats);

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 400);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon AND gate, QuickExact")
    {
        lyt.assign_cell_type({36, 1, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({38, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({23, 9, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 11, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 9, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 8, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({20, 14, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 13, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({32, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({30, 3, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 5, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({32, 18, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({30, 17, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb::technology::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        SECTION("Kinks are allowed")
        {
            const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(26.02, 0.01));
            CHECK_THAT(std::abs(ct_qe - 57.24), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

            const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(26.02, 0.01));
            CHECK_THAT(std::abs(ct_cc - 57.24), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
        SECTION("Kinks are not allowed")
        {
            params.operational_params.op_condition =
                sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 2.5;
            const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(5.1153718076, 0.01));
            CHECK_THAT(std::abs(ct_qe - 12.95), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

            const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                       Catch::Matchers::WithinAbs(5.1153718076, 0.01));
            CHECK_THAT(std::abs(ct_cc - 12.95), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
    }

    SECTION("Bestagon AND gate, QuickSim")
    {
        lyt.assign_cell_type({36, 1, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({38, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({23, 9, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 11, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 9, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 8, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({20, 14, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 13, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({32, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({30, 3, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 5, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({32, 18, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({30, 17, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb::technology::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKSIM;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 500;
        params.alpha                         = 0.6;

        const auto ct_qs = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

        CHECK(ct_qs > 0);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

        CHECK(ct_cc > 0);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon FO2 gate")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({21, 11, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({17, 11, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 13, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 7, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 6, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({12, 16, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 15, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({8, 17, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({6, 18, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({30, 17, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 18, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({2, 19, 0}, sidb::technology::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        SECTION("Kinks are allowed")
        {
            const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_qe - 1.41), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

            const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_cc - 1.41), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
        SECTION("Kinks are not allowed")
        {
            params.operational_params.op_condition =
                sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
            const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_qe - 1.42), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

            const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_fan_out_tt()}, params, &critical_stats);

            CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                       Catch::Matchers::WithinAbs(0.00, 0.01));
            CHECK_THAT(std::abs(ct_cc - 1.42), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
        }
    }

    SECTION("Bestagon CX gate")
    {
        const auto crossing_lyt = blueprints::bestagon_crossing<TestType>();

        params.operational_params.sim_params = sim_params;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            crossing_lyt, std::vector{synthesis::create_crossing_wire_tt()}, params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 0.8199), Catch::Matchers::WithinAbs(0.000000, 0.001));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            crossing_lyt, std::vector{synthesis::create_crossing_wire_tt()}, params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 0.81999), Catch::Matchers::WithinAbs(0.000000, 0.0001));

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("SiQAD OR gate")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({26, 0, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({24, 1, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({20, 2, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 3, 0}, sidb::technology::cell_type::NORMAL);

        // three canvas SiDBs
        lyt.assign_cell_type({12, 6, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 7, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 11, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 13, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({20, 14, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);

        sim_params.mu_minus = -0.25;

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        SECTION("Kinks are allowed")
        {
            const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_or_tt()}, params, &critical_stats);

            CHECK(ct_qe < 350);

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

            const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_or_tt()}, params, &critical_stats);

            CHECK(ct_cc < 350);

#endif  // FICTION_ALGLIB_ENABLED
        }
        SECTION("Kinks are not allowed")
        {
            params.operational_params.op_condition =
                sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
            const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_or_tt()}, params, &critical_stats);

            CHECK(ct_qe < 350);

#if (FICTION_ALGLIB_ENABLED)

            params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

            const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, std::vector{synthesis::create_or_tt()}, params, &critical_stats);

            CHECK(ct_cc < 350);

#endif  // FICTION_ALGLIB_ENABLED
        }
    }

    SECTION("Not working diagonal Wire")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb::technology::cell_type::NORMAL);

        // canvas SiDB
        lyt.assign_cell_type({14, 6, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({30, 17, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 18, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb::technology::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(305.95, 0.01));
        CHECK_THAT(std::abs(ct_qe), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "ClusterComplete");

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(305.95, 0.01));
        CHECK_THAT(std::abs(ct_cc), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("nine SiDBs, QuickSim, non-gate-based")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 0, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 0, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({3, 1, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 1, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 1, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 1, 1}, sidb::technology::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKSIM;
        params.confidence_level              = 0.99;
        params.max_temperature               = 750;
        params.iteration_steps               = 500;
        params.alpha                         = 0.6;

        const auto ct_qe =
            sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickSim");

        CHECK_THAT(std::abs(ct_qe), Catch::Matchers::WithinAbs(11.17, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "ClusterComplete");

        CHECK_THAT(std::abs(ct_cc), Catch::Matchers::WithinAbs(11.17, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEMPLATE_TEST_CASE("Test critical_temperature function, using offset coordinates", "[critical-temperature]",
                   sidb_100_cell_clk_lyt)
{
    TestType lyt{};

    sidb::simulation::analysis::critical_temperature_params params{};
    sidb::model::simulation_parameters                      sim_params{2, -0.32, 5.6, 5.0};

    sidb::simulation::analysis::critical_temperature_stats critical_stats{};

    SECTION("No physically valid charge distribution could be found")
    {
        lyt.assign_cell_type({0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 2}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({6, 2}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({8, 2}, sidb::technology::cell_type::OUTPUT);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKSIM;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 0;
        params.alpha                         = 0.0;

        const auto ct_qs = sidb::simulation::analysis::critical_temperature_gate_based<TestType>(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickSim");

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct_qs == 0.0);
    }

    SECTION("One SiDB")
    {
        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct = sidb::simulation::analysis::critical_temperature_gate_based<TestType>(lyt, std::vector{tt{}},
                                                                                              params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(ct == 0.0);
    }

    SECTION("Not working diagonal Wire where positively charged SiDBs can occur")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 2, 0}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 4, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 6, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 8, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 10, 0}, sidb::technology::cell_type::NORMAL);

        // canvas SiDB
        lyt.assign_cell_type({14, 12, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 13, 1}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 12, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({24, 30, 0}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 32, 0}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({30, 34, 0}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 36, 0}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 38, 0}, sidb::technology::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(ct_qe == 0.0);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_id_tt()}, params, &critical_stats);

        CHECK(ct_cc == 0.0);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("four SiDBs with two valid charge distributions, QuickExact")
    {
        lyt.assign_cell_type({0, 2}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 2}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 2}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 4}, TestType::cell_type::NORMAL);

        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe =
            sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 350);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc =
            sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "ClusterComplete");

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 350);

#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Y-shape SiDB AND gate")
    {
        lyt.assign_cell_type({0, 1}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 3}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({20, 1}, sidb::technology::cell_type::INPUT);
        lyt.assign_cell_type({18, 3}, sidb::technology::cell_type::INPUT);

        lyt.assign_cell_type({4, 5}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 7}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({14, 7}, sidb::technology::cell_type::NORMAL);
        lyt.assign_cell_type({16, 5}, sidb::technology::cell_type::NORMAL);

        lyt.assign_cell_type({10, 12}, sidb::technology::cell_type::OUTPUT);
        lyt.assign_cell_type({10, 14}, sidb::technology::cell_type::OUTPUT);

        lyt.assign_cell_type({10, 19}, sidb::technology::cell_type::NORMAL);

        sim_params.mu_minus                  = -0.28;
        params.operational_params.sim_params = sim_params;
        params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
        params.confidence_level              = 0.99;
        params.max_temperature               = 350;
        params.iteration_steps               = 80;
        params.alpha                         = 0.7;

        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_qe == 350);

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt, std::vector{synthesis::create_and_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "ClusterComplete");

        CHECK(std::isinf(critical_stats.energy_between_ground_state_and_first_erroneous));
        CHECK(ct_cc == 350);

#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEST_CASE("Pre-generated input pattern layouts match the layout-based overload", "[critical-temperature]")
{
    const sidb_100_cell_clk_lyt_siqad lat{blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>()};

    for (const auto condition : {sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS,
                                 sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS})
    {
        sidb::simulation::analysis::critical_temperature_params params{};
        params.operational_params.sim_params   = sidb::model::simulation_parameters{2, -0.32};
        params.operational_params.sim_engine   = sidb::simulation::engine::QUICKEXACT;
        params.operational_params.op_condition = condition;

        sidb::simulation::analysis::critical_temperature_stats expected_stats{};

        const auto expected_ct = sidb::simulation::analysis::critical_temperature_gate_based(
            lat, std::vector<tt>{synthesis::create_and_tt()}, params, &expected_stats);

        const auto input_wires = sidb::simulation::logic::detect_bdl_wires(
            lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
            sidb::simulation::logic::bdl_wire_selection::INPUT);
        const auto output_wires = sidb::simulation::logic::detect_bdl_wires(
            lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
            sidb::simulation::logic::bdl_wire_selection::OUTPUT);
        const auto output_pairs = sidb::simulation::logic::detect_bdl_pairs(
            lat, sidb::technology::cell_type::OUTPUT,
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

        const auto input_pattern_layouts = sidb::simulation::logic::generate_bdl_input_pattern_layouts(
            lat, params.operational_params.input_bdl_iterator_params, input_wires);

        sidb::simulation::analysis::critical_temperature_stats stats{};

        const auto ct = sidb::simulation::analysis::critical_temperature_gate_based(
            input_pattern_layouts, std::vector<tt>{synthesis::create_and_tt()}, params, output_pairs, input_wires,
            output_wires, &stats);

        // the two overloads run the same computation, so the results must be bit-identical
        CHECK_THAT(ct, Catch::Matchers::WithinULP(expected_ct, 0));
        CHECK(stats.num_valid_lyt == expected_stats.num_valid_lyt);
        CHECK(stats.energy_between_ground_state_and_first_erroneous ==
              expected_stats.energy_between_ground_state_and_first_erroneous);
    }
}

TEST_CASE("Pre-generated input pattern layouts reject mismatched BDL data", "[critical-temperature]")
{
    const sidb_100_cell_clk_lyt_siqad lat{blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>()};

    const sidb::simulation::analysis::critical_temperature_params params{};

    const auto input_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
        sidb::simulation::logic::bdl_wire_selection::INPUT);
    const auto output_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
        sidb::simulation::logic::bdl_wire_selection::OUTPUT);
    const auto output_pairs = sidb::simulation::logic::detect_bdl_pairs(
        lat, sidb::technology::cell_type::OUTPUT,
        params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

    const auto input_pattern_layouts = sidb::simulation::logic::generate_bdl_input_pattern_layouts(
        lat, params.operational_params.input_bdl_iterator_params, input_wires);

    SECTION("empty specification")
    {
        CHECK_THROWS_AS(sidb::simulation::analysis::critical_temperature_gate_based(
                            input_pattern_layouts, std::vector<tt>{}, params, output_pairs, input_wires, output_wires),
                        std::invalid_argument);
    }
    SECTION("too few input pattern layouts")
    {
        const std::vector<sidb_100_cell_clk_lyt_siqad> too_few{input_pattern_layouts.front()};

        CHECK_THROWS_AS(
            sidb::simulation::analysis::critical_temperature_gate_based(
                too_few, std::vector<tt>{synthesis::create_and_tt()}, params, output_pairs, input_wires, output_wires),
            std::invalid_argument);
    }
    SECTION("more output BDL pairs than truth tables")
    {
        auto too_many = output_pairs;
        too_many.push_back(output_pairs.front());

        CHECK_THROWS_AS(sidb::simulation::analysis::critical_temperature_gate_based(
                            input_pattern_layouts, std::vector<tt>{synthesis::create_and_tt()}, params, too_many,
                            input_wires, output_wires),
                        std::invalid_argument);
    }
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEMPLATE_TEST_CASE("Critical temperature of Bestagon CX, QuickExact", "[critical-temperature], [quality]",
                   sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    TestType lyt{};

    lyt.assign_cell_type({36, 1, 0}, sidb::technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);

    lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
    lyt.assign_cell_type({38, 0, 0}, sidb::technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 12, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 11, 1}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 4, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 9, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 16, 0}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 9, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 13, 1}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 5, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 13, 1}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 2, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 8, 0}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb::technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb::technology::cell_type::OUTPUT);

    lyt.assign_cell_type({32, 18, 0}, sidb::technology::cell_type::OUTPUT);
    lyt.assign_cell_type({8, 17, 0}, sidb::technology::cell_type::OUTPUT);

    lyt.assign_cell_type({2, 19, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({36, 19, 0}, sidb::technology::cell_type::NORMAL);

    sidb::simulation::analysis::critical_temperature_params params{};
    const sidb::model::simulation_parameters                sim_params{2, -0.32, 5.6, 5.0};

    sidb::simulation::analysis::critical_temperature_stats critical_stats{};

    params.operational_params.sim_params = sim_params;
    params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
    params.confidence_level              = 0.99;
    params.max_temperature               = 350;

    const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
        lyt, synthesis::create_crossing_wire_tt(), params, &critical_stats);

    CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
               Catch::Matchers::WithinAbs(0.00, 0.01));

    CHECK_THAT(std::abs(ct_qe - 0.82), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

    params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

    const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
        lyt, synthesis::create_crossing_wire_tt(), params, &critical_stats);

    CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
               Catch::Matchers::WithinAbs(0.00, 0.01));
    CHECK_THAT(std::abs(ct_cc - 0.82), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
}

TEMPLATE_TEST_CASE("Critical temperature of Bestagon double wire, QuickExact", "[critical-temperature], [quality]",
                   sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    const auto lyt_double_wire_gate = blueprints::bestagon_double_wire<TestType>();

    sidb::simulation::analysis::critical_temperature_params params{};
    const sidb::model::simulation_parameters                sim_params{2, -0.32, 5.6, 5.0};

    sidb::simulation::analysis::critical_temperature_stats critical_stats{};

    params.operational_params.sim_params = sim_params;
    params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
    params.confidence_level              = 0.99;
    params.max_temperature               = 350;

    SECTION("Kinks are allowed")
    {
        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_double_wire_gate, synthesis::create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 23.38), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_double_wire_gate, synthesis::create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 23.38), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
    SECTION("Kinks are not allowed")
    {
        params.operational_params.op_condition =
            sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_double_wire_gate, synthesis::create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 26.18), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_double_wire_gate, synthesis::create_double_wire_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 10.717),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 26.18), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEMPLATE_TEST_CASE("Critical temperature of Bestagon half adder gate, QuickExact", "[critical-temperature], [quality]",
                   sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    const auto lyt_half_adder_gate = blueprints::bestagon_ha<TestType>();

    sidb::simulation::analysis::critical_temperature_params params{};
    const sidb::model::simulation_parameters                sim_params{2, -0.32, 5.6, 5.0};

    sidb::simulation::analysis::critical_temperature_stats critical_stats{};

    params.operational_params.sim_params = sim_params;
    params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;
    params.confidence_level              = 0.99;
    params.max_temperature               = 350;

    SECTION("Kinks are allowed")
    {
        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_half_adder_gate, synthesis::create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_half_adder_gate, synthesis::create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
    SECTION("Kinks are not allowed")
    {
        params.operational_params.op_condition =
            sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
        const auto ct_qe = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_half_adder_gate, synthesis::create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_qe - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#if (FICTION_ALGLIB_ENABLED)

        params.operational_params.sim_engine = sidb::simulation::engine::CLUSTERCOMPLETE;

        const auto ct_cc = sidb::simulation::analysis::critical_temperature_gate_based(
            lyt_half_adder_gate, synthesis::create_half_adder_tt(), params, &critical_stats);

        CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.15),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(ct_cc - 0.39), Catch::Matchers::WithinAbs(0.00, 0.01));

#endif  // FICTION_ALGLIB_ENABLED
    }
}
#endif
