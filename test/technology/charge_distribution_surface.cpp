//
// Created by Jan Drewniok on 05.12.22.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_surface.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "charge distribution surface traits and construction (layer on cell_level_layout)", "[charge-distribution-surface]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>>),
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>>))
{
    REQUIRE(is_cell_level_layout_v<TestType>);
    CHECK(!has_assign_charge_state_v<TestType>);
    CHECK(!has_get_charge_state_v<TestType>);
    CHECK(!has_foreach_charge_state_v<TestType>);
}

TEMPLATE_TEST_CASE(
    "charge distribution surface traits (layer on cell_level_layout)", "[charge-distribution-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    REQUIRE(is_cell_level_layout_v<TestType>);
    CHECK(!has_assign_charge_state_v<TestType>);
    CHECK(!has_get_charge_state_v<TestType>);
    CHECK(!has_foreach_charge_state_v<TestType>);
    CHECK(!has_assign_sidb_defect_v<TestType>);
    CHECK(!has_get_sidb_defect_v<TestType>);
    CHECK(!has_foreach_sidb_defect_v<TestType>);

    TestType lyt{};
    CHECK(!has_assign_sidb_defect_v<TestType>);
    CHECK(!has_get_sidb_defect_v<TestType>);
    CHECK(!has_foreach_sidb_defect_v<TestType>);
}

TEMPLATE_TEST_CASE(
    "assign and delete charge states", "[charge-distribution-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>),

    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>>),
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>>))

