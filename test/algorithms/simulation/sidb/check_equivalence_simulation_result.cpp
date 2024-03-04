//
// Created by Jan Drewniok on 04.03.24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/check_equivalence_simulation_result.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Conversion of potential to distance", "[check-equivalence-simulation-result]")
{
    sidb_cell_clk_lyt lyt1{};
    lyt1.assign_cell_type({0, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 1}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 2}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 3}, sidb_cell_clk_lyt::cell_type::NORMAL);

    auto lyt2{lyt1};

    lyt1.assign_cell_type({0, 5}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 6}, sidb_cell_clk_lyt::cell_type::NORMAL);

    charge_distribution_surface<sidb_cell_clk_lyt>       cds1{lyt1};
    const charge_distribution_surface<sidb_cell_clk_lyt> cds2{lyt2};

    sidb_simulation_result<sidb_cell_clk_lyt> results1{};
    sidb_simulation_result<sidb_cell_clk_lyt> results2{};

    results1.charge_distributions = {cds1, cds2};
    results2.charge_distributions = {cds2, cds1};

    SECTION("equality")
    {
        CHECK(check_equivalence_simulation_result(results1, results2));
    }

    SECTION("inequality of cds with different charge states")
    {
        cds1.assign_charge_state({0, 0}, sidb_charge_state::POSITIVE);
        results1.charge_distributions = {cds1, cds2};
        CHECK(!check_equivalence_simulation_result(results1, results2));
    }

    SECTION("inequality of cds with same charge state but different energy")
    {
        cds1.assign_system_energy_to_zero();
        results1.charge_distributions = {cds1, cds2};
        CHECK(!check_equivalence_simulation_result(results1, results2));
    }
}
