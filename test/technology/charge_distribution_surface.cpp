//
// Created by Jan Drewniok on 05.12.22.
//

#include <catch2/catch_template_test_macros.hpp>

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
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>>))
{
    REQUIRE(is_cell_level_layout_v<TestType>);
    CHECK(!has_assign_charge_state_v<TestType>);
    CHECK(!has_get_charge_state_v<TestType>);
    CHECK(!has_foreach_charge_state_v<TestType>);

    TestType lyt{};

    using charge_layout = charge_distribution_surface<TestType>;
    CHECK(is_cell_level_layout_v<charge_layout>);
    CHECK(has_assign_charge_state_v<charge_layout>);
    CHECK(has_get_charge_state_v<charge_layout>);
    CHECK(has_foreach_charge_state_v<charge_layout>);

    const charge_layout defect_lyt{};
    const charge_layout charge_lyt_from_lyt{lyt};

    using charge_charge_layout = sidb_surface<charge_layout>;
    CHECK(is_cell_level_layout_v<charge_charge_layout>);
    CHECK(has_assign_charge_state_v<charge_charge_layout>);
    CHECK(has_get_charge_state_v<charge_charge_layout>);
    CHECK(has_foreach_charge_state_v<charge_charge_layout>);
}

TEMPLATE_TEST_CASE(
    "charge distribution surface traits (layer on cell_level_layout)", "[charge-distribution-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    REQUIRE(is_cell_level_layout_v<TestType>);
    CHECK(!has_assign_charge_state_v<TestType>);
    CHECK(!has_get_charge_state_v<TestType>);
    CHECK(!has_foreach_charge_state_v<TestType>);
    CHECK(!has_assign_sidb_defect_v<TestType>);
    CHECK(!has_get_sidb_defect_v<TestType>);
    CHECK(!has_foreach_sidb_defect_v<TestType>);

    TestType lyt{};
    using charge_layout = charge_distribution_surface<TestType>;
    CHECK(is_cell_level_layout_v<charge_layout>);
    CHECK(has_assign_charge_state_v<charge_layout>);
    CHECK(has_get_charge_state_v<charge_layout>);
    CHECK(has_foreach_charge_state_v<charge_layout>);
    CHECK(!has_assign_sidb_defect_v<TestType>);
    CHECK(!has_get_sidb_defect_v<TestType>);
    CHECK(!has_foreach_sidb_defect_v<TestType>);
}

TEMPLATE_TEST_CASE(
    "assign and delete charge states", "[charge-distribution-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>),

    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>>),
    (sidb_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>>))

{
    TestType                    lyt{{11, 9}};
    charge_distribution_surface charge_layout{lyt};

    SECTION("assign and read out charge states")
    {
        // assign SiDBs and charge states to three different cells and read the charge state
        charge_layout.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        charge_layout.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        charge_layout.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_layout.assign_charge_state({5, 4}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 6}, sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);

        // check if charge state for empty cell is 'NONE'
        CHECK(charge_layout.get_charge_state({5, 7}) == sidb_charge_state::NONE);
    }

    SECTION("assign different charge states to the same cell")
    {
        // check if the charge state can be overwritten
        charge_layout.assign_cell_type({5, 1}, TestType::cell_type::NORMAL);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NONE);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("assign different charge states to empty cell")
    {

        // check if charge state stays 'NONE' for empty cell after several charge state assignments
        charge_layout.assign_cell_type({5, 1}, TestType::cell_type::EMPTY);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NONE);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 1}, sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);
    }

    SECTION("overwrite an assigned charge state")
    {
        // assign SiDBs and charge states to three different cells
        charge_layout.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        charge_layout.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        charge_layout.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_layout.assign_charge_state({5, 4}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state({5, 6}, sidb_charge_state::NEGATIVE);

        // all SiDBs' charge states are set to positive
        charge_layout.foreach_charge_state(
            [&charge_layout](const auto& cd)
            { charge_layout.assign_charge_state(cd.first, sidb_charge_state::POSITIVE); });

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.foreach_charge_state(
            [&charge_layout](const auto& c)
            { CHECK(charge_layout.get_charge_state(c.first) == sidb_charge_state::POSITIVE); });

        // all SiDBs' charge states are set to neutral
        charge_layout.foreach_charge_state(
            [&charge_layout](const auto& cd)
            { charge_layout.assign_charge_state(cd.first, sidb_charge_state::NEUTRAL); });

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.foreach_charge_state(
            [&charge_layout](const auto& c)
            { CHECK(charge_layout.get_charge_state(c.first) == sidb_charge_state::NEUTRAL); });

        // all SiDBs' charge states are set to negative
        charge_layout.foreach_charge_state(
            [&charge_layout](const auto& cd)
            { charge_layout.assign_charge_state(cd.first, sidb_charge_state::NEGATIVE); });

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.foreach_charge_state(
            [&charge_layout](const auto& c)
            { CHECK(charge_layout.get_charge_state(c.first) == sidb_charge_state::NEGATIVE); });
    }
    SECTION("overwrite the charge state")
    {
        // assign SiDBs and charge states to three different cells
        charge_layout.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        charge_layout.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        charge_layout.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
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
}
