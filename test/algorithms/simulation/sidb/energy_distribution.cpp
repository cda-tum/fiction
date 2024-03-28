//
// Created by Jan Drewniok on 18.01.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/types.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Test energy_distribution function", "[energy-distribution]")
{
    SECTION("one empty layout")
    {
        sidb_100_cell_clk_lyt_siqad                                           lyt{};
        std::vector<charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>> all_lyts{};
        const charge_distribution_surface                                     charge_layout{lyt};
        all_lyts.push_back(charge_layout);
        auto result = energy_distribution(all_lyts);
        CHECK(result.size() == 1);
        CHECK(result[0.0] == 1);
    }

    SECTION("one layout with one SiDB placed")
    {
        sidb_100_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
        std::vector<charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>> all_lyts{};
        charge_distribution_surface                                           charge_layout{lyt};
        charge_layout.assign_charge_state({0, 0}, sidb_charge_state::NEUTRAL);
        all_lyts.push_back(charge_layout);

        auto result = energy_distribution(all_lyts);
        CHECK(result.size() == 1);
        for (const auto& it : result)
        {
            CHECK(it.second == 1);
        }
    }

    SECTION("several layouts")
    {
        sidb_100_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({10, 10}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({11, 10}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({12, 10}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);

        const auto                                                            sim_param = sidb_simulation_parameters{};
        std::vector<charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>> all_lyts{};
        charge_distribution_surface                                           charge_layout_first{lyt, sim_param};

        charge_layout_first.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_first.assign_charge_state({11, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_first.assign_charge_state({12, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_first.update_local_potential();
        charge_layout_first.recompute_system_energy();
        all_lyts.push_back(charge_layout_first);
        all_lyts.push_back(charge_layout_first);
        all_lyts.push_back(charge_layout_first);

        charge_distribution_surface charge_layout_second{lyt};
        charge_layout_second.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_second.assign_charge_state({11, 10}, sidb_charge_state::NEGATIVE);
        charge_layout_second.assign_charge_state({12, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_second.update_local_potential();
        charge_layout_second.recompute_system_energy();
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);

        charge_distribution_surface charge_layout_third{lyt};
        charge_layout_third.assign_charge_state({10, 10}, sidb_charge_state::NEGATIVE);
        charge_layout_third.assign_charge_state({11, 10}, sidb_charge_state::NEGATIVE);
        charge_layout_third.assign_charge_state({12, 10}, sidb_charge_state::NEGATIVE);
        charge_layout_third.update_local_potential();
        charge_layout_third.recompute_system_energy();
        all_lyts.push_back(charge_layout_third);
        all_lyts.push_back(charge_layout_third);
        all_lyts.push_back(charge_layout_third);

        auto result = energy_distribution(all_lyts);

        // "all_lyts" collects all three layouts (charge_layout_first, charge_layout_second, charge_layout_third). The
        // last two have an identical potential energy (it.second == 2) which is smaller than the one from the first
        // layout.
        auto counter = 0u;
        for (const auto& it : result)
        {
            if (counter == 0)
            {
                CHECK(it.second == 2);
            }
            else
            {
                CHECK(it.second == 1);
            }
            counter += 1;
        }
    }
}
