//
// Created by marcel on 26.02.26.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/clock_emulator.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <any>
#include <array>
#include <cstdint>

using namespace fiction;

/**
 * @brief Creates a 5-pair BDL wire layout with 1 perturber, where each pair belongs to one clock phase.
 *
 * @return Pre-configured SiDB layout with 11 cells across 5 clock zones.
 */
static sidb_100_cell_clk_lyt_siqad create_5_pair_bdl_wire_layout()
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

    lyt.assign_cell_type({27, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({27, 0, 0}, 0);
    lyt.assign_cell_type({29, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({29, 0, 0}, 0);

    return lyt;
}

/**
 * @brief Creates the default SiQAD simulation parameters for clock emulation testing.
 *
 * @param engine The simulation engine to use.
 * @return Configured clock emulator parameters.
 */
static clock_emulator_params
create_default_params(const sidb_simulation_engine engine = sidb_simulation_engine::QUICKEXACT)
{
    clock_emulator_params params{};
    params.sim_params.epsilon_r = 5.6;
    params.sim_params.lambda_tf = 5.0;
    params.sim_params.mu_minus  = -0.28;
    params.sim_engine           = engine;
    return params;
}

// All 11 cell positions used in the layout, ordered by x-coordinate
static constexpr std::array<std::pair<int32_t, int32_t>, 11> ALL_CELLS = {
    {{0, 0}, {3, 0}, {5, 0}, {9, 0}, {11, 0}, {15, 0}, {17, 0}, {21, 0}, {23, 0}, {27, 0}, {29, 0}}};

// Expected charge states for each of the 8 clock phases
// Phases 0-3 form one full cycle, phases 4-7 repeat it
static constexpr auto NEG = sidb_charge_state::NEGATIVE;
static constexpr auto NEU = sidb_charge_state::NEUTRAL;
// clang-format off
static constexpr std::array<std::array<sidb_charge_state, 11>, 8> EXPECTED_CHARGES = {{
    //  (0,0) (3,0) (5,0) (9,0) (11,0) (15,0) (17,0) (21,0) (23,0) (27,0) (29,0)
    { NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 0
    { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 1
    { NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU },  // phase 2
    { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU },  // phase 3
    { NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG },  // phase 4
    { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 5
    { NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU },  // phase 6
    { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU },  // phase 7
}};
// clang-format on

/**
 * @brief Validates the emulation result metadata and charge state patterns.
 *
 * @param result The emulation result to validate.
 * @param params The parameters used for the emulation.
 */
template <typename Lyt>
static void validate_emulation_result(const clock_emulator_result<Lyt>& result, const clock_emulator_params& params)
{
    // we should have 8 clock phase results
    CHECK(result.clock_phase_results.size() == 8);

    uint8_t phase = 0;
    for (const auto& phase_result : result.clock_phase_results)
    {
        // each should contain a single charge distribution with 9 cells and no defects
        // the latter is crucial because the emulation process utilizes temporary defects
        // as fixed static charges to emulate Hold phase behavior
        REQUIRE(phase_result.charge_distributions.size() == 1);
        CHECK(phase_result.charge_distributions.front().num_cells() == 11);
        CHECK(phase_result.charge_distributions.front().num_defects() == 0);

        // metadata should be intact
        CHECK(phase_result.simulation_parameters.epsilon_r == params.sim_params.epsilon_r);
        CHECK(phase_result.simulation_parameters.lambda_tf == params.sim_params.lambda_tf);
        CHECK(phase_result.simulation_parameters.mu_minus == params.sim_params.mu_minus);
        CHECK(phase_result.simulation_parameters.base == params.sim_params.base);
        CHECK(phase_result.algorithm_name == sidb_simulation_engine_name(params.sim_engine));
        CHECK(phase_result.simulation_runtime.count() >= 0.0);
        CHECK(std::any_cast<uint8_t>(phase_result.additional_simulation_parameters.at("clock_phase")) == phase++ % 4);
    }

    // check charge states for all 8 phases
    for (std::size_t p = 0; p < 8; ++p)
    {
        const auto& cds = result.clock_phase_results[p].charge_distributions.front();
        for (std::size_t c = 0; c < ALL_CELLS.size(); ++c)
        {
            CHECK(cds.get_charge_state({ALL_CELLS[c].first, ALL_CELLS[c].second, 0}) == EXPECTED_CHARGES[p][c]);
        }
    }
}

TEST_CASE("Clock emulator instantiation test", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    const auto result = emulate_clocks(lyt, 4);

    CHECK(result.clock_phase_results.size() == 4);
}

TEST_CASE("4-pair BDL wire emulation test with QuickExact", "[clock-emulator]")
{
    const auto lyt    = create_5_pair_bdl_wire_layout();
    const auto params = create_default_params(sidb_simulation_engine::QUICKEXACT);

    const auto result = emulate_clocks(lyt, 8, params);

    validate_emulation_result(result, params);

    // pretty print the results for visual inspection
    result.pretty_print();

    // result.animate(300, 10);
}

#if (FICTION_ALGLIB_ENABLED)
TEST_CASE("4-pair BDL wire emulation test with ClusterComplete", "[clock-emulator]")
{
    const auto lyt    = create_5_pair_bdl_wire_layout();
    const auto params = create_default_params(sidb_simulation_engine::CLUSTERCOMPLETE);

    const auto result = emulate_clocks(lyt, 8, params);

    validate_emulation_result(result, params);

    // pretty print the results for visual inspection
    result.pretty_print();
}
#endif  // FICTION_ALGLIB_ENABLED

TEST_CASE("Zero clock phases", "[clock-emulator]")
{
    const auto lyt    = create_5_pair_bdl_wire_layout();
    const auto params = create_default_params();

    const auto result = emulate_clocks(lyt, 0, params);

    CHECK(result.clock_phase_results.empty());
}

TEST_CASE("Empty layout", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    const auto params = create_default_params();
    const auto result = emulate_clocks(lyt, 4, params);

    REQUIRE(result.clock_phase_results.size() == 4);

    for (const auto& phase_result : result.clock_phase_results)
    {
        REQUIRE(phase_result.charge_distributions.size() == 1);
        CHECK(phase_result.charge_distributions.front().num_cells() == 0);
        CHECK(phase_result.charge_distributions.front().num_defects() == 0);
    }
}

TEST_CASE("Single SiDB", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({0, 0, 0}, 0);

    const auto params = create_default_params();
    const auto result = emulate_clocks(lyt, 4, params);

    REQUIRE(result.clock_phase_results.size() == 4);

    // phase 0: the single SiDB should be simulated
    REQUIRE(result.clock_phase_results[0].charge_distributions.size() == 1);
    CHECK(result.clock_phase_results[0].charge_distributions.front().num_cells() == 1);
    CHECK(result.clock_phase_results[0].charge_distributions.front().num_defects() == 0);
    CHECK(result.clock_phase_results[0].charge_distributions.front().get_charge_state({0, 0, 0}) ==
          sidb_charge_state::NEGATIVE);
}

TEST_CASE("All SiDBs in one clock zone", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    // place two SiDBs, both in clock zone 0
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({0, 0, 0}, 0);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_clock_number({5, 0, 0}, 0);

    const auto params = create_default_params();
    const auto result = emulate_clocks(lyt, 4, params);

    REQUIRE(result.clock_phase_results.size() == 4);

    for (const auto& phase_result : result.clock_phase_results)
    {
        REQUIRE(phase_result.charge_distributions.size() == 1);
        CHECK(phase_result.charge_distributions.front().num_cells() == 2);
        CHECK(phase_result.charge_distributions.front().num_defects() == 0);
    }
}

TEST_CASE("Unsupported simulation engine throws", "[clock-emulator]")
{
    const auto lyt = create_5_pair_bdl_wire_layout();

    clock_emulator_params params{};
    params.sim_engine = sidb_simulation_engine::EXGS;

    CHECK_THROWS_AS(emulate_clocks(lyt, 1, params), std::invalid_argument);
}
