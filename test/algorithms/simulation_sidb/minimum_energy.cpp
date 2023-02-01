//
// Created by Jan Drewniok on 18.01.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation_sidb/energy_distribution.hpp>
#include <fiction/algorithms/simulation_sidb/quicksim.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Test minimum energy function", "[minimum energy]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("layout with no SiDB placed")
    {
        TestType lyt{{10, 10}};

        std::vector<charge_distribution_surface<TestType>> all_lyts{};
        charge_distribution_surface                        charge_layout{lyt};

        CHECK(minimum_energy(all_lyts) == std::numeric_limits<double>::max());

        all_lyts.push_back(charge_layout);

        CHECK(std::abs(minimum_energy(all_lyts) - 0) < 0.00000001);
    }

    SECTION("layout with one SiDB placed")
    {
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
        std::vector<charge_distribution_surface<TestType>> all_lyts{};
        charge_distribution_surface                        charge_layout{lyt};

        CHECK(minimum_energy(all_lyts) == std::numeric_limits<double>::max());

        all_lyts.push_back(charge_layout);

        CHECK(std::abs(minimum_energy(all_lyts) - 0) < 0.00000001);
    }

    SECTION("layout with three SiDBs placed")
    {
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 10}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({9, 9}, TestType::cell_type::NORMAL);

        std::vector<charge_distribution_surface<TestType>> all_lyts{};
        charge_distribution_surface                        charge_layout_first{lyt};

        CHECK(minimum_energy(all_lyts) == std::numeric_limits<double>::max());

        charge_layout_first.assign_charge_state({0, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_first.update_local_potential();
        charge_layout_first.recompute_system_energy();
        all_lyts.push_back(charge_layout_first);

        charge_distribution_surface charge_layout_second{lyt};
        charge_layout_second.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_second.assign_charge_state({9, 9}, sidb_charge_state::NEUTRAL);
        charge_layout_second.update_local_potential();
        charge_layout_second.recompute_system_energy();
        all_lyts.push_back(charge_layout_second);

        CHECK(std::abs(minimum_energy(all_lyts) - 0) < 0.00000001);
    }
}
