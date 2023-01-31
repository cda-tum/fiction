//
// Created by Jan Drewniok on 18.12.22.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation_sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "exhaustive ground state search", "[ExGS]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("simulation with several SiDBs placed")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface      charge_layout{lyt};
        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(charge_layout, params, &exgs_stats);
        auto size_before = exgs_stats.valid_lyts.size();
        exhaustive_ground_state_simulation<TestType>(charge_layout, params, &exgs_stats);
        auto size_after = exgs_stats.valid_lyts.size();
        CHECK(size_before == size_after);

        CHECK(!exgs_stats.valid_lyts.empty());

        for (const auto& it : exgs_stats.valid_lyts)
        {
            CHECK(!it.charge_exists(sidb_charge_state::POSITIVE));
        }
    }

    SECTION("simulation with one SiDBs placed")
    {
        TestType lyt{{20, 10}};
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        charge_distribution_surface      charge_layout{lyt};
        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(charge_layout, params, &exgs_stats);

        CHECK(exgs_stats.valid_lyts.size() == 1);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
    }

    SECTION("simulation with no SiDB placed")
    {
        TestType lyt{{20, 10}};
        charge_distribution_surface      charge_layout{lyt};
        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(charge_layout, params, &exgs_stats);

        CHECK(exgs_stats.valid_lyts.size() == 0);  // empty layout is saved.

    }

}
