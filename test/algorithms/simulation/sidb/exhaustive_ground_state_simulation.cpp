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

        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({13, 0, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({17, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({19, 0, 0}, TestType::cell_type::NORMAL);

        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);
        auto size_before = exgs_stats.valid_lyts.size();
        exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);
        auto size_after = exgs_stats.valid_lyts.size();
        CHECK(size_before == 1);
        CHECK(size_after == 1);

        CHECK(exgs_stats.valid_lyts[0].get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({13, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(std::abs(exgs_stats.valid_lyts[0].get_system_energy() - 0.24602741408) <
              physical_constants::POP_STABILITY_ERR);
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
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(std::abs(exgs_stats.valid_lyts[0].get_system_energy() - 0.46621669) <
              physical_constants::POP_STABILITY_ERR);
    }
}
