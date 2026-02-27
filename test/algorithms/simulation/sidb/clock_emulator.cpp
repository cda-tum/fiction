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
 * @brief Creates a 5-pair BDL wire layout with 1 perturber. No clock zones are assigned.
 *
 * @return Layout with 11 SiDBs, all in clock zone 0 by default.
 */
static sidb_100_cell_clk_lyt_siqad create_bdl_wire_layout()
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({9, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({11, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({15, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({17, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({21, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({23, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({27, 0, 0}, sidb_technology::NORMAL);
    lyt.assign_cell_type({29, 0, 0}, sidb_technology::NORMAL);

    return lyt;
}

/// Assigns clock zones with 1 pair per clock zone (zones 0, 1, 2, 3, 0).
static void assign_1_pair_per_clock(sidb_100_cell_clk_lyt_siqad& lyt)
{
    lyt.assign_clock_number({0, 0, 0}, 0);
    lyt.assign_clock_number({3, 0, 0}, 0);
    lyt.assign_clock_number({5, 0, 0}, 0);
    lyt.assign_clock_number({9, 0, 0}, 1);
    lyt.assign_clock_number({11, 0, 0}, 1);
    lyt.assign_clock_number({15, 0, 0}, 2);
    lyt.assign_clock_number({17, 0, 0}, 2);
    lyt.assign_clock_number({21, 0, 0}, 3);
    lyt.assign_clock_number({23, 0, 0}, 3);
    lyt.assign_clock_number({27, 0, 0}, 0);
    lyt.assign_clock_number({29, 0, 0}, 0);
}

/// Assigns clock zones with 2 pairs per clock zone (zones 0, 0, 1, 1, 2).
static void assign_2_pairs_per_clock(sidb_100_cell_clk_lyt_siqad& lyt)
{
    lyt.assign_clock_number({0, 0, 0}, 0);
    lyt.assign_clock_number({3, 0, 0}, 0);
    lyt.assign_clock_number({5, 0, 0}, 0);
    lyt.assign_clock_number({9, 0, 0}, 0);
    lyt.assign_clock_number({11, 0, 0}, 0);
    lyt.assign_clock_number({15, 0, 0}, 1);
    lyt.assign_clock_number({17, 0, 0}, 1);
    lyt.assign_clock_number({21, 0, 0}, 1);
    lyt.assign_clock_number({23, 0, 0}, 1);
    lyt.assign_clock_number({27, 0, 0}, 2);
    lyt.assign_clock_number({29, 0, 0}, 2);
}

/// Assigns clock zones with 3 pairs per clock zone (zones 0, 0, 0, 1, 1).
static void assign_3_pairs_per_clock(sidb_100_cell_clk_lyt_siqad& lyt)
{
    lyt.assign_clock_number({0, 0, 0}, 0);
    lyt.assign_clock_number({3, 0, 0}, 0);
    lyt.assign_clock_number({5, 0, 0}, 0);
    lyt.assign_clock_number({9, 0, 0}, 0);
    lyt.assign_clock_number({11, 0, 0}, 0);
    lyt.assign_clock_number({15, 0, 0}, 0);
    lyt.assign_clock_number({17, 0, 0}, 0);
    lyt.assign_clock_number({21, 0, 0}, 1);
    lyt.assign_clock_number({23, 0, 0}, 1);
    lyt.assign_clock_number({27, 0, 0}, 1);
    lyt.assign_clock_number({29, 0, 0}, 1);
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

// All 11 cell positions used in the BDL wire layout, ordered by x-coordinate
static constexpr std::array<std::pair<int32_t, int32_t>, 11> ALL_CELLS = {
    {{0, 0}, {3, 0}, {5, 0}, {9, 0}, {11, 0}, {15, 0}, {17, 0}, {21, 0}, {23, 0}, {27, 0}, {29, 0}}};

// Charge state aliases for readability
static constexpr auto NEG = sidb_charge_state::NEGATIVE;
static constexpr auto NEU = sidb_charge_state::NEUTRAL;

/**
 * @brief Validates the emulation result metadata and charge state patterns.
 *
 * @tparam Lyt Layout type.
 * @tparam NumCells Number of cells in the layout.
 * @tparam NumPhases Number of simulated clock phases.
 * @param result The emulation result to validate.
 * @param params The parameters used for the emulation.
 * @param cells The cell positions to check charge states at.
 * @param expected_charges The expected charge state matrix (phases × cells).
 */
template <typename Lyt, std::size_t NumCells, std::size_t NumPhases>
static void
validate_emulation_result(const clock_emulator_result<Lyt>& result, const clock_emulator_params& params,
                          const std::array<std::pair<int32_t, int32_t>, NumCells>&              cells,
                          const std::array<std::array<sidb_charge_state, NumCells>, NumPhases>& expected_charges)
{
    CHECK(result.clock_phase_results.size() == NumPhases);

    uint8_t phase = 0;
    for (const auto& phase_result : result.clock_phase_results)
    {
        // each should contain a single charge distribution with the expected number of cells and no defects;
        // the latter is crucial because the emulation process utilizes temporary defects
        // as fixed static charges to emulate Hold phase behavior
        REQUIRE(phase_result.charge_distributions.size() == 1);
        CHECK(phase_result.charge_distributions.front().num_cells() == NumCells);
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

    // check charge states for all phases
    for (std::size_t p = 0; p < NumPhases; ++p)
    {
        const auto& cds = result.clock_phase_results[p].charge_distributions.front();
        for (std::size_t c = 0; c < NumCells; ++c)
        {
            CHECK(cds.get_charge_state({cells[c].first, cells[c].second, 0}) == expected_charges[p][c]);
        }
    }
}

TEST_CASE("Clock emulator instantiation test", "[clock-emulator]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    const auto result = emulate_clocks(lyt, 4);

    CHECK(result.clock_phase_results.size() == 4);
}

TEST_CASE("BDL wire emulation", "[clock-emulator]")
{
    SECTION("1 pair per clock")
    {
        auto lyt = create_bdl_wire_layout();
        assign_1_pair_per_clock(lyt);

        // Zone 0: (0,3,5,27,29), Zone 1: (9,11), Zone 2: (15,17), Zone 3: (21,23)
        // clang-format off
        static constexpr std::array<std::array<sidb_charge_state, 11>, 8> expected = {{
            //  (0,0) (3,0) (5,0) (9,0) (11,0) (15,0) (17,0) (21,0) (23,0) (27,0) (29,0)
            { NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 0: zone 0 Switch
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 1: zone 1 Switch, zone 0 Hold
            { NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU },  // phase 2: zone 2 Switch, zone 1 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU },  // phase 3: zone 3 Switch, zone 2 Hold
            { NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG },  // phase 4: zone 0 Switch, zone 3 Hold
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 5: zone 1 Switch, zone 0 Hold
            { NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU },  // phase 6: zone 2 Switch, zone 1 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU },  // phase 7: zone 3 Switch, zone 2 Hold
        }};
        // clang-format on

        SECTION("QuickExact")
        {
            const auto params = create_default_params(sidb_simulation_engine::QUICKEXACT);
            const auto result = emulate_clocks(lyt, 8, params);

            validate_emulation_result(result, params, ALL_CELLS, expected);
            result.pretty_print();
        }
#if (FICTION_ALGLIB_ENABLED)
        SECTION("ClusterComplete")
        {
            const auto params = create_default_params(sidb_simulation_engine::CLUSTERCOMPLETE);
            const auto result = emulate_clocks(lyt, 8, params);

            validate_emulation_result(result, params, ALL_CELLS, expected);
            result.pretty_print();
        }
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("2 pairs per clock")
    {
        auto lyt = create_bdl_wire_layout();
        assign_2_pairs_per_clock(lyt);

        // Zone 0: (0,3,5,9,11), Zone 1: (15,17,21,23), Zone 2: (27,29), Zone 3: empty
        // Zone 3 is empty, so no carry from zone 2 back to zone 0; phases 4-7 repeat 0-3
        // clang-format off
        static constexpr std::array<std::array<sidb_charge_state, 11>, 8> expected = {{
            //  (0,0) (3,0) (5,0) (9,0) (11,0) (15,0) (17,0) (21,0) (23,0) (27,0) (29,0)
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU },  // phase 0: zone 0 Switch
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU },  // phase 1: zone 1 Switch, zone 0 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG },  // phase 2: zone 2 Switch, zone 1 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 3: zone 3 empty, zone 2 Hold
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU },  // phase 4: zone 0 Switch (no carry)
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU },  // phase 5: zone 1 Switch, zone 0 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG },  // phase 6: zone 2 Switch, zone 1 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG },  // phase 7: zone 3 empty, zone 2 Hold
        }};
        // clang-format on

        SECTION("QuickExact")
        {
            const auto params = create_default_params(sidb_simulation_engine::QUICKEXACT);
            const auto result = emulate_clocks(lyt, 8, params);

            validate_emulation_result(result, params, ALL_CELLS, expected);
            result.pretty_print();
        }
#if (FICTION_ALGLIB_ENABLED)
        SECTION("ClusterComplete")
        {
            const auto params = create_default_params(sidb_simulation_engine::CLUSTERCOMPLETE);
            const auto result = emulate_clocks(lyt, 8, params);

            validate_emulation_result(result, params, ALL_CELLS, expected);
            result.pretty_print();
        }
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("3 pairs per clock")
    {
        auto lyt = create_bdl_wire_layout();
        assign_3_pairs_per_clock(lyt);

        // Zone 0: (0,3,5,9,11,15,17), Zone 1: (21,23,27,29), Zone 2+3: empty
        // Zones 2 and 3 are empty, so no carry from zone 1 back to zone 0; phases 4-7 repeat 0-3
        // clang-format off
        static constexpr std::array<std::array<sidb_charge_state, 11>, 8> expected = {{
            //  (0,0) (3,0) (5,0) (9,0) (11,0) (15,0) (17,0) (21,0) (23,0) (27,0) (29,0)
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU },  // phase 0: zone 0 Switch
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG },  // phase 1: zone 1 Switch, zone 0 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG },  // phase 2: zone 2 empty, zone 1 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU },  // phase 3: zone 3 empty (no carry)
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEU,  NEU,  NEU },  // phase 4: zone 0 Switch (no carry)
            { NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG,  NEU,  NEG },  // phase 5: zone 1 Switch, zone 0 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEG,  NEU,  NEG },  // phase 6: zone 2 empty, zone 1 Hold
            { NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU,  NEU },  // phase 7: zone 3 empty, no carry
        }};
        // clang-format on

        SECTION("QuickExact")
        {
            const auto params = create_default_params(sidb_simulation_engine::QUICKEXACT);
            const auto result = emulate_clocks(lyt, 8, params);

            validate_emulation_result(result, params, ALL_CELLS, expected);
            result.pretty_print();
        }
#if (FICTION_ALGLIB_ENABLED)
        SECTION("ClusterComplete")
        {
            const auto params = create_default_params(sidb_simulation_engine::CLUSTERCOMPLETE);
            const auto result = emulate_clocks(lyt, 8, params);

            validate_emulation_result(result, params, ALL_CELLS, expected);
            result.pretty_print();
        }
#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEST_CASE("Zero clock phases", "[clock-emulator]")
{
    auto lyt = create_bdl_wire_layout();
    assign_1_pair_per_clock(lyt);

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
    auto lyt = create_bdl_wire_layout();
    assign_1_pair_per_clock(lyt);

    clock_emulator_params params{};
    params.sim_engine = sidb_simulation_engine::EXGS;

    CHECK_THROWS_AS(emulate_clocks(lyt, 1, params), std::invalid_argument);
}
