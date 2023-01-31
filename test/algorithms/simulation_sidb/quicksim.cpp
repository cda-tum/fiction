//
// Created by Jan Drewniok on 18.12.22.
//

#include <fiction/algorithms/simulation_sidb/quicksim.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "quicksim test", "[quicksim]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("layout with no SiDB placed")
    {
        TestType lyt{{20, 10}};

        charge_distribution_surface      charge_layout{lyt};
        quicksim_stats<TestType>         quicksimstats{};
        const quicksim_params            quicksim_params{sidb_simulation_parameters{2, -0.30}};
        CHECK(quicksim_params.phys_params.mu == -0.30);
        quicksim<TestType>(charge_layout, quicksim_params, &quicksimstats);

        CHECK(quicksimstats.valid_lyts.empty());
    }

    SECTION("layout with one SiDB placed")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface      charge_layout{lyt};
        quicksim_stats<TestType>         quicksimstats{};
        const quicksim_params            quicksim_params{sidb_simulation_parameters{2, -0.30}};
        CHECK(quicksim_params.phys_params.mu == -0.30);
        quicksim<TestType>(charge_layout, quicksim_params, &quicksimstats);

        CHECK(quicksimstats.valid_lyts.size()==1);
    }

    SECTION("layout with several SiDBs placed")
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
        quicksim_stats<TestType>         quicksimstats{};
        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};
        CHECK(quicksim_params.phys_params.mu == -0.30);
        quicksim<TestType>(charge_layout, quicksim_params, &quicksimstats);

        CHECK(!quicksimstats.valid_lyts.empty());

        for (const auto& it : quicksimstats.valid_lyts)
        {
            CHECK(!it.charge_exists(sidb_charge_state::POSITIVE));
        }
    }
}
