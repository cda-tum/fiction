//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/is_ground_state.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("check if ground state is found", "[is-ground-state]", sidb_100_cell_clk_lyt_siqad,
                   cds_sidb_100_cell_clk_lyt_siqad)
{
    SECTION("layout with no SiDB placed")
    {
        TestType                          lyt{};
        const charge_distribution_surface charge_layout{lyt};
        const sidb_simulation_parameters  params{2, -0.32};
        const auto simulation_results_exgs = exhaustive_ground_state_simulation<TestType>(charge_layout, params);
        const quicksim_params quicksim_params{params};
        const auto            simulation_results_quicksim = quicksim<TestType>(charge_layout, quicksim_params);

        CHECK(!is_ground_state(simulation_results_exgs, simulation_results_quicksim));
    }

    SECTION("layout with seven SiDBs placed")
    {
        TestType lyt{};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

        const charge_distribution_surface charge_layout{lyt};
        const sidb_simulation_parameters  params{2, -0.32};

        auto simulation_results_exgs = exhaustive_ground_state_simulation<TestType>(charge_layout, params);

        // assign different charge index on purpose to see if the algorithm still works as desired
        for (auto& cds : simulation_results_exgs.charge_distributions)
        {
            cds.assign_charge_index(0, charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);
        }

        for (auto& cds : simulation_results_exgs.charge_distributions)
        {
            CHECK(cds.get_charge_index_and_base().first == 0);
        }

        const quicksim_params quicksim_params{params};
        auto            simulation_results_quicksim = quicksim<TestType>(charge_layout, quicksim_params);

        // assign different charge index on purpose to see if the algorithm still works as desired
        for (auto& cds : simulation_results_quicksim.charge_distributions)
        {
            cds.assign_charge_index(0, charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION);
        }

        for (auto& cds : simulation_results_quicksim.charge_distributions)
        {
            CHECK(cds.get_charge_index_and_base().first == 0);
        }

        CHECK(is_ground_state(simulation_results_exgs, simulation_results_quicksim));
    }
}