{
    TestType lyt{{11, 11}};

    SECTION("cell to index and vs")
    {
        // assign SiDBs and charge states to three different cells and read the charge state
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        CHECK(charge_layout.cell_to_index({5, 4}) != charge_layout.cell_to_index({5, 5}));
        CHECK(charge_layout.cell_to_index({5, 6}) != charge_layout.cell_to_index({5, 5}));
        CHECK(charge_layout.index_to_cell(4) == (siqad::coord_t()));
        uint64_t found = 0;
        for (uint64_t i = 0u; i < 3; i++)
        {
            if (charge_layout.index_to_cell(i) == (siqad::coord_t(5, 4)))
            {
                found += 1;
            }
        }
        CHECK(found == 1);

        found = 0;
        for (uint64_t i = 0u; i < 3; i++)
        {
            if (charge_layout.index_to_cell(i) == (siqad::coord_t(5, 5)))
            {
                found += 1;
            }
        }
        CHECK(found == 1);

        found = 0;
        for (uint64_t i = 0u; i < 3; i++)
        {
            if (charge_layout.index_to_cell(i) == (siqad::coord_t(5, 6)))
            {
                found += 1;
            }
        }
        CHECK(found == 1);
    }

    SECTION("charge distribution defined by a given charge index (base 3)")
    {
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.assign_charge_index(charge_layout.get_max_charge_index());
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::POSITIVE);

        charge_layout.assign_charge_index(0);
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("charge distribution defined by a given charge index (base 2)")
    {
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{2}};
        charge_layout.assign_charge_index(charge_layout.get_max_charge_index());
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEUTRAL);

        charge_layout.assign_charge_index(0);
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("perturber is replaced by an equivalent defect")
    {
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        lyt.assign_cell_type({5, 6}, TestType::cell_type::EMPTY);
        charge_distribution_surface charge_layout_new{lyt, sidb_simulation_parameters{}};
        charge_layout_new.assign_defect({5, 6}, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                            charge_layout_new.get_phys_params().epsilon_r,
                                                            charge_layout_new.get_phys_params().lambda_tf});
        charge_layout_new.update_after_charge_change();
        CHECK_THAT(charge_layout.get_system_energy() - charge_layout_new.get_system_energy(),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
    }

    SECTION("assign and read out charge states")
    {
        // assign SiDBs and charge states to three different cells and read the charge state
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.assign_charge_state({5, 4}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 6}, sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);

        CHECK(charge_layout.get_charge_state({7, 6}) == sidb_charge_state::NONE);

        charge_layout.assign_charge_state_by_cell_index(0, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state_by_cell_index(1, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state_by_cell_index(2, sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state_by_index(1) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state_by_index(2) == sidb_charge_state::POSITIVE);

        // check if charge state for empty cell is 'NONE'
        CHECK(charge_layout.get_charge_state_by_index(3) == sidb_charge_state::NONE);
    }

    SECTION("assign different charge states to the same cell")
    {
        // check if the charge state can be overwritten
        lyt.assign_cell_type({5, 1}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NONE);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("overwrite an assigned charge state")
    {
        // assign SiDBs and charge states to three different cells
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.assign_charge_state({5, 4}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 6}, sidb_charge_state::NEGATIVE);

        // all SiDBs' charge states are set to positive
        charge_layout.set_all_charge_states(sidb_charge_state::POSITIVE);

        // calculate potential between two sidbs (charge sign not included)
        CHECK(charge_layout.potential_between_sidbs({5, 4}, {5, 5}) > 0);
        CHECK_THAT(charge_layout.potential_between_sidbs({5, 4}, {5, 4}), Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK(charge_layout.potential_between_sidbs({5, 4}, {5, 6}) > 0);
        CHECK(charge_layout.potential_between_sidbs({5, 5}, {5, 6}) > 0);
        CHECK_THAT(charge_layout.potential_between_sidbs({5, 6}, {5, 5}) -
                       charge_layout.potential_between_sidbs({5, 5}, {5, 6}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.set_all_charge_states(sidb_charge_state::POSITIVE);

        // all SiDBs' charge states are set to neutral
        charge_layout.set_all_charge_states(sidb_charge_state::NEUTRAL);

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.set_all_charge_states(sidb_charge_state::NEUTRAL);

        // all SiDBs' charge states are set to negative
        charge_layout.set_all_charge_states(sidb_charge_state::NEGATIVE);

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.set_all_charge_states(sidb_charge_state::NEGATIVE);
    }

    SECTION("overwrite the charge state")
    {
        // assign SiDBs and charge states to three different cells
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        charge_layout.assign_charge_state({5, 4}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 6}, sidb_charge_state::NEGATIVE);

        // remove previously assigned charge states
        charge_layout.assign_charge_state({5, 4}, sidb_charge_state::NONE);
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NONE);
        charge_layout.assign_charge_state({5, 6}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NONE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NONE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::POSITIVE);
    }

    SECTION("Distance matrix")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        CHECK_THAT(charge_layout.get_nm_distance_between_cells({0, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(charge_layout.get_nm_distance_between_cells({0, 0, 0}, {1, 0, 0}),
                   Catch::Matchers::WithinAbs(sidb_simulation_parameters{}.lat_a * 0.1, 0.00001));
        CHECK_THAT(charge_layout.get_nm_distance_between_cells({1, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(sidb_simulation_parameters{}.lat_a * 0.1, 0.00001));
        CHECK_THAT(charge_layout.get_nm_distance_between_cells({1, 0, 0}, {1, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(charge_layout.get_nm_distance_between_cells({0, 0, 0}, {1, 1, 1}),
                   Catch::Matchers::WithinAbs(
                       std::hypot(sidb_simulation_parameters{}.lat_a * 0.1,
                                  sidb_simulation_parameters{}.lat_b * 0.1 + sidb_simulation_parameters{}.lat_c * 0.1),
                       0.00001));
        CHECK_THAT(charge_layout.get_nm_distance_between_cells({1, 1, 1}, {1, 1, 1}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
    }

    SECTION("Potential matrix")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 8, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 10, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({2, 8, 0}, {2, 10, 1}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({0, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 8, 0}, {1, 8, 0}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 10, 1}, {1, 10, 1}),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 8, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0121934043, 0.00001));
        CHECK_THAT(std::abs(charge_layout.get_chargeless_potential_between_sidbs({0, 0, 0}, {1, 10, 1}) -
                            charge_layout.get_chargeless_potential_between_sidbs({1, 10, 1}, {0, 0, 0})),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));

        CHECK(charge_layout.get_chargeless_potential_between_sidbs({0, 0, 0}, {1, 8, 0}) >
              charge_layout.get_chargeless_potential_between_sidbs({1, 10, 1}, {0, 0, 0}));
    }

    SECTION("Local Potential")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 8, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 10, 1}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.assign_charge_state({0, 0, 0}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({1, 8, 0}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({1, 10, 1}, sidb_charge_state::POSITIVE);

        charge_layout.update_local_potential();

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK(*p > 0.0);
            });

        charge_layout.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({1, 8, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({1, 10, 1}, sidb_charge_state::NEGATIVE);

        charge_layout.update_local_potential();

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK(*p < 0.0);
            });

        charge_layout.set_all_charge_states(sidb_charge_state::NEUTRAL);

        charge_layout.update_local_potential();

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK_THAT(*p, Catch::Matchers::WithinAbs(0.0, 0.00001));
            });
    }

    SECTION("Electrostatic potential energy of the charge configuration")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        charge_layout.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({1, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({1, 1, 1}, sidb_charge_state::NEGATIVE);

        // system energy is zero when all SiDBs are positively charged.
        charge_layout.update_local_potential();
        charge_layout.recompute_system_energy();
        CHECK(charge_layout.get_system_energy() > 0);

        // system energy is zero when all SiDBs are neutrally charged.
        charge_layout.set_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout.update_local_potential();
        charge_layout.recompute_system_energy();
        CHECK_THAT(charge_layout.get_system_energy(), Catch::Matchers::WithinAbs(0.0, 0.00001));

        // system energy is zero when all SiDBs are positively charged.
        charge_layout.set_all_charge_states(sidb_charge_state::POSITIVE);
        charge_layout.update_local_potential();
        charge_layout.recompute_system_energy();
        CHECK(charge_layout.get_system_energy() > 0);
    }

    SECTION("Physical validity check, far distance of SIDBs, all NEGATIVE")
    {

        TestType layout{{11, 11}};
        layout.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({4, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_five{layout, sidb_simulation_parameters{}};
        CHECK(charge_layout_five.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_five.get_charge_state({0, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_five.get_charge_state({4, 1, 1}) == sidb_charge_state::NEGATIVE);

        charge_layout_five.update_local_potential();
        charge_layout_five.recompute_system_energy();
        charge_layout_five.validity_check();
        CHECK(charge_layout_five.get_charge_index().first == 0);

        charge_layout_five.assign_charge_state({4, 1, 1}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout_five.get_charge_state({4, 1, 1}) == sidb_charge_state::POSITIVE);
        charge_layout_five.charge_distribution_to_index();
        CHECK(charge_layout_five.get_charge_index().first == 2);

        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 3);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 4);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 5);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 6);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 7);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 8);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 9);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index().first == 10);

        CHECK(charge_layout_five.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout_five.get_charge_state({0, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_five.get_charge_state({4, 1, 1}) == sidb_charge_state::NEUTRAL);
    }

    SECTION("Physical validity check, small distance, not all can be negatively charged anymore")
    {
        lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 2, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.assign_charge_state({1, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({0, 2, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({0, 2, 1}, sidb_charge_state::NEGATIVE);

        // closely arranged SiDBs cannot be all negatively charged
        charge_layout.update_local_potential();
        charge_layout.recompute_system_energy();
        charge_layout.validity_check();
        CHECK(!charge_layout.is_physically_valid());
    }

    SECTION("apply external voltage at two cells")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new,
                                                      params,
                                                      sidb_charge_state::NEUTRAL,
                                                      {},
                                                      {{{0, 0, 1}, -0.5}}};
        REQUIRE(!charge_layout_new.get_external_potentials().empty());
        CHECK(charge_layout_new.get_external_potentials().size() == 1);
        CHECK(charge_layout_new.get_external_potentials().size() == 1);

        CHECK_THAT(*charge_layout_new.get_local_potential({0, 0, 1}) + 0.5,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(*charge_layout_new.get_local_potential({1, 3, 0}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
        CHECK_THAT(*charge_layout_new.get_local_potential({10, 5, 1}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
        charge_layout_new.set_all_charge_states(sidb_charge_state::POSITIVE);
        charge_layout_new.update_after_charge_change();
        CHECK(charge_layout_new.get_charge_state({0, 0, 1}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({10, 5, 1}) == sidb_charge_state::POSITIVE);

        CHECK(*charge_layout_new.get_local_potential({0, 0, 1}) > -0.5);
        CHECK(*charge_layout_new.get_local_potential({1, 3, 0}) > -0.5);
        CHECK(*charge_layout_new.get_local_potential({10, 5, 1}) > -0.5);

        charge_layout_new.set_all_charge_states(sidb_charge_state::NEUTRAL);

        charge_layout_new.set_local_external_potential({{{0, 0, 1}, -0.1}});
        CHECK_THAT(*charge_layout_new.get_local_potential({0, 0, 1}) + 0.1,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(*charge_layout_new.get_local_potential({1, 3, 0}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
        CHECK_THAT(*charge_layout_new.get_local_potential({10, 5, 1}), Catch::Matchers::WithinAbs(0.000000, 0.000001));

        charge_layout_new.set_local_external_potential({{{0, 0, 1}, -0.5}, {{10, 5, 1}, -0.1}});
        charge_layout_new.set_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();

        CHECK(*charge_layout_new.get_local_potential({0, 0, 1}) < -0.5);
        CHECK(*charge_layout_new.get_local_potential({10, 5, 1}) < -0.1);
        CHECK(*charge_layout_new.get_local_potential({1, 3, 0}) < 0);
    }

    SECTION("apply homogenous external voltage to layout")
    {
        const sidb_simulation_parameters params{3, -0.32};
        lyt.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEUTRAL};
        CHECK(charge_layout.get_external_potentials().empty());
        charge_layout.set_global_external_potential(-0.1);
        CHECK(!charge_layout.get_external_potentials().empty());

        CHECK_THAT(*charge_layout.get_local_potential({0, 0, 1}) + 0.1,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(*charge_layout.get_local_potential({1, 3, 0}) + 0.1,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(*charge_layout.get_local_potential({10, 5, 1}) + 0.1,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));

        lyt.assign_cell_type({0, 0, 1}, TestType::cell_type::EMPTY);
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::EMPTY);
        lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::EMPTY);
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout_new{lyt, params, sidb_charge_state::NEGATIVE};
        charge_layout_new.assign_charge_state({3, 0, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({5, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();
        CHECK(charge_layout_new.is_physically_valid());

        charge_layout_new.set_global_external_potential(0.2);
        CHECK(!charge_layout_new.is_physically_valid());

        charge_layout_new.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({3, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({5, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();
        CHECK(charge_layout_new.is_physically_valid());
    }

    SECTION("no external voltage given")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};

        CHECK_THAT(*charge_layout_new.get_local_potential({0, 0, 1}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
        CHECK_THAT(*charge_layout_new.get_local_potential({1, 3, 1}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
        CHECK_THAT(*charge_layout_new.get_local_potential({10, 5, 1}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
    }

    SECTION("assign defect | negative defect")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL, {}};

        charge_layout_new.assign_defect({5, 1, 1}, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                               charge_layout_new.get_phys_params().epsilon_r,
                                                               charge_layout_new.get_phys_params().lambda_tf});

        CHECK(*charge_layout_new.get_local_potential({0, 0, 1}) < 0);
        CHECK(*charge_layout_new.get_local_potential({1, 3, 0}) < 0);
        CHECK(*charge_layout_new.get_local_potential({10, 5, 1}) < 0);
    }

    SECTION("assign defect | positive defect")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL, {}};

        charge_layout_new.assign_defect({5, 1, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 1,
                                                               charge_layout_new.get_phys_params().epsilon_r,
                                                               charge_layout_new.get_phys_params().lambda_tf});

        CHECK(*charge_layout_new.get_local_potential({0, 0, 1}) > 0);
        CHECK(*charge_layout_new.get_local_potential({1, 3, 0}) > 0);
        CHECK(*charge_layout_new.get_local_potential({10, 5, 1}) > 0);
    }

    SECTION("assign defect and perturber")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL, {}};

        charge_layout_new.assign_charge_state({10, 5, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_defect({-10, 5, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 1,
                                                                 charge_layout_new.get_phys_params().epsilon_r,
                                                                 charge_layout_new.get_phys_params().lambda_tf});

        CHECK_THAT(*charge_layout_new.get_local_potential({0, 0, 1}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
    }

    SECTION("layout with perturber |assigning and erasing defect")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt_new, params, sidb_charge_state::NEUTRAL, {}};

        charge_layout.assign_charge_state({10, 5, 1}, sidb_charge_state::NEGATIVE);
        charge_layout.update_after_charge_change();
        charge_layout.assign_defect({-10, 5, 1},
                                    sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_phys_params().epsilon_r,
                                                charge_layout.get_phys_params().lambda_tf});

        CHECK_THAT(*charge_layout.get_local_potential({0, 0, 0}), Catch::Matchers::WithinAbs(0.000000, 0.000001));

        charge_layout.erase_defect({-10, 5, 1});
        CHECK(charge_layout.get_local_potential({0, 0, 0}) < 0);
    }

    SECTION("layout with neutrally charged SiDBs |assigning and erasing defect")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt_new, params, sidb_charge_state::NEUTRAL};
        charge_layout.assign_defect({-10, 5, 1},
                                    sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_phys_params().epsilon_r,
                                                charge_layout.get_phys_params().lambda_tf});

        CHECK(*charge_layout.get_local_potential({0, 0, 0}) > 0);
        CHECK(*charge_layout.get_local_potential({10, 5, 1}) > 0);

        charge_layout.erase_defect({-10, 5, 1});
        CHECK_THAT(*charge_layout.get_local_potential({0, 0, 0}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
        CHECK_THAT(*charge_layout.get_local_potential({10, 5, 1}), Catch::Matchers::WithinAbs(0.000000, 0.000001));
    }

    SECTION("increase charge index")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params};
        const auto                  negative_sidbs = charge_layout_new.negative_sidb_detection();
        REQUIRE(negative_sidbs.size() == 3);
        CHECK(negative_sidbs[0] == 0);
        CHECK(negative_sidbs[1] == 1);
        CHECK(negative_sidbs[2] == 2);
        CHECK(charge_layout_new.get_charge_index().first == 0);

        charge_layout_new.set_all_charge_states(sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({0, 0, 1}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({10, 5, 1}) == sidb_charge_state::POSITIVE);

        charge_layout_new.charge_distribution_to_index();
        CHECK(charge_layout_new.get_charge_index().first == 26);

        charge_layout_new.set_all_charge_states(sidb_charge_state::NEUTRAL);

        charge_layout_new.charge_distribution_to_index();
        CHECK(charge_layout_new.get_charge_index().first == 13);

        charge_layout_new.increase_charge_index_by_one();
        charge_layout_new.charge_distribution_to_index();
        CHECK(charge_layout_new.get_charge_index().first == 14);
        charge_layout_new.increase_charge_index_by_one();
        CHECK(charge_layout_new.get_charge_index().first == 15);
    }

    SECTION("detecting perturber in layout with only one SiDB")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params};
        const auto                  negative_sidbs = charge_layout_new.negative_sidb_detection();
        REQUIRE(negative_sidbs.size() == 1);
    }

    SECTION("Seven randomly distributed DB | checking for physical validity")
    {
        TestType lyt_new{{11, 11}};
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);
        const sidb_simulation_parameters params{2, -0.28};
        charge_distribution_surface      charge_layout_new{lyt, params};
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({3, 3, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({4, 3, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({6, 3, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({7, 3, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({6, 10, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({7, 10, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();

        REQUIRE(charge_layout_new.is_physically_valid());
    }

    SECTION("using chargeless and normal potential function")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32, 5.0, 3.84, 7.68, 2.25};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params};

        charge_layout_new.assign_charge_state({0, 0, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::POSITIVE);
        charge_layout_new.assign_charge_state({10, 5, 1}, sidb_charge_state::NEUTRAL);

        CHECK(charge_layout_new.get_charge_state({0, 0, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({10, 5, 1}) == sidb_charge_state::NEUTRAL);

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {1, 3, 0}) > 0);
        CHECK(charge_layout_new.get_potential_between_sidbs({0, 0, 1}, {1, 3, 0}) > 0);

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {10, 5, 1}) > 0);
        CHECK_THAT(charge_layout_new.get_potential_between_sidbs({0, 0, 1}, {10, 5, 1}),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({10, 5, 1}, {0, 0, 1}) > 0);
        CHECK(charge_layout_new.get_potential_between_sidbs({10, 5, 1}, {0, 0, 1}) < 0);

        CHECK_THAT(charge_layout_new.get_potential_between_sidbs({10, 5, 1}, {0, 0, 1}) +
                       charge_layout_new.get_chargeless_potential_between_sidbs({10, 5, 1}, {0, 0, 1}),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));

        CHECK_THAT(charge_layout_new.get_potential_between_sidbs({0, 0, 1}, {1, 3, 0}) -
                       charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {1, 3, 0}),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));
    }

    SECTION("adding dependent cell")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 3, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 4, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEGATIVE, {10, 4, 1}};

        charge_layout_new.assign_charge_state({0, 3, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);

        CHECK(charge_layout_new.get_charge_state({0, 3, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEUTRAL);
        charge_layout_new.update_after_charge_change();
        CHECK(*charge_layout_new.get_local_potential({0, 3, 1}) < 0);
        CHECK(*charge_layout_new.get_local_potential({1, 3, 0}) < 0);
        CHECK(*charge_layout_new.get_local_potential({10, 4, 1}) < 0);
        charge_layout_new.update_charge_state_of_dependent_cell();
        CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("adding dependent cell and increase index")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{2, -0.32};

        lyt_new.assign_cell_type({0, 3, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 4, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEGATIVE, {10, 4, 1}};

        CHECK(*charge_layout_new.get_local_potential({0, 3, 1}) < 0);
        CHECK(*charge_layout_new.get_local_potential({1, 3, 0}) < 0);
        CHECK(*charge_layout_new.get_local_potential({10, 4, 1}) < 0);

        for (uint64_t i = 0; i < 3; i++)
        {
            charge_layout_new.increase_charge_index_by_one();
            CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEGATIVE);
        }
    }

    SECTION("adding dependent cell and compare local potential and system energy")
    {
        TestType                         lyt_new{{11, 11}};
        const sidb_simulation_parameters params{2, -0.32};

        lyt_new.assign_cell_type({0, 3, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 4, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEGATIVE, {10, 4, 1}};

        charge_layout_new.assign_charge_state({0, 3, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
        charge_layout_new.update_after_charge_change();

        CHECK(*charge_layout_new.get_local_potential({0, 3, 1}) == 0.0);
        CHECK(*charge_layout_new.get_local_potential({1, 3, 0}) < 0);
        CHECK(*charge_layout_new.get_local_potential({10, 4, 1}) < 0);

        charge_layout_new.update_charge_state_of_dependent_cell();
        CHECK(charge_layout_new.get_charge_state({0, 3, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEGATIVE);
        const auto loc_one   = *charge_layout_new.get_local_potential({0, 3, 1});
        const auto loc_two   = *charge_layout_new.get_local_potential({1, 3, 0});
        const auto loc_three = *charge_layout_new.get_local_potential({10, 4, 1});
        CHECK(loc_one < 0);
        CHECK(loc_two < 0);
        CHECK(loc_three < 0);
        charge_layout_new.recompute_system_energy();
        auto system_energy_first = charge_layout_new.get_system_energy();

        charge_layout_new.assign_charge_state({0, 3, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();
        CHECK_THAT(loc_one - *charge_layout_new.get_local_potential({0, 3, 1}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_two - *charge_layout_new.get_local_potential({1, 3, 0}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_three - *charge_layout_new.get_local_potential({10, 4, 1}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        auto system_energy_second = charge_layout_new.get_system_energy();
        CHECK_THAT(system_energy_first - system_energy_second,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Physical validity check after dependent cell is updated")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt,
                                                  sidb_simulation_parameters{},
                                                  sidb_charge_state::NEGATIVE,
                                                  {3, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.is_physically_valid() == 0);

        charge_layout.update_charge_state_of_dependent_cell();
        charge_layout.update_after_charge_change(false);
        charge_layout.validity_check();
        CHECK(charge_layout.is_physically_valid() == 1);
    }

    SECTION("check charge index")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE, {3, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_index().first == 0);

        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index().first == 1);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index().first == 2);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index().first == 3);

        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_index().first == 3);
    }

    SECTION("Physical validity check after dependent cell is updated")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt,
                                                  sidb_simulation_parameters{},
                                                  sidb_charge_state::NEGATIVE,
                                                  {3, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.is_physically_valid() == 0);

        charge_layout.update_charge_state_of_dependent_cell();
        charge_layout.update_after_charge_change(false);
        charge_layout.validity_check();
        CHECK(charge_layout.is_physically_valid() == 1);
    }

    SECTION("check charge index")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE, {0, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_index().first == 0);

        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index().first == 1);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index().first == 2);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index().first == 3);

        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_index().first == 3);
    }

    SECTION("experiments with defects")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE, {0, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        auto loc_one_wo_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_wo_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_wo_defect = *charge_layout.get_local_potential({5, 0, 0});

        charge_layout.assign_defect({-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                            charge_layout.get_phys_params().epsilon_r,
                                                            charge_layout.get_phys_params().lambda_tf});
        auto loc_one_w_negative_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_negative_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_negative_defect = *charge_layout.get_local_potential({5, 0, 0});
        auto defect_potentials_negative  = charge_layout.get_defect_potentials();
        REQUIRE(!defect_potentials_negative.empty());

        CHECK(loc_one_wo_defect > loc_one_w_negative_defect);
        CHECK(loc_two_wo_defect > loc_two_w_negative_defect);
        CHECK(loc_three_wo_defect > loc_three_w_negative_defect);

        charge_layout.assign_defect({-4, 0, 0},
                                    sidb_defect{sidb_defect_type::UNKNOWN, 0, charge_layout.get_phys_params().epsilon_r,
                                                charge_layout.get_phys_params().lambda_tf});
        auto loc_one_w_neutral_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_neutral_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_neutral_defect = *charge_layout.get_local_potential({5, 0, 0});
        CHECK_THAT(loc_one_wo_defect - loc_one_w_neutral_defect,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_two_wo_defect - loc_two_w_neutral_defect,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_three_wo_defect - loc_three_w_neutral_defect,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));

        charge_layout.assign_defect({-4, 0, 0},
                                    sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_phys_params().epsilon_r,
                                                charge_layout.get_phys_params().lambda_tf});
        auto loc_one_w_positive_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_positive_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_positive_defect = *charge_layout.get_local_potential({5, 0, 0});
        auto defect_potentials_positive  = charge_layout.get_defect_potentials();
        REQUIRE(!defect_potentials_positive.empty());

        CHECK(loc_one_w_positive_defect > loc_one_w_neutral_defect);
        CHECK(loc_two_w_positive_defect > loc_two_w_neutral_defect);
        CHECK(loc_three_w_positive_defect > loc_three_w_neutral_defect);

        CHECK_THAT((defect_potentials_negative[{0, 0, 0}] + defect_potentials_positive[{0, 0, 0}]),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));
        CHECK_THAT((defect_potentials_negative[{3, 0, 0}] + defect_potentials_positive[{3, 0, 0}]),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));
        CHECK_THAT((defect_potentials_negative[{5, 0, 0}] + defect_potentials_positive[{5, 0, 0}]),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));
    }

    SECTION("experiments with defects | assigning and reassigning defects")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE, {0, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        auto loc_one_wo_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_wo_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_wo_defect = *charge_layout.get_local_potential({5, 0, 0});

        charge_layout.assign_defect({-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                            charge_layout.get_phys_params().epsilon_r,
                                                            charge_layout.get_phys_params().lambda_tf});
        auto loc_one_w_negative_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_negative_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_negative_defect = *charge_layout.get_local_potential({5, 0, 0});
        auto defect_potentials_negative  = charge_layout.get_defect_potentials();
        REQUIRE(!defect_potentials_negative.empty());

        CHECK(loc_one_wo_defect > loc_one_w_negative_defect);
        CHECK(loc_two_wo_defect > loc_two_w_negative_defect);
        CHECK(loc_three_wo_defect > loc_three_w_negative_defect);

        charge_layout.assign_defect({-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 0.0,
                                                            charge_layout.get_phys_params().epsilon_r,
                                                            charge_layout.get_phys_params().lambda_tf});
        auto loc_one_w_neutral_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_neutral_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_neutral_defect = *charge_layout.get_local_potential({5, 0, 0});

        CHECK_THAT(loc_one_wo_defect - loc_one_w_neutral_defect,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_two_wo_defect - loc_two_w_neutral_defect,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_three_wo_defect - loc_three_w_neutral_defect,
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));

        charge_layout.assign_defect({-4, 0, 0},
                                    sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_phys_params().epsilon_r,
                                                charge_layout.get_phys_params().lambda_tf});
        auto loc_one_w_positive_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_positive_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_positive_defect = *charge_layout.get_local_potential({5, 0, 0});
        auto defect_potentials_positive  = charge_layout.get_defect_potentials();
        REQUIRE(!defect_potentials_positive.empty());

        CHECK(loc_one_w_positive_defect > loc_one_w_neutral_defect);
        CHECK(loc_two_w_positive_defect > loc_two_w_neutral_defect);
        CHECK(loc_three_w_positive_defect > loc_three_w_neutral_defect);

        CHECK_THAT((defect_potentials_negative[{0, 0, 0}] + defect_potentials_positive[{0, 0, 0}]),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));
        CHECK_THAT((defect_potentials_negative[{3, 0, 0}] + defect_potentials_positive[{3, 0, 0}]),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));
        CHECK_THAT((defect_potentials_negative[{5, 0, 0}] + defect_potentials_positive[{5, 0, 0}]),
                   Catch::Matchers::WithinAbs(0.0, 0.000001));
    }

    SECTION("assign defect on DB position which is not allowed")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEUTRAL, {0, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        auto loc_one_wo_defect   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_wo_defect   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_wo_defect = *charge_layout.get_local_potential({5, 0, 0});

        charge_layout.assign_defect({0, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                           charge_layout.get_phys_params().epsilon_r,
                                                           charge_layout.get_phys_params().lambda_tf});

        CHECK_THAT(loc_one_wo_defect - *charge_layout.get_local_potential({0, 0, 0}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_two_wo_defect - *charge_layout.get_local_potential({3, 0, 0}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(loc_three_wo_defect - *charge_layout.get_local_potential({5, 0, 0}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));

        auto defect_potentials_negative = charge_layout.get_defect_potentials();
        CHECK(defect_potentials_negative.empty());
    }

    SECTION("assign defects with different screening lengths")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEUTRAL, {0, 0, 0}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);

        charge_layout.assign_defect({8, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                           charge_layout.get_phys_params().epsilon_r,
                                                           charge_layout.get_phys_params().lambda_tf});

        auto loc_one_w_defect_normal_screening   = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_defect_normal_screening   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_defect_normal_screening = *charge_layout.get_local_potential({5, 0, 0});

        charge_layout.assign_defect({8, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                           charge_layout.get_phys_params().epsilon_r,
                                                           charge_layout.get_phys_params().lambda_tf * 20});

        auto loc_one_w_defec_strong_screening    = *charge_layout.get_local_potential({0, 0, 0});
        auto loc_two_w_defect_strong_screening   = *charge_layout.get_local_potential({3, 0, 0});
        auto loc_three_w_defect_strong_screening = *charge_layout.get_local_potential({5, 0, 0});

        auto defect_potentials_negative = charge_layout.get_defect_potentials();
        CHECK(!defect_potentials_negative.empty());

        CHECK(loc_one_w_defect_normal_screening > loc_one_w_defec_strong_screening);
        CHECK(loc_two_w_defect_normal_screening > loc_two_w_defect_strong_screening);
        CHECK(loc_three_w_defect_normal_screening > loc_three_w_defect_strong_screening);

        charge_layout.erase_defect({8, 0, 0});
        CHECK_THAT(*charge_layout.get_local_potential({0, 0, 0}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(*charge_layout.get_local_potential({3, 0, 0}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(*charge_layout.get_local_potential({5, 0, 0}),
                   Catch::Matchers::WithinAbs(0, fiction::physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Y-shape SiDB OR gate with input 01 and global external potential (high)")
    {

        lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{3, -0.28};
        charge_distribution_surface      charge_lyt_first{lyt, params, sidb_charge_state::POSITIVE};
        charge_lyt_first.set_global_external_potential(-2.0);
        CHECK(charge_lyt_first.is_physically_valid());
    }

    SECTION("detecting DBs which could be positively charged due to maximal band bending")
    {

        lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 2, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({10, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({20, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({23, 2, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{3, -0.28};
        charge_distribution_surface      charge_lyt_first{lyt, params, sidb_charge_state::NEGATIVE};
        charge_lyt_first.three_state_sim_required();
        const auto positive_candidates = charge_lyt_first.get_positive_candidates();
        REQUIRE(positive_candidates.size() == 3);
        uint64_t loop_counter = 0;
        for (const auto& cell : positive_candidates)
        {
            if (cell == siqad::coord_t(6, 2, 0))
            {
                loop_counter += 1;
            }
        }
        CHECK(loop_counter == 1);

        uint64_t loop_counter_second = 0;
        for (const auto& cell : positive_candidates)
        {
            if (cell == siqad::coord_t(7, 2, 0))
            {
                loop_counter_second += 1;
            }
        }
        CHECK(loop_counter_second == 1);

        uint64_t loop_counter_third = 0;
        for (const auto& cell : positive_candidates)
        {
            if (cell == siqad::coord_t(7, 2, 0))
            {
                loop_counter_third += 1;
            }
        }
        CHECK(loop_counter == 1);
    }

    SECTION("detecting DBs which could be positively charged due to maximal band bending")
    {

        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{3, -0.32};
        charge_distribution_surface      charge_lyt_first{lyt, params, sidb_charge_state::NEGATIVE};
        charge_lyt_first.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_lyt_first.assign_charge_state({4, 0, 0}, sidb_charge_state::NEUTRAL);
        charge_lyt_first.assign_charge_state({6, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_lyt_first.update_after_charge_change();
    }
}
