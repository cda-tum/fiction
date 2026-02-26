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

    clock_emulator_params params{};
    params.sim_params.mu_minus = -0.28;
    params.sim_engine          = sidb_simulation_engine::QUICKEXACT;
    const auto result          = emulate_clocks(lyt, 4, params);

    CHECK(result.clock_phase_results.size() == 4);

    result.pretty_print();
}
