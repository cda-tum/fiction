//
// Created by Jan Drewniok on 04.03.24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/equivalence_check_for_simulation_results.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Several tests", "[equivalence-check-for-simulation-results]")
{
    sidb_100_cell_clk_lyt lyt1{};
    lyt1.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 1}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 2}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 3}, sidb_100_cell_clk_lyt::cell_type::NORMAL);

    cds_sidb_100_cell_clk_lyt       cds1{lyt1};
    const cds_sidb_100_cell_clk_lyt cds2{lyt1, sidb_simulation_parameters{}, sidb_charge_state::NEUTRAL};

    sidb_simulation_result<sidb_100_cell_clk_lyt> results1{};
    sidb_simulation_result<sidb_100_cell_clk_lyt> results2{};

    results1.charge_distributions = {cds1, cds2};
    results2.charge_distributions = {cds2, cds1};

    SECTION("equivalence")
    {
        CHECK(check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence with respect to cds with different charge states")
    {
        cds1.assign_charge_state({0, 0}, sidb_charge_state::POSITIVE);
        results1.charge_distributions = {cds1, cds2};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence with respect to cds with same charge state but different energy")
    {
        cds1.assign_electrostatic_potential_energy_to_zero();
        results1.charge_distributions = {cds1, cds2};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to different number of solutions")
    {
        results1.charge_distributions = {cds1};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to different number of SiDBs")
    {
        auto lyt2{lyt1.clone()};
        lyt2.assign_cell_type({4, 2}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
        lyt2.assign_cell_type({4, 3}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
        results2.charge_distributions = {
            charge_distribution_surface{lyt2, sidb_simulation_parameters{}, sidb_charge_state::NEUTRAL},
            charge_distribution_surface{lyt2}};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to duplication in first solution")
    {
        results1.charge_distributions = {charge_distribution_surface{lyt1}, charge_distribution_surface{lyt1}};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }
    SECTION("non-equivalence due to duplication in second solution")
    {
        results2.charge_distributions = {charge_distribution_surface{lyt1}, charge_distribution_surface{lyt1}};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }
    SECTION("non-equivalence due to different charge states of the first and the second solution")
    {
        results1.charge_distributions = {charge_distribution_surface{lyt1}};
        results2.charge_distributions = {charge_distribution_surface{lyt1}};
        results2.charge_distributions.at(0).assign_charge_state({0, 0}, sidb_charge_state::POSITIVE);
        results2.charge_distributions.at(0).assign_electrostatic_potential_energy_to_zero();
        results1.charge_distributions.at(0).assign_electrostatic_potential_energy_to_zero();
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }
}
