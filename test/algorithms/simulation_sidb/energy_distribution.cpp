//
// Created by Jan Drewniok on 18.01.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation_sidb/energy_distribution.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Test energy_distribution function", "[energy_distribution]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("one empty layout")
    {
        TestType                                           lyt{{10, 10}};
        std::vector<charge_distribution_surface<TestType>> all_lyts{};
        charge_distribution_surface                        charge_layout{lyt};
        all_lyts.push_back(charge_layout);
        auto result = energy_distribution(all_lyts);
        CHECK(result.size() == 1);
        CHECK(result[0.0] == 1);
    }

    SECTION("one layout with one SiDB placed")
    {
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
        std::vector<charge_distribution_surface<TestType>> all_lyts{};
        charge_distribution_surface                        charge_layout{lyt};
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
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 10}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({9, 9}, TestType::cell_type::NORMAL);

        const auto                                         sim_param = sidb_simulation_parameters{};
        std::vector<charge_distribution_surface<TestType>> all_lyts{};
        charge_distribution_surface                        charge_layout_first{lyt, sim_param};

        charge_layout_first.assign_charge_state({0, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_first.update_local_potential();
        charge_layout_first.system_energy();
        all_lyts.push_back(charge_layout_first);

        charge_distribution_surface charge_layout_second{lyt};
        charge_layout_second.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_second.assign_charge_state({9, 9}, sidb_charge_state::NEUTRAL);
        charge_layout_second.update_local_potential();
        charge_layout_second.system_energy();
        all_lyts.push_back(charge_layout_second);

        charge_distribution_surface charge_layout_third{lyt};
        charge_layout_third.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_third.assign_charge_state({9, 9}, sidb_charge_state::NEUTRAL);
        charge_layout_third.update_local_potential();
        charge_layout_third.system_energy();
        all_lyts.push_back(charge_layout_third);

        auto result = energy_distribution(all_lyts);

        // "all_lyts" collects all three layouts (charge_layout_first, charge_layout_second, charge_layout_third). The
        // last two have an identical potential energy (it.second == 2) which is smaller than the one from the first layout.
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
