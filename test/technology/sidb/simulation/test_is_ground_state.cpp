//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/is_ground_state.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/technology/sidb/surfaces/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("check if ground state is found", "[is-ground-state]", sidb_100_cell_clk_lyt_siqad)
{
    SECTION("degenerate ground state")
    {
        TestType lyt{};
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

        sidb::surfaces::charge_distribution_surface charge_layout_first{lyt};
        sidb::surfaces::charge_distribution_surface charge_layout_second{lyt};

        charge_layout_first.assign_charge_state({1, 3, 0}, sidb::model::charge_state::NEGATIVE);
        charge_layout_first.assign_charge_state({3, 3, 0}, sidb::model::charge_state::NEUTRAL);

        // assign different charge index on purpose to see if the algorithm still works as desired
        charge_layout_first.assign_charge_index(3, sidb::surfaces::charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);

        sidb::simulation::result<TestType> simulation_result_exhaustive{};
        simulation_result_exhaustive.charge_distributions.push_back(charge_layout_first);

        charge_layout_second.assign_charge_state({1, 3, 0}, sidb::model::charge_state::NEUTRAL);
        charge_layout_second.assign_charge_state({3, 3, 0}, sidb::model::charge_state::NEGATIVE);

        // assign different charge index on purpose to see if the algorithm still works as desired
        charge_layout_second.assign_charge_index(3, sidb::surfaces::charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);

        simulation_result_exhaustive.charge_distributions.push_back(charge_layout_second);

        SECTION("heuristic only finds one ground state")
        {
            sidb::simulation::result<TestType> simulation_result_heuristic{};
            simulation_result_heuristic.charge_distributions = {{charge_layout_first}};
            CHECK(!sidb::simulation::is_ground_state(simulation_result_heuristic, simulation_result_exhaustive));
        }

        SECTION("two identical ground states are stored in the simulation results")
        {
            simulation_result_exhaustive.charge_distributions = {{charge_layout_first, charge_layout_first}};
            sidb::simulation::result<TestType> simulation_result_heuristic{};
            simulation_result_heuristic.charge_distributions = {{charge_layout_first}};
            CHECK(sidb::simulation::is_ground_state(simulation_result_heuristic, simulation_result_exhaustive));
        }

        SECTION("heuristic finds both ground states")
        {
            sidb::simulation::result<TestType> simulation_result_heuristic{};
            simulation_result_heuristic.charge_distributions = simulation_result_exhaustive.charge_distributions;
            simulation_result_heuristic.charge_distributions[0].assign_charge_index(
                0, sidb::surfaces::charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);
            simulation_result_heuristic.charge_distributions[1].assign_charge_index(
                1, sidb::surfaces::charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);
            CHECK(sidb::simulation::is_ground_state(simulation_result_heuristic, simulation_result_exhaustive));
        }
    }

    SECTION("layout with no SiDB placed")
    {
        TestType                                          lyt{};
        const sidb::surfaces::charge_distribution_surface charge_layout{lyt};
        constexpr sidb::model::simulation_parameters      params{2, -0.32};
        const auto                                        simulation_results_exgs =
            sidb::simulation::engines::exhaustive_ground_state_simulation<TestType>(charge_layout, params);
        const sidb::simulation::engines::quicksim_params qs_params{params};
        const auto                                       simulation_results_quicksim =
            sidb::simulation::engines::quicksim<TestType>(charge_layout, qs_params);

        REQUIRE(!simulation_results_quicksim.has_value());
    }

    SECTION("Layout with seven SiDBs placed, verify independence from charge index")
    {
        TestType lyt{};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

        const sidb::surfaces::charge_distribution_surface charge_layout{lyt};
        constexpr sidb::model::simulation_parameters      params{2, -0.32};

        sidb::simulation::result<TestType> simulation_results_exgs =
            sidb::simulation::engines::exhaustive_ground_state_simulation<TestType>(charge_layout, params);

        // assign different charge index on purpose to see if the algorithm still works as desired
        for (auto& cds : simulation_results_exgs.charge_distributions)
        {
            cds.assign_charge_index(0, sidb::surfaces::charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);
        }

        for (auto& cds : simulation_results_exgs.charge_distributions)
        {
            CHECK(cds.get_charge_index_and_base().first == 0);
        }

        const sidb::simulation::engines::quicksim_params qs_params{params};
        auto simulation_results_quicksim = sidb::simulation::engines::quicksim<TestType>(charge_layout, qs_params);

        REQUIRE(simulation_results_quicksim.has_value());
        auto& quicksim_res = simulation_results_quicksim.value();  // NOLINT(bugprone-unchecked-optional-access)

        // assign different charge index on purpose to see if the algorithm still works as desired
        for (auto& cds : quicksim_res.charge_distributions)
        {
            cds.assign_charge_index(0, sidb::surfaces::charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);
        }

        for (auto& cds : quicksim_res.charge_distributions)
        {
            CHECK(cds.get_charge_index_and_base().first == 0);
        }

        CHECK(sidb::simulation::is_ground_state(simulation_results_exgs, quicksim_res));
    }
}
