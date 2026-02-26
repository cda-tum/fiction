//
// Created by marcel on 26.02.26.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/clock_emulator.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Clock emulator instantiation test", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    const auto result = emulate_clocks(lyt, 4);

    CHECK(result.clock_phase_results.size() == 4);
}

TEST_CASE("4-pair BDL wire emulation test", "[clock-emulator]")
{
    // create a layout with 4 pairs of SiDBs, where each pair belongs to a different clock phase
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

    // and each should contain a single charge distribution with 9 cells and no defects
    // the latter is crucial because the emulation process utilizes temporary defects
    // as fixed static charges to emulate Hold phase behavior
    for (const auto& phase_result : result.clock_phase_results)
    {
        REQUIRE(phase_result.charge_distributions.size() == 1);
        CHECK(phase_result.charge_distributions[0].num_cells() == 9);
        CHECK(phase_result.charge_distributions[0].num_defects() == 0);
    }

    // pretty print the results for visual inspection
    result.pretty_print();
}
