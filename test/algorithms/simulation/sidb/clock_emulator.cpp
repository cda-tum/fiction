//
// Created by marcel on 26.02.26.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/clock_emulator.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <any>

using namespace fiction;

TEST_CASE("Clock emulator instantiation test", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    const auto result = emulate_clocks(lyt, 4);

    CHECK(result.clock_phase_results.size() == 4);
}

TEST_CASE("4-pair BDL wire emulation test", "[clock-emulator]")
{
    // create a layout with 4 pairs of SiDBs and 1 perturber, where each pair belongs to a different clock phase
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({0, 0, 0}, 0);
    lyt.assign_cell_type({3, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({3, 0, 0}, 0);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({5, 0, 0}, 0);

    lyt.assign_cell_type({9, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({9, 0, 0}, 1);
    lyt.assign_cell_type({11, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({11, 0, 0}, 1);

    lyt.assign_cell_type({15, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({15, 0, 0}, 2);
    lyt.assign_cell_type({17, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({17, 0, 0}, 2);

    lyt.assign_cell_type({21, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({21, 0, 0}, 3);
    lyt.assign_cell_type({23, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({23, 0, 0}, 3);

    // set the simulation parameters to default SiQAD values
    clock_emulator_params params{};
    params.sim_params.epsilon_r = 5.6;
    params.sim_params.lambda_tf = 5.0;
    params.sim_params.mu_minus  = -0.28;
    params.sim_engine           = sidb_simulation_engine::QUICKEXACT;

    // run an 8-timestep clock emulation
    const auto result = emulate_clocks(lyt, 8, params);

    // we should have 8 clock phase results
    CHECK(result.clock_phase_results.size() == 8);

    uint8_t phase = 0;
    for (const auto& phase_result : result.clock_phase_results)
    {
        // and each should contain a single charge distribution with 9 cells and no defects
        // the latter is crucial because the emulation process utilizes temporary defects
        // as fixed static charges to emulate Hold phase behavior
        REQUIRE(phase_result.charge_distributions.size() == 1);
        CHECK(phase_result.charge_distributions.front().num_cells() == 9);
        CHECK(phase_result.charge_distributions.front().num_defects() == 0);

        // plus, the metadata should be intact
        CHECK(phase_result.simulation_parameters.epsilon_r == params.sim_params.epsilon_r);
        CHECK(phase_result.simulation_parameters.lambda_tf == params.sim_params.lambda_tf);
        CHECK(phase_result.simulation_parameters.mu_minus == params.sim_params.mu_minus);
        CHECK(phase_result.simulation_parameters.base == params.sim_params.base);
        CHECK(phase_result.algorithm_name == sidb_simulation_engine_name(params.sim_engine));
        CHECK(phase_result.simulation_runtime.count() >= 0.0);
        CHECK(std::any_cast<uint8_t>(phase_result.additional_simulation_parameters.at("clock_phase")) == phase++ % 4);
    }

    // detailed checks for each clock phase result
    const auto& phase_0_cds = result.clock_phase_results[0].charge_distributions.front();
    CHECK(phase_0_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_0_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_0_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_0_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_0_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_0_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_0_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_0_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_0_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEUTRAL);

    const auto& phase_1_cds = result.clock_phase_results[1].charge_distributions.front();
    CHECK(phase_1_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_1_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_1_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_1_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_1_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_1_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_1_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_1_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_1_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEUTRAL);

    const auto& phase_2_cds = result.clock_phase_results[2].charge_distributions.front();
    CHECK(phase_2_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_2_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_2_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_2_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_2_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_2_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_2_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_2_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_2_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEUTRAL);

    const auto& phase_3_cds = result.clock_phase_results[3].charge_distributions.front();
    CHECK(phase_3_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_3_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_3_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_3_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_3_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_3_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_3_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_3_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_3_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEGATIVE);

    const auto& phase_4_cds = result.clock_phase_results[4].charge_distributions.front();
    CHECK(phase_4_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_4_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_4_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_4_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_4_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_4_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_4_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_4_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_4_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEGATIVE);

    const auto& phase_5_cds = result.clock_phase_results[5].charge_distributions.front();
    CHECK(phase_5_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_5_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_5_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_5_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_5_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_5_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_5_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_5_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_5_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEUTRAL);

    const auto& phase_6_cds = result.clock_phase_results[6].charge_distributions.front();
    CHECK(phase_6_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_6_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_6_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_6_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_6_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_6_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_6_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_6_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_6_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEUTRAL);

    const auto& phase_7_cds = result.clock_phase_results[7].charge_distributions.front();
    CHECK(phase_7_cds.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_7_cds.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_7_cds.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_7_cds.get_charge_state({9, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_7_cds.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_7_cds.get_charge_state({15, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_7_cds.get_charge_state({17, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(phase_7_cds.get_charge_state({21, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(phase_7_cds.get_charge_state({23, 0, 0}) == sidb_charge_state::NEGATIVE);

    // pretty print the results for visual inspection
    result.pretty_print();
}
