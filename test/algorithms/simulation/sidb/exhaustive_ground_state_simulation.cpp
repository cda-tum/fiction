//
// Created by Jan Drewniok on 18.12.22.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Exhaustive ground state search", "[ExGS]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("simulation with one SiDB placed")
    {
        TestType lyt{{20, 10}};
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

        CHECK(exgs_stats.valid_lyts.size() == 1);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
    }

    SECTION("simulation with no SiDB placed")
    {
        TestType lyt{{20, 10}};

        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

        CHECK(exgs_stats.valid_lyts.size() == 0);  // empty layout is saved.
    }

    SECTION("simulation of wire with one perturber and two SiDB pairs")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);
        auto size_before = exgs_stats.valid_lyts.size();
        exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);
        auto size_after = exgs_stats.valid_lyts.size();
        CHECK(size_before == 1);
        CHECK(size_after == 1);

        CHECK(!exgs_stats.valid_lyts.empty());

        for (const auto& it : exgs_stats.valid_lyts)
        {
            CHECK(!it.charge_exists(sidb_charge_state::POSITIVE));
        }

        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(1) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(2) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(3) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(4) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(5) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(6) == sidb_charge_state::NEUTRAL);
    }

    SECTION("simulation of SiQAD-OR gate with input 01")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.28};
        exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

        CHECK(exgs_stats.valid_lyts[0].get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(1) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(2) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(3) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(4) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(5) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(6) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(7) == sidb_charge_state::NEGATIVE);

        CHECK(exgs_stats.valid_lyts[0].get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
    }
}
