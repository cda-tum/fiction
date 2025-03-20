//
// Created by Jan Drewniok on 18.01.23.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/types.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Test energy_distribution function", "[energy-distribution]")
{
    SECTION("one empty layout")
    {
        const sidb_100_cell_clk_lyt_siqad                                     lyt{};
        std::vector<charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>> all_lyts{};
        const charge_distribution_surface                                     charge_layout{lyt};
        all_lyts.push_back(charge_layout);
        auto result = energy_distribution(all_lyts);
        CHECK(result.size() == 1);
        REQUIRE(result.get_nth_state(0).has_value());
        CHECK_THAT(result.get_nth_state(0).value().first, Catch::Matchers::WithinAbs(0.0, 0.00001));
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
        result.for_each([&](const auto& energy [[maybe_unused]], const auto& degeneracy) { CHECK(degeneracy == 1); });
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

        charge_layout_first.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL,
                                                charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_first.assign_charge_state({11, 10}, sidb_charge_state::NEUTRAL,
                                                charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_first.assign_charge_state({12, 10}, sidb_charge_state::NEUTRAL,
                                                charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_first.update_local_potential();
        charge_layout_first.recompute_system_energy();
        all_lyts.push_back(charge_layout_first);
        all_lyts.push_back(charge_layout_first);
        all_lyts.push_back(charge_layout_first);

        charge_distribution_surface charge_layout_second{lyt};
        charge_layout_second.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL,
                                                 charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_second.assign_charge_state({11, 10}, sidb_charge_state::NEGATIVE,
                                                 charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_second.assign_charge_state({12, 10}, sidb_charge_state::NEUTRAL,
                                                 charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_second.update_local_potential();
        charge_layout_second.recompute_system_energy();
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);

        charge_distribution_surface charge_layout_third{lyt};
        charge_layout_third.assign_charge_state({10, 10}, sidb_charge_state::NEGATIVE,
                                                charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_third.assign_charge_state({11, 10}, sidb_charge_state::NEGATIVE,
                                                charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_third.assign_charge_state({12, 10}, sidb_charge_state::NEGATIVE,
                                                charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_third.update_local_potential();
        charge_layout_third.recompute_system_energy();
        all_lyts.push_back(charge_layout_third);
        all_lyts.push_back(charge_layout_third);
        all_lyts.push_back(charge_layout_third);

        const auto result = energy_distribution(all_lyts);

        // "all_lyts" collects all three layouts (charge_layout_first, charge_layout_second, charge_layout_third). The
        // last two have an identical potential energy (it.second == 2) which is smaller than the one from the first
        // layout.
        REQUIRE(result.get_nth_state(0).has_value());
        CHECK(result.get_nth_state(0).value().second == 2);

        REQUIRE(result.get_nth_state(1).has_value());
        CHECK(result.get_nth_state(1).value().second == 1);
    }
}
