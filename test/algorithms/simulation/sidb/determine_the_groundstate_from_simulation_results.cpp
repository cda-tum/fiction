//
// Created by Jan Drewniok on 12.02.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/determine_the_groundstate_from_simulation_results.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Determine the groundstate from simulation results", "[determine-the-groundstate-from-simulation-results]")
{
    SECTION("Three distinct charge distributions")
    {
        sidb_cell_clk_lyt lyt{};

        lyt.assign_cell_type({5, 4}, sidb_cell_clk_lyt::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, sidb_cell_clk_lyt::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, sidb_cell_clk_lyt::cell_type::NORMAL);

        const charge_distribution_surface cds1{lyt};

        charge_distribution_surface cds2{lyt};
        cds2.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        cds2.update_after_charge_change();

        charge_distribution_surface cds3{lyt};
        cds2.assign_all_charge_states(sidb_charge_state::POSITIVE);
        cds3.update_after_charge_change();

        CHECK_THAT(cds2.get_system_energy(), Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK(cds2.get_system_energy() < cds3.get_system_energy());
        CHECK(cds2.get_system_energy() < cds1.get_system_energy());

        sidb_simulation_result<sidb_cell_clk_lyt> results{};
        results.charge_distributions = {cds1, cds2, cds3};
        results.algorithm_name       = "test";

        const auto ground_state = determine_groundstate_from_simulation_results(results);
        CHECK(ground_state.size() == 1);
    }
    SECTION("Several charge distributions with degeneracy")
    {
        sidb_cell_clk_lyt lyt{};

        lyt.assign_cell_type({5, 4}, sidb_cell_clk_lyt::cell_type::NORMAL);
        lyt.assign_cell_type({6, 4}, sidb_cell_clk_lyt::cell_type::NORMAL);

        charge_distribution_surface cds1{lyt};
        cds1.assign_charge_state({5, 4}, sidb_charge_state::NEUTRAL);
        cds1.assign_charge_state({6, 4}, sidb_charge_state::NEGATIVE);
        cds1.update_after_charge_change();

        charge_distribution_surface cds2{lyt};
        cds2.assign_charge_state({5, 4}, sidb_charge_state::NEGATIVE);
        cds2.assign_charge_state({6, 4}, sidb_charge_state::NEUTRAL);
        cds2.update_after_charge_change();

        charge_distribution_surface cds3{lyt};
        cds2.assign_all_charge_states(sidb_charge_state::POSITIVE);
        cds3.update_after_charge_change();

        // copy cds2 to check for degeneracy.
        const charge_distribution_surface cds4{cds2};

        CHECK_THAT(cds2.get_system_energy() - cds1.get_system_energy(), Catch::Matchers::WithinAbs(0.0, 0.00001));

        sidb_simulation_result<sidb_cell_clk_lyt> results{};
        results.charge_distributions = {cds1, cds2, cds3, cds4};
        results.algorithm_name       = "test";

        const auto ground_states = determine_groundstate_from_simulation_results(results);
        REQUIRE(ground_states.size() == 2);
        CHECK_THAT(ground_states[0].get_system_energy() - ground_states[1].get_system_energy(),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
    }
}