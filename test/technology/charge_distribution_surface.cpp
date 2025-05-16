//
// Created by Jan Drewniok on 05.12.22.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/constants.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <cstdint>

using namespace fiction;

TEMPLATE_TEST_CASE("Charge distribution surface traits and construction", "[charge-distribution-surface]",
                   sidb_cell_clk_lyt_siqad, sidb_cell_clk_lyt_cube, sidb_100_cell_clk_lyt_siqad,
                   sidb_100_cell_clk_lyt_cube, sidb_111_cell_clk_lyt_siqad, sidb_111_cell_clk_lyt_cube)
{
    REQUIRE(is_cell_level_layout_v<TestType>);
    CHECK(!has_assign_charge_state_v<TestType>);
    CHECK(!has_get_charge_state_v<TestType>);

    TestType lyt{};

    using charge_layout = charge_distribution_surface<TestType>;
    CHECK(is_charge_distribution_surface_v<charge_layout>);
    CHECK(is_cell_level_layout_v<charge_layout>);
    CHECK(has_assign_charge_state_v<charge_layout>);
    CHECK(has_get_charge_state_v<charge_layout>);

    const charge_layout charge_lyt{};
    const charge_layout charge_lyt_from_lyt{lyt};

    using charge_charge_layout = charge_distribution_surface<charge_layout>;
    CHECK(is_charge_distribution_surface_v<charge_charge_layout>);
    CHECK(is_cell_level_layout_v<charge_charge_layout>);
    CHECK(has_assign_charge_state_v<charge_charge_layout>);
    CHECK(has_get_charge_state_v<charge_charge_layout>);
}

TEMPLATE_TEST_CASE("Assign and delete charge states without defects", "[charge-distribution-surface]",
                   (sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>),
                   (sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>>))
{
    TestType lyt{};

    SECTION("cell to index and vs")
    {
        // assign SiDBs and charge states to three different cells and read the charge state
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        const charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
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
        charge_distribution_surface charge_layout{sidb_lattice{lyt}, sidb_simulation_parameters{}};
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

    SECTION("increase charge index of layout")
    {
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{2}};
        CHECK(charge_layout.get_charge_index_and_base().first == 0);
        charge_layout.increase_charge_index_by_one(dependent_cell_mode::FIXED, energy_calculation::UPDATE_ENERGY,
                                                   charge_distribution_history::NEGLECT);
        CHECK(charge_layout.get_charge_index_and_base().first == 1);
        CHECK(charge_layout.get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state_by_index(1) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state_by_index(2) == sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_index(0);
        CHECK(charge_layout.get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state_by_index(1) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state_by_index(2) == sidb_charge_state::NEGATIVE);

        charge_distribution_surface charge_layout_quickexact{lyt, sidb_simulation_parameters{2},
                                                             sidb_charge_state::NEGATIVE};
        charge_layout_quickexact.set_sidb_simulation_engine(sidb_simulation_engine::QUICKEXACT);
        charge_layout_quickexact.assign_dependent_cell({5, 4});
        charge_layout_quickexact.is_three_state_simulation_required();
        CHECK(charge_layout_quickexact.get_charge_index_and_base().first == 0);
        charge_layout_quickexact.increase_charge_index_by_one(
            dependent_cell_mode::FIXED, energy_calculation::UPDATE_ENERGY, charge_distribution_history::NEGLECT);
        CHECK(charge_layout_quickexact.get_charge_index_and_base().first == 1);
        CHECK(charge_layout_quickexact.get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_quickexact.get_charge_state_by_index(1) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_quickexact.get_charge_state_by_index(2) == sidb_charge_state::NEUTRAL);
    }

    SECTION("charge distribution defined by a given charge index and vs and dependent-cell")
    {
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 5}, TestType::cell_type::NORMAL);
        // assign dependent-cell at {5,5}
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{3, -0.25},
                                                  sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({5, 5});
        CHECK(charge_layout.get_max_charge_index() == 8);
        CHECK(charge_layout.get_max_charge_index_sub_layout() == 0);

        CHECK(charge_layout.get_charge_index_and_base().first == 0);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 0);

        // check if three state simulations are required.
        charge_layout.is_three_state_simulation_required();

        // all SiDBs of the layout can be positively charged.
        CHECK(charge_layout.get_max_charge_index() == 0);
        // maximal charge index of the sublayout (i.e., SiDBs that can be positively charged).
        CHECK(charge_layout.get_max_charge_index_sub_layout() == 8);

        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({6, 5}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({7, 5}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 8);

        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::POSITIVE);

        charge_layout.assign_charge_index(0);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::NEGATIVE);

        const auto system_energy_maximum = charge_layout.get_electrostatic_potential_energy();

        charge_layout.update_after_charge_change(dependent_cell_mode::VARIABLE,
                                                 energy_calculation::KEEP_OLD_ENERGY_VALUE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_layout.get_electrostatic_potential_energy() - system_energy_maximum,
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        // update energy and dependent cell is variable with respect to its charge state
        charge_layout.update_after_charge_change(dependent_cell_mode::VARIABLE, energy_calculation::UPDATE_ENERGY);
        CHECK(charge_layout.get_electrostatic_potential_energy() < system_energy_maximum);

        // assign charge states to dependent cell and check that charge index does not change since dependent-cell is
        // detached.
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_index_and_base().first == 0);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 0);

        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_index_and_base().first == 0);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 0);

        // assign charge state to cells, but not to the dependent cell
        charge_layout.assign_charge_state({6, 5}, sidb_charge_state::POSITIVE, charge_index_mode::KEEP_CHARGE_INDEX);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::POSITIVE);

        CHECK(charge_layout.get_charge_index_and_base().first == 0);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 0);

        charge_layout.assign_charge_state({6, 5}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 6);

        charge_layout.assign_charge_state({7, 5}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_index_and_base().first == charge_layout.get_max_charge_index());
        charge_layout.update_after_charge_change();
        CHECK_THAT(charge_layout.get_electrostatic_potential_energy() - system_energy_maximum,
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        // change charge state of the dependent-cell and check if system energy is reduced
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::NEGATIVE);
        charge_layout.update_after_charge_change();
        CHECK(charge_layout.get_electrostatic_potential_energy() < system_energy_maximum);

        CHECK(charge_layout.get_charge_index_of_sub_layout() == 8);
        charge_layout.set_sidb_simulation_engine(sidb_simulation_engine::EXGS);
        charge_layout.reset_charge_index_sub_layout();
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 0);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("cover behavior of dependent_cell in and outside the sublayout (positively charged SiDBs)")
    {
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 8}, TestType::cell_type::NORMAL);

        // dependent-cell is within the sublayout
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}, sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({5, 5});
        CHECK(charge_layout.is_three_state_simulation_required());
        charge_layout.assign_charge_state({6, 5}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({7, 5}, sidb_charge_state::POSITIVE);
        charge_layout.charge_distribution_to_index();
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 8);

        // dependent-cell is not within the sublayout
        charge_distribution_surface charge_layout_dependent_cell_not_in_sublayout{lyt, sidb_simulation_parameters{},
                                                                                  sidb_charge_state::NEGATIVE};
        charge_layout_dependent_cell_not_in_sublayout.assign_dependent_cell({10, 4});
        CHECK(charge_layout_dependent_cell_not_in_sublayout.is_three_state_simulation_required());
        charge_layout_dependent_cell_not_in_sublayout.assign_charge_state({5, 5}, sidb_charge_state::POSITIVE);
        charge_layout_dependent_cell_not_in_sublayout.assign_charge_state({6, 5}, sidb_charge_state::POSITIVE);
        charge_layout_dependent_cell_not_in_sublayout.assign_charge_state({7, 5}, sidb_charge_state::POSITIVE);
        charge_layout_dependent_cell_not_in_sublayout.assign_charge_state({10, 8}, sidb_charge_state::NEUTRAL);
        charge_layout_dependent_cell_not_in_sublayout.charge_distribution_to_index();
        CHECK(charge_layout_dependent_cell_not_in_sublayout.get_charge_index_and_base().first == 8);
        CHECK(charge_layout_dependent_cell_not_in_sublayout.get_charge_index_of_sub_layout() == 26);
    }

    SECTION("charge index of layout and sublayout (positively charged SiDBs) without dependent-cell")
    {
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 4}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}, sidb_charge_state::NEGATIVE};
        CHECK(charge_layout.is_three_state_simulation_required());
        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({6, 5}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({7, 5}, sidb_charge_state::POSITIVE);
        charge_layout.charge_distribution_to_index();
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 26);
        CHECK(charge_layout.get_charge_index_and_base().first == 0);
    }

    SECTION("charge index to charge distribution")
    {
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 4}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}, sidb_charge_state::NEGATIVE};
        charge_layout.set_sidb_simulation_engine(sidb_simulation_engine::QUICKEXACT);

        charge_layout.is_three_state_simulation_required();
        CHECK(charge_layout.get_max_charge_index_sub_layout() == 26);

        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({10, 4}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 0);
        CHECK(charge_layout.get_charge_index_and_base().first == 0);

        charge_layout.assign_charge_state({5, 5}, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state({10, 4}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({10, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 18);
        CHECK(charge_layout.get_charge_index_and_base().first == 16);

        charge_layout.assign_charge_index(0);

        CHECK(charge_layout.get_charge_index_of_sub_layout() == 0);
        CHECK(charge_layout.get_charge_index_and_base().first == 0);

        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({10, 4}) == sidb_charge_state::NEGATIVE);

        charge_layout.increase_charge_index_of_sub_layout_by_one();
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 1);
        charge_layout.assign_charge_index(1);
        CHECK(charge_layout.get_charge_index_and_base().first == 1);

        charge_layout.increase_charge_index_of_sub_layout_by_one();
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 2);

        charge_layout.set_sidb_simulation_engine(sidb_simulation_engine::EXGS);
        charge_layout.increase_charge_index_of_sub_layout_by_one(
            dependent_cell_mode::FIXED, energy_calculation::UPDATE_ENERGY, charge_distribution_history::NEGLECT);
        CHECK(charge_layout.get_charge_index_of_sub_layout() == 3);

        // set the charge index to zero and thereby, all siDBs to negatively charged.
        charge_layout.assign_charge_index(0);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({10, 4}) == sidb_charge_state::NEGATIVE);

        charge_layout.assign_charge_index(charge_layout.get_max_charge_index());
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({6, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({7, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({10, 4}) == sidb_charge_state::POSITIVE);
    }

    SECTION("three_state_cell_to_index, two_state_cell_to_index")
    {
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 4}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}, sidb_charge_state::NEGATIVE};
        charge_layout.is_three_state_simulation_required();

        CHECK(charge_layout.three_state_cell_to_index({5, 5}) == 0);
        CHECK(charge_layout.three_state_cell_to_index({6, 5}) == 1);
        CHECK(charge_layout.three_state_cell_to_index({7, 5}) == 2);
        CHECK(charge_layout.index_to_three_state_cell(1) == cell<TestType>(6, 5));
        CHECK(charge_layout.index_to_three_state_cell(4) == cell<TestType>());

        CHECK(charge_layout.two_state_cell_to_index({10, 4}) == 0);
        CHECK(charge_layout.two_state_cell_to_index({10, 5}) == -1);
        CHECK(charge_layout.index_to_two_state_cell(0) == cell<TestType>(10, 4));
        CHECK(charge_layout.index_to_two_state_cell(1) == cell<TestType>());
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

        charge_layout.assign_charge_state_by_index(0, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state_by_index(1, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state_by_index(2, sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state_by_index(1) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state_by_index(2) == sidb_charge_state::NEGATIVE);

        CHECK(charge_layout.get_charge_state({7, 6}) == sidb_charge_state::NONE);

        charge_layout.assign_charge_state_by_index(0, sidb_charge_state::NEUTRAL);
        charge_layout.assign_charge_state_by_index(1, sidb_charge_state::POSITIVE);
        charge_layout.assign_charge_state_by_index(2, sidb_charge_state::POSITIVE);
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
        charge_layout.assign_all_charge_states(sidb_charge_state::POSITIVE);

        // calculate potential between two sidbs (charge sign not included)
        CHECK(charge_layout.calculate_chargeless_potential_between_sidbs({5, 4}, {5, 5}) > 0.0);
        CHECK_THAT(charge_layout.calculate_chargeless_potential_between_sidbs({5, 4}, {5, 4}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK(charge_layout.calculate_chargeless_potential_between_sidbs({5, 4}, {5, 6}) > 0);
        CHECK(charge_layout.calculate_chargeless_potential_between_sidbs({5, 5}, {5, 6}) > 0);
        CHECK_THAT(charge_layout.calculate_chargeless_potential_between_sidbs({5, 6}, {5, 5}) -
                       charge_layout.calculate_chargeless_potential_between_sidbs({5, 5}, {5, 6}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.assign_all_charge_states(sidb_charge_state::POSITIVE);

        // all SiDBs' charge states are set to neutral
        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        // all SiDBs' charge states are set to negative
        charge_layout.assign_all_charge_states(sidb_charge_state::NEGATIVE);

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.assign_all_charge_states(sidb_charge_state::NEGATIVE);
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

    SECTION("set physical simulation parameters")
    {
        // assign SiDBs and charge states to three different cells
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        charge_layout.assign_physical_parameters(sidb_simulation_parameters{2, -0.2});
        CHECK(charge_layout.get_simulation_params().base == 2);
        CHECK(charge_layout.get_simulation_params().mu_minus == -0.2);
        CHECK(charge_layout.get_simulation_params().epsilon_r == 5.6);
        CHECK(charge_layout.get_simulation_params().lambda_tf == 5.0);

        charge_layout.assign_physical_parameters(sidb_simulation_parameters{3, -0.4, 5.1, 5.5});
        CHECK(charge_layout.get_simulation_params().base == 3);
        CHECK(charge_layout.get_simulation_params().mu_minus == -0.4);
        CHECK(charge_layout.get_simulation_params().epsilon_r == 5.1);
        CHECK(charge_layout.get_simulation_params().lambda_tf == 5.5);
    }

    SECTION("Distance matrix")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 1}, TestType::cell_type::NORMAL);

        const charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        // Take cells that are not part of the layout
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({3, 0, 0}, {3, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({0, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({0, 0, 0}, {1, 0, 0}),
                   Catch::Matchers::WithinAbs((sidb_100_lattice::LAT_A * 0.1), constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({1, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs((sidb_100_lattice::LAT_A * 0.1), constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({1, 0, 0}, {1, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(
            charge_layout.get_nm_distance_between_sidbs({0, 0, 0}, {1, 1, 1}),
            Catch::Matchers::WithinAbs(std::hypot(sidb_100_lattice::LAT_A * 0.1,
                                                  sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1),
                                       constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({1, 1, 1}, {1, 1, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("Potential matrix")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 8, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 10, 1}, TestType::cell_type::NORMAL);

        const charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({2, 8, 0}, {2, 10, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({0, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 8, 0}, {1, 8, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 10, 1}, {1, 10, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 8, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.01219448987342317, constants::ERROR_MARGIN));
        CHECK_THAT(std::abs(charge_layout.get_chargeless_potential_between_sidbs({0, 0, 0}, {1, 10, 1}) -
                            charge_layout.get_chargeless_potential_between_sidbs({1, 10, 1}, {0, 0, 0})),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

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

        charge_layout.update_local_internal_potential();

        // cell and index are not part of the layout.
        CHECK(!charge_layout.get_local_potential({2, 0, 0}).has_value());
        CHECK(!charge_layout.get_local_potential_by_index(4).has_value());

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK(p.value() > 0.0);
            });

        charge_layout.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({1, 8, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({1, 10, 1}, sidb_charge_state::NEGATIVE);

        charge_layout.update_local_internal_potential();

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK(p.value() < 0.0);
            });

        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        charge_layout.update_local_internal_potential();

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK_THAT(p.value(), Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
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
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK(charge_layout.get_electrostatic_potential_energy() > 0.0);

        // system energy is zero when all SiDBs are neutrally charged.
        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK_THAT(charge_layout.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        // system energy is zero when all SiDBs are positively charged.
        charge_layout.assign_all_charge_states(sidb_charge_state::POSITIVE);
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK(charge_layout.get_electrostatic_potential_energy() > 0.0);
    }

    SECTION("Physical validity check under different physical parameters")
    {
        TestType layout{};
        layout.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({4, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{layout, sidb_simulation_parameters{}};
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({0, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({4, 1, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(charge_layout.is_physically_valid());

        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout.update_after_charge_change();

        CHECK(!charge_layout.is_physically_valid());

        charge_layout.assign_physical_parameters(sidb_simulation_parameters{2, 0.0});
        CHECK(charge_layout.is_physically_valid());
    }

    SECTION("Physical validity check, far distance of SiDBs, all NEGATIVE")
    {
        TestType layout{};
        layout.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({4, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_five{layout, sidb_simulation_parameters{}};
        CHECK(charge_layout_five.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_five.get_charge_state({0, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_five.get_charge_state({4, 1, 1}) == sidb_charge_state::NEGATIVE);

        charge_layout_five.update_local_internal_potential();
        charge_layout_five.recompute_electrostatic_potential_energy();
        charge_layout_five.validity_check();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 0);

        charge_layout_five.assign_charge_state({4, 1, 1}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout_five.get_charge_state({4, 1, 1}) == sidb_charge_state::POSITIVE);
        charge_layout_five.charge_distribution_to_index();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 6);

        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 7);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 8);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 9);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 10);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 11);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 12);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 13);

        CHECK(charge_layout_five.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout_five.get_charge_state({0, 2, 0}) == sidb_charge_state::NEUTRAL);
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
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        charge_layout.validity_check();
        CHECK(!charge_layout.is_physically_valid());
    }

    SECTION("apply external voltage at two cells")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};
        charge_layout_new.assign_local_external_potential({{{0, 0, 1}, -0.5}});
        REQUIRE(!charge_layout_new.get_local_external_potential_map().empty());
        CHECK(charge_layout_new.get_local_external_potential_map().size() == 1);
        CHECK(charge_layout_new.get_local_external_potential_map().size() == 1);

        REQUIRE(charge_layout_new.get_local_potential({0, 0, 1}).has_value());
        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 1}).value() + 0.5,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        CHECK_THAT(charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        REQUIRE(charge_layout_new.get_local_potential({10, 5, 1}).has_value());
        CHECK_THAT(charge_layout_new.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        charge_layout_new.assign_all_charge_states(sidb_charge_state::POSITIVE);
        charge_layout_new.update_after_charge_change();
        CHECK(charge_layout_new.get_charge_state({0, 0, 1}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({10, 5, 1}) == sidb_charge_state::POSITIVE);

        CHECK(charge_layout_new.get_local_potential({0, 0, 1}).value() > -0.5);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() > -0.5);
        CHECK(charge_layout_new.get_local_potential({10, 5, 1}).value() > -0.5);

        charge_layout_new.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout_new.update_after_charge_change();

        charge_layout_new.reset_local_external_potentials();
        charge_layout_new.assign_local_external_potential({{{0, 0, 1}, -0.1}});
        REQUIRE(charge_layout_new.get_local_potential({0, 0, 1}).has_value());
        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 1}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        charge_layout_new.assign_local_external_potential({{{0, 0, 1}, -0.5}, {{10, 5, 1}, -0.1}});
        charge_layout_new.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();

        CHECK(charge_layout_new.get_local_potential({0, 0, 1}).value() < -0.5);
        CHECK(charge_layout_new.get_local_potential({10, 5, 1}).value() < -0.1);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() < 0);
    }

    SECTION("apply homogenous external voltage to layout")
    {
        const sidb_simulation_parameters params{3, -0.32};
        lyt.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEUTRAL};
        CHECK(charge_layout.get_local_external_potential_map().empty());
        charge_layout.assign_global_external_potential(-0.1);
        CHECK(!charge_layout.get_local_external_potential_map().empty());

        REQUIRE(charge_layout.get_local_potential({0, 0, 1}).has_value());
        CHECK_THAT(charge_layout.get_local_potential({0, 0, 1}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        REQUIRE(charge_layout.get_local_potential({1, 3, 0}).has_value());
        CHECK_THAT(charge_layout.get_local_potential({1, 3, 0}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        REQUIRE(charge_layout.get_local_potential({10, 5, 1}).has_value());
        CHECK_THAT(charge_layout.get_local_potential({10, 5, 1}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));

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

        charge_layout_new.assign_global_external_potential(0.2);
        charge_layout_new.validity_check();
        CHECK(!charge_layout_new.is_physically_valid());

        charge_layout_new.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({3, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({5, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();
        CHECK(charge_layout_new.is_physically_valid());
    }

    SECTION("no external voltage given")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        const charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};

        REQUIRE(charge_layout_new.get_local_potential({0, 0, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 5, 1}).has_value());

        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("increase charge index")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params};
        const auto                  negative_sidbs = charge_layout_new.negative_sidb_detection();
        REQUIRE(negative_sidbs.size() == 3);
        CHECK(charge_layout_new.get_charge_index_and_base().first == 0);

        charge_layout_new.assign_all_charge_states(sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({0, 0, 1}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({10, 5, 1}) == sidb_charge_state::POSITIVE);

        charge_layout_new.charge_distribution_to_index();
        CHECK(charge_layout_new.get_charge_index_and_base().first == 26);

        charge_layout_new.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        charge_layout_new.charge_distribution_to_index();
        CHECK(charge_layout_new.get_charge_index_and_base().first == 13);

        charge_layout_new.increase_charge_index_by_one();
        charge_layout_new.charge_distribution_to_index();
        CHECK(charge_layout_new.get_charge_index_and_base().first == 14);
        charge_layout_new.increase_charge_index_by_one();
        CHECK(charge_layout_new.get_charge_index_and_base().first == 15);
    }

    SECTION("detecting perturber in layout with only one SiDB")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

        const charge_distribution_surface charge_layout_new{lyt_new, params};
        const auto                        negative_sidbs = charge_layout_new.negative_sidb_detection();
        REQUIRE(negative_sidbs.size() == 1);
    }

    SECTION("Seven randomly distributed DB | checking for physical validity")
    {
        TestType lyt_new{};
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

    SECTION("Test several getters")
    {
        TestType lyt_new{};
        lyt.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);
        const sidb_simulation_parameters params{2, -0.28};
        charge_distribution_surface      charge_layout{lyt, params};
        const auto                       location_in_nm = charge_layout.get_all_sidb_locations_in_nm();
        REQUIRE(location_in_nm.size() == charge_layout.num_cells());

        const auto all_charges = charge_layout.get_all_sidb_charges();
        REQUIRE(all_charges.size() == charge_layout.num_cells());

        charge_layout.assign_electrostatic_potential_energy_to_zero();
        CHECK(charge_layout.get_electrostatic_potential_energy() == 0.0);

        CHECK(charge_layout.three_state_cell_to_index({0, 0, 0}) == -1);
    }

    SECTION("using chargeless and normal potential function")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32, 5.0};

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

        CHECK_THAT(charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {1, 3, 0}) -
                       charge_layout_new.calculate_chargeless_potential_between_sidbs({0, 0, 1}, {1, 3, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 0}, {0, 0, 1}) == 0.0);
        CHECK(charge_layout_new.get_potential_between_sidbs({0, 0, 0}, {0, 0, 1}) == 0.0);

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {0, 0, 1}) == 0.0);
        CHECK(charge_layout_new.get_potential_between_sidbs({0, 0, 1}, {0, 0, 1}) == 0.0);

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {1, 3, 0}) > 0.0);
        CHECK(charge_layout_new.get_potential_between_sidbs({0, 0, 1}, {1, 3, 0}) > 0.0);

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {10, 5, 1}) > 0.0);
        CHECK_THAT(charge_layout_new.get_potential_between_sidbs({0, 0, 1}, {10, 5, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        CHECK(charge_layout_new.get_chargeless_potential_between_sidbs({10, 5, 1}, {0, 0, 1}) > 0.0);
        CHECK(charge_layout_new.get_potential_between_sidbs({10, 5, 1}, {0, 0, 1}) < 0.0);

        CHECK_THAT(charge_layout_new.get_potential_between_sidbs({10, 5, 1}, {0, 0, 1}) +
                       charge_layout_new.get_chargeless_potential_between_sidbs({10, 5, 1}, {0, 0, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        CHECK_THAT(charge_layout_new.get_potential_between_sidbs({0, 0, 1}, {1, 3, 0}) -
                       charge_layout_new.get_chargeless_potential_between_sidbs({0, 0, 1}, {1, 3, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("adding dependent cell")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 3, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 4, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEGATIVE};
        charge_layout_new.assign_dependent_cell({10, 4, 1});

        charge_layout_new.assign_charge_state({0, 3, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);

        CHECK(charge_layout_new.get_charge_state({0, 3, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEUTRAL);
        charge_layout_new.update_after_charge_change();

        REQUIRE(charge_layout_new.get_local_potential({0, 3, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 4, 1}).has_value());

        CHECK(charge_layout_new.get_local_potential({0, 3, 1}).value() < 0);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() < 0);
        CHECK(charge_layout_new.get_local_potential({10, 4, 1}).value() < 0);
        charge_layout_new.update_charge_state_of_dependent_cell();
        CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("adding dependent cell and increase index")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{2, -0.32};

        lyt_new.assign_cell_type({0, 3, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 4, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEGATIVE};
        charge_layout_new.assign_dependent_cell({10, 4, 1});

        REQUIRE(charge_layout_new.get_local_potential({0, 3, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 4, 1}).has_value());

        CHECK(charge_layout_new.get_local_potential({0, 3, 1}).value() < 0);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() < 0);
        CHECK(charge_layout_new.get_local_potential({10, 4, 1}).value() < 0);

        for (uint64_t i = 0; i < 3; i++)
        {
            charge_layout_new.increase_charge_index_by_one();
            CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEGATIVE);
        }
    }

    SECTION("dependent cell in alignment with the base number")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{2, -0.32};

        lyt_new.assign_cell_type({0, 1, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({0, 1, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({2, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEGATIVE};
        charge_layout_new.assign_dependent_cell({0, 1, 1});

        charge_layout_new.assign_charge_state({0, 1, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({0, 1, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({2, 1, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();

        charge_layout_new.update_charge_state_of_dependent_cell();
        CHECK(charge_layout_new.get_charge_state({0, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({0, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({2, 1, 1}) == sidb_charge_state::NEGATIVE);

        charge_layout_new.assign_base_number(3);
        charge_layout_new.assign_charge_state({0, 1, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({0, 1, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({2, 1, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change(dependent_cell_mode::VARIABLE);
        CHECK(charge_layout_new.get_charge_state({0, 1, 1}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({0, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({2, 1, 1}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("adding dependent cell and compare local potential and system energy")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{2, -0.32};

        lyt_new.assign_cell_type({0, 3, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 4, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEGATIVE};
        charge_layout_new.assign_dependent_cell({10, 4, 1});

        charge_layout_new.assign_charge_state({0, 3, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({10, 4, 1}, sidb_charge_state::NEUTRAL);
        charge_layout_new.update_after_charge_change();

        REQUIRE(charge_layout_new.get_local_potential({0, 3, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 4, 1}).has_value());

        CHECK(charge_layout_new.get_local_potential({0, 3, 1}).value() == 0.0);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() < 0);
        CHECK(charge_layout_new.get_local_potential({10, 4, 1}).value() < 0);

        charge_layout_new.update_charge_state_of_dependent_cell();
        CHECK(charge_layout_new.get_charge_state({0, 3, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout_new.get_charge_state({10, 4, 1}) == sidb_charge_state::NEGATIVE);
        const auto loc_one   = charge_layout_new.get_local_potential({0, 3, 1}).value();
        const auto loc_two   = charge_layout_new.get_local_potential({1, 3, 0}).value();
        const auto loc_three = charge_layout_new.get_local_potential({10, 4, 1}).value();
        CHECK(loc_one < 0);
        CHECK(loc_two < 0);
        CHECK(loc_three < 0);
        charge_layout_new.recompute_electrostatic_potential_energy();
        auto system_energy_first = charge_layout_new.get_electrostatic_potential_energy();

        charge_layout_new.assign_charge_state({0, 3, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({1, 3, 0}, sidb_charge_state::NEUTRAL);
        charge_layout_new.assign_charge_state({10, 4, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();
        CHECK_THAT(loc_one - charge_layout_new.get_local_potential({0, 3, 1}).value(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(loc_two - charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(loc_three - charge_layout_new.get_local_potential({10, 4, 1}).value(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        auto system_energy_second = charge_layout_new.get_electrostatic_potential_energy();
        CHECK_THAT(system_energy_first - system_energy_second, Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
    }

    SECTION("Physical validity check after dependent cell is updated")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}, sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({3, 0, 0});
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(!charge_layout.is_physically_valid());

        charge_layout.update_charge_state_of_dependent_cell();
        charge_layout.update_after_charge_change(dependent_cell_mode::VARIABLE);
        charge_layout.validity_check();
        CHECK(charge_layout.is_physically_valid());
    }

    SECTION("check charge index")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({3, 0, 0});
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_index_and_base().first == 0);

        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index_and_base().first == 1);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index_and_base().first == 2);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index_and_base().first == 3);

        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_index_and_base().first == 3);
    }

    SECTION("Physical validity check after dependent cell is updated")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}, sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({3, 0, 0});
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(!charge_layout.is_physically_valid());

        charge_layout.update_charge_state_of_dependent_cell();
        charge_layout.update_after_charge_change(dependent_cell_mode::VARIABLE);
        charge_layout.validity_check();
        CHECK(charge_layout.is_physically_valid());
    }

    SECTION("check charge index")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({0, 0, 0});
        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_index_and_base().first == 0);

        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index_and_base().first == 1);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index_and_base().first == 2);
        charge_layout.increase_charge_index_by_one();
        CHECK(charge_layout.get_charge_index_and_base().first == 3);

        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_index_and_base().first == 3);
    }

    SECTION("Y-shaped SiDB OR gate with input 01 and global external potential (high)")
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
        charge_lyt_first.assign_global_external_potential(-2.0);
        charge_lyt_first.validity_check();
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
        charge_lyt_first.is_three_state_simulation_required();
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
        CHECK(loop_counter_third == 1);
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

TEMPLATE_TEST_CASE("Assign and delete charge states without defects, part one", "[charge-distribution-surface]",
                   (sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>>))

{
    TestType lyt{};

    SECTION("Assign charged defect")
    {
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.add_sidb_defect_to_potential_landscape(
            {5, 6}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout.get_simulation_params().epsilon_r,
                                charge_layout.get_simulation_params().lambda_tf});
    }

    SECTION("Assign neutral defect")
    {
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        charge_layout.add_sidb_defect_to_potential_landscape(
            {5, 6}, sidb_defect{sidb_defect_type::UNKNOWN, 0, charge_layout.get_simulation_params().epsilon_r,
                                charge_layout.get_simulation_params().lambda_tf});

        REQUIRE(charge_layout.get_local_potential({5, 4}).has_value());
        CHECK_THAT(charge_layout.get_local_potential({5, 4}).value(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
    }

    SECTION("perturber is replaced by an equivalent defect")
    {
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);
        const charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};
        lyt.assign_cell_type({5, 6}, TestType::cell_type::EMPTY);
        charge_distribution_surface charge_layout_new{lyt, sidb_simulation_parameters{}};
        charge_layout_new.add_sidb_defect_to_potential_landscape(
            {5, 6}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout_new.get_simulation_params().epsilon_r,
                                charge_layout_new.get_simulation_params().lambda_tf});
        CHECK_THAT(charge_layout_new.chargeless_potential_generated_by_defect_at_given_distance(0.0),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        charge_layout_new.update_after_charge_change();
        CHECK_THAT(charge_layout.get_electrostatic_potential_energy() -
                       charge_layout_new.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
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
        charge_layout.assign_all_charge_states(sidb_charge_state::POSITIVE);

        // calculate potential between two sidbs (charge sign not included)
        CHECK(charge_layout.calculate_chargeless_potential_between_sidbs({5, 4}, {5, 5}) > 0.0);
        CHECK_THAT(charge_layout.calculate_chargeless_potential_between_sidbs({5, 4}, {5, 4}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK(charge_layout.calculate_chargeless_potential_between_sidbs({5, 4}, {5, 6}) > 0);
        CHECK(charge_layout.calculate_chargeless_potential_between_sidbs({5, 5}, {5, 6}) > 0);
        CHECK_THAT(charge_layout.calculate_chargeless_potential_between_sidbs({5, 6}, {5, 5}) -
                       charge_layout.calculate_chargeless_potential_between_sidbs({5, 5}, {5, 6}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.assign_all_charge_states(sidb_charge_state::POSITIVE);

        // all SiDBs' charge states are set to neutral
        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        // all SiDBs' charge states are set to negative
        charge_layout.assign_all_charge_states(sidb_charge_state::NEGATIVE);

        // read SiDBs' charge states
        CHECK(charge_layout.get_charge_state({5, 4}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 5}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 6}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 1}) == sidb_charge_state::NONE);

        charge_layout.assign_all_charge_states(sidb_charge_state::NEGATIVE);
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

    SECTION("set physical simulation parameters")
    {
        // assign SiDBs and charge states to three different cells
        lyt.assign_cell_type({5, 4}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 5}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        charge_layout.assign_physical_parameters(sidb_simulation_parameters{2, -0.2});
        CHECK(charge_layout.get_simulation_params().base == 2);
        CHECK(charge_layout.get_simulation_params().mu_minus == -0.2);
        CHECK(charge_layout.get_simulation_params().epsilon_r == 5.6);
        CHECK(charge_layout.get_simulation_params().lambda_tf == 5.0);

        charge_layout.assign_physical_parameters(sidb_simulation_parameters{3, -0.4, 5.1, 5.5});
        CHECK(charge_layout.get_simulation_params().base == 3);
        CHECK(charge_layout.get_simulation_params().mu_minus == -0.4);
        CHECK(charge_layout.get_simulation_params().epsilon_r == 5.1);
        CHECK(charge_layout.get_simulation_params().lambda_tf == 5.5);
    }

    SECTION("Distance matrix")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        // Take cells that are not part of the layout
        CHECK(charge_layout.get_nm_distance_between_sidbs({3, 0, 0}, {3, 0, 0}) == 0.0);

        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({0, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({0, 0, 0}, {1, 0, 0}),
                   Catch::Matchers::WithinAbs((sidb_100_lattice::LAT_A * 0.1), constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({1, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs((sidb_100_lattice::LAT_A * 0.1), constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({1, 0, 0}, {1, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(
            charge_layout.get_nm_distance_between_sidbs({0, 0, 0}, {1, 1, 1}),
            Catch::Matchers::WithinAbs(std::hypot(sidb_100_lattice::LAT_A * 0.1,
                                                  sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1),
                                       constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({1, 1, 1}, {1, 1, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("Potential matrix")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 8, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 10, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({2, 8, 0}, {2, 10, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({0, 0, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 8, 0}, {1, 8, 0}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 10, 1}, {1, 10, 1}),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_chargeless_potential_between_sidbs({1, 8, 0}, {0, 0, 0}),
                   Catch::Matchers::WithinAbs(0.01219448987342317, constants::ERROR_MARGIN));
        CHECK_THAT(std::abs(charge_layout.get_chargeless_potential_between_sidbs({0, 0, 0}, {1, 10, 1}) -
                            charge_layout.get_chargeless_potential_between_sidbs({1, 10, 1}, {0, 0, 0})),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

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

        charge_layout.update_local_internal_potential();

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({1, 8, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({1, 10, 1}).has_value());

        // cell and index are not part of the layout.
        CHECK(!charge_layout.get_local_potential({2, 0, 0}).has_value());
        CHECK(!charge_layout.get_local_potential_by_index(4).has_value());

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

        charge_layout.update_local_internal_potential();

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK(p.value() < 0.0);
            });

        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);

        charge_layout.update_local_internal_potential();

        charge_layout.foreach_cell(
            [&charge_layout](const auto& c)
            {
                const auto p = charge_layout.get_local_potential(c);
                REQUIRE(p.has_value());
                CHECK_THAT(p.value(), Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
            });
    }
}

TEMPLATE_TEST_CASE("Assign and delete charge states without defects, part two", "[charge-distribution-surface]",
                   (sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>>))

{
    TestType lyt{};

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
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK(charge_layout.get_electrostatic_potential_energy() > 0.0);

        // system energy is zero when all SiDBs are neutrally charged.
        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK_THAT(charge_layout.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        // system energy is zero when all SiDBs are positively charged.
        charge_layout.assign_all_charge_states(sidb_charge_state::POSITIVE);
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK(charge_layout.get_electrostatic_potential_energy() > 0.0);
    }

    SECTION("Physical validity check, far distance of SIDBs, all NEGATIVE")
    {

        TestType layout{};
        layout.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);
        layout.assign_cell_type({4, 1, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_five{layout, sidb_simulation_parameters{}};
        CHECK(charge_layout_five.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_five.get_charge_state({0, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout_five.get_charge_state({4, 1, 1}) == sidb_charge_state::NEGATIVE);

        charge_layout_five.update_local_internal_potential();
        charge_layout_five.recompute_electrostatic_potential_energy();
        charge_layout_five.validity_check();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 0);

        charge_layout_five.assign_charge_state({4, 1, 1}, sidb_charge_state::POSITIVE);
        CHECK(charge_layout_five.get_charge_state({4, 1, 1}) == sidb_charge_state::POSITIVE);
        charge_layout_five.charge_distribution_to_index();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 6);

        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 7);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 8);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 9);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 10);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 11);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 12);
        charge_layout_five.increase_charge_index_by_one();
        CHECK(charge_layout_five.get_charge_index_and_base().first == 13);

        CHECK(charge_layout_five.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout_five.get_charge_state({0, 2, 0}) == sidb_charge_state::NEUTRAL);
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
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        charge_layout.validity_check();
        CHECK(!charge_layout.is_physically_valid());
    }

    SECTION("apply external voltage at two cells")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};
        charge_layout_new.assign_local_external_potential({{{0, 0, 1}, -0.5}});
        REQUIRE(!charge_layout_new.get_local_external_potential_map().empty());
        CHECK(charge_layout_new.get_local_external_potential_map().size() == 1);
        CHECK(charge_layout_new.get_local_external_potential_map().size() == 1);

        REQUIRE(charge_layout_new.get_local_potential({0, 0, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 5, 1}).has_value());

        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 1}).value() + 0.5,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        charge_layout_new.assign_all_charge_states(sidb_charge_state::POSITIVE);
        charge_layout_new.update_after_charge_change();
        CHECK(charge_layout_new.get_charge_state({0, 0, 1}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({1, 3, 0}) == sidb_charge_state::POSITIVE);
        CHECK(charge_layout_new.get_charge_state({10, 5, 1}) == sidb_charge_state::POSITIVE);

        CHECK(charge_layout_new.get_local_potential({0, 0, 1}).value() > -0.5);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() > -0.5);
        CHECK(charge_layout_new.get_local_potential({10, 5, 1}).value() > -0.5);

        charge_layout_new.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout_new.update_after_charge_change();

        charge_layout_new.assign_local_external_potential({{{0, 0, 1}, -0.1}});
        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 1}).value() + 0.6,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        charge_layout_new.reset_local_external_potentials();
        charge_layout_new.assign_local_external_potential({{{0, 0, 1}, -0.1}});
        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 1}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        charge_layout_new.reset_local_external_potentials();
        charge_layout_new.assign_local_external_potential({{{0, 0, 1}, -0.5}, {{10, 5, 1}, -0.1}});
        charge_layout_new.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();

        CHECK(charge_layout_new.get_local_potential({0, 0, 1}).value() < -0.5);
        CHECK(charge_layout_new.get_local_potential({10, 5, 1}).value() < -0.1);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() < 0);
    }

    SECTION("apply homogenous external voltage to layout")
    {
        const sidb_simulation_parameters params{3, -0.32};
        lyt.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEUTRAL};
        CHECK(charge_layout.get_local_external_potential_map().empty());
        charge_layout.assign_global_external_potential(-0.1);
        CHECK(!charge_layout.get_local_external_potential_map().empty());

        REQUIRE(charge_layout.get_local_potential({0, 0, 1}).has_value());
        REQUIRE(charge_layout.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({10, 5, 1}).has_value());

        CHECK_THAT(charge_layout.get_local_potential({0, 0, 1}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_local_potential({1, 3, 0}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_local_potential({10, 5, 1}).value() + 0.1,
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));

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

        charge_layout_new.assign_global_external_potential(0.2);
        charge_layout_new.validity_check();
        CHECK(!charge_layout_new.is_physically_valid());

        charge_layout_new.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({3, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.assign_charge_state({5, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();
        CHECK(charge_layout_new.is_physically_valid());
    }

    SECTION("no external voltage given")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};

        REQUIRE(charge_layout_new.get_local_potential({0, 0, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 5, 1}).has_value());

        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({1, 3, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout_new.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("assign defect | negative defect")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};

        charge_layout_new.add_sidb_defect_to_potential_landscape(
            {5, 1, 1}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout_new.get_simulation_params().epsilon_r,
                                   charge_layout_new.get_simulation_params().lambda_tf});

        REQUIRE(charge_layout_new.get_local_potential({0, 0, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 5, 1}).has_value());

        CHECK(charge_layout_new.get_local_potential({0, 0, 1}).value() < 0);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() < 0);
        CHECK(charge_layout_new.get_local_potential({10, 5, 1}).value() < 0);
    }

    SECTION("assign defect | positive defect")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};

        charge_layout_new.add_sidb_defect_to_potential_landscape(
            {5, 1, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout_new.get_simulation_params().epsilon_r,
                                   charge_layout_new.get_simulation_params().lambda_tf});

        REQUIRE(charge_layout_new.get_local_potential({0, 0, 1}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({1, 3, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 5, 1}).has_value());

        CHECK(charge_layout_new.get_local_potential({0, 0, 1}).value() > 0);
        CHECK(charge_layout_new.get_local_potential({1, 3, 0}).value() > 0);
        CHECK(charge_layout_new.get_local_potential({10, 5, 1}).value() > 0);
    }

    SECTION("assign defect and perturber")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout_new{lyt_new, params, sidb_charge_state::NEUTRAL};

        REQUIRE(charge_layout_new.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout_new.get_local_potential({10, 5, 1}).has_value());

        charge_layout_new.assign_charge_state({10, 5, 1}, sidb_charge_state::NEGATIVE);
        charge_layout_new.update_after_charge_change();

        charge_layout_new.add_sidb_defect_to_potential_landscape(
            {-10, 5, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout_new.get_simulation_params().epsilon_r,
                                     charge_layout_new.get_simulation_params().lambda_tf});
        CHECK_THAT(charge_layout_new.get_local_potential({0, 0, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("layout with perturber |assigning and erasing defect")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt_new, params, sidb_charge_state::NEUTRAL};

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({10, 5, 1}).has_value());

        charge_layout.assign_charge_state({10, 5, 1}, sidb_charge_state::NEGATIVE);
        charge_layout.update_after_charge_change();
        charge_layout.add_sidb_defect_to_potential_landscape(
            {-10, 5, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_simulation_params().epsilon_r,
                                     charge_layout.get_simulation_params().lambda_tf});

        CHECK_THAT(charge_layout.get_local_potential({0, 0, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        charge_layout.erase_defect({-10, 5, 1});
        CHECK(charge_layout.get_local_potential({0, 0, 0}).value() < 0);
    }

    SECTION("layout with neutrally charged SiDBs |assigning and erasing defect")
    {
        TestType                         lyt_new{};
        const sidb_simulation_parameters params{3, -0.32};

        lyt_new.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt_new.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt_new, params, sidb_charge_state::NEUTRAL};
        charge_layout.add_sidb_defect_to_potential_landscape(
            {-10, 5, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_simulation_params().epsilon_r,
                                     charge_layout.get_simulation_params().lambda_tf});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({10, 5, 1}).has_value());

        CHECK(charge_layout.get_local_potential({0, 0, 0}).value() > 0);
        CHECK(charge_layout.get_local_potential({10, 5, 1}).value() > 0);

        charge_layout.erase_defect({-10, 5, 1});
        CHECK_THAT(charge_layout.get_local_potential({0, 0, 0}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_local_potential({10, 5, 1}).value(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("experiments with defects")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({0, 0, 0});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());

        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());

        const auto loc_one_wo_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        const auto loc_two_wo_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        const auto loc_three_wo_defect = charge_layout.get_local_potential({5, 0, 0}).value();

        charge_layout.add_sidb_defect_to_potential_landscape(
            {-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout.get_simulation_params().epsilon_r,
                                    charge_layout.get_simulation_params().lambda_tf});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());

        const auto loc_one_w_negative_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        const auto loc_two_w_negative_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        const auto loc_three_w_negative_defect = charge_layout.get_local_potential({5, 0, 0}).value();
        const auto defect_potentials_negative  = charge_layout.get_local_defect_potentials();
        REQUIRE(!defect_potentials_negative.empty());

        CHECK(loc_one_wo_defect > loc_one_w_negative_defect);
        CHECK(loc_two_wo_defect > loc_two_w_negative_defect);
        CHECK(loc_three_wo_defect > loc_three_w_negative_defect);

        charge_layout.add_sidb_defect_to_potential_landscape(
            {-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 0, charge_layout.get_simulation_params().epsilon_r,
                                    charge_layout.get_simulation_params().lambda_tf});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());

        const auto loc_one_w_neutral_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        const auto loc_two_w_neutral_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        const auto loc_three_w_neutral_defect = charge_layout.get_local_potential({5, 0, 0}).value();

        charge_layout.add_sidb_defect_to_potential_landscape(
            {-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_simulation_params().epsilon_r,
                                    charge_layout.get_simulation_params().lambda_tf});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());
        const auto loc_one_w_positive_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        const auto loc_two_w_positive_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        const auto loc_three_w_positive_defect = charge_layout.get_local_potential({5, 0, 0}).value();
        const auto defect_potentials_positive  = charge_layout.get_local_defect_potentials();
        REQUIRE(!defect_potentials_positive.empty());

        CHECK(loc_one_w_positive_defect > loc_one_w_neutral_defect);
        CHECK(loc_two_w_positive_defect > loc_two_w_neutral_defect);
        CHECK(loc_three_w_positive_defect > loc_three_w_neutral_defect);
    }

    SECTION("experiments with defects | assigning and reassigning defects")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEGATIVE};
        charge_layout.assign_dependent_cell({0, 0, 0});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());

        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        auto loc_one_wo_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        auto loc_two_wo_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        auto loc_three_wo_defect = charge_layout.get_local_potential({5, 0, 0}).value();

        charge_layout.add_sidb_defect_to_potential_landscape(
            {-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout.get_simulation_params().epsilon_r,
                                    charge_layout.get_simulation_params().lambda_tf});
        auto loc_one_w_negative_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        auto loc_two_w_negative_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        auto loc_three_w_negative_defect = charge_layout.get_local_potential({5, 0, 0}).value();
        auto defect_potentials_negative  = charge_layout.get_local_defect_potentials();
        REQUIRE(!defect_potentials_negative.empty());

        CHECK(loc_one_wo_defect > loc_one_w_negative_defect);
        CHECK(loc_two_wo_defect > loc_two_w_negative_defect);
        CHECK(loc_three_wo_defect > loc_three_w_negative_defect);

        charge_layout.add_sidb_defect_to_potential_landscape(
            {-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 0, charge_layout.get_simulation_params().epsilon_r,
                                    charge_layout.get_simulation_params().lambda_tf});
        auto loc_one_w_neutral_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        auto loc_two_w_neutral_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        auto loc_three_w_neutral_defect = charge_layout.get_local_potential({5, 0, 0}).value();

        charge_layout.add_sidb_defect_to_potential_landscape(
            {-4, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1, charge_layout.get_simulation_params().epsilon_r,
                                    charge_layout.get_simulation_params().lambda_tf});
        auto loc_one_w_positive_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        auto loc_two_w_positive_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        auto loc_three_w_positive_defect = charge_layout.get_local_potential({5, 0, 0}).value();
        auto defect_potentials_positive  = charge_layout.get_local_defect_potentials();
        REQUIRE(!defect_potentials_positive.empty());

        CHECK(loc_one_w_positive_defect > loc_one_w_neutral_defect);
        CHECK(loc_two_w_positive_defect > loc_two_w_neutral_defect);
        CHECK(loc_three_w_positive_defect > loc_three_w_neutral_defect);

        CHECK_THAT((defect_potentials_negative[static_cast<uint64_t>(charge_layout.cell_to_index({0, 0, 0}))] +
                    defect_potentials_positive[static_cast<uint64_t>(charge_layout.cell_to_index({0, 0, 0}))]),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT((defect_potentials_negative[static_cast<uint64_t>(charge_layout.cell_to_index({3, 0, 0}))] +
                    defect_potentials_positive[static_cast<uint64_t>(charge_layout.cell_to_index({3, 0, 0}))]),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        CHECK_THAT((defect_potentials_negative[static_cast<uint64_t>(charge_layout.cell_to_index({5, 0, 0}))] +
                    defect_potentials_positive[static_cast<uint64_t>(charge_layout.cell_to_index({5, 0, 0}))]),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        // todo
    }

    SECTION("assign defect on DB position which is not allowed")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEUTRAL};
        charge_layout.assign_dependent_cell({0, 0, 0});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());

        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        auto loc_one_wo_defect   = charge_layout.get_local_potential({0, 0, 0}).value();
        auto loc_two_wo_defect   = charge_layout.get_local_potential({3, 0, 0}).value();
        auto loc_three_wo_defect = charge_layout.get_local_potential({5, 0, 0}).value();

        charge_layout.add_sidb_defect_to_potential_landscape(
            {0, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout.get_simulation_params().epsilon_r,
                                   charge_layout.get_simulation_params().lambda_tf});

        CHECK_THAT(loc_one_wo_defect - (charge_layout.get_local_potential({0, 0, 0}).value()),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(loc_two_wo_defect - (charge_layout.get_local_potential({3, 0, 0}).value()),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(loc_three_wo_defect - (charge_layout.get_local_potential({5, 0, 0}).value()),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));

        auto defect_potentials_negative = charge_layout.get_local_defect_potentials();
        REQUIRE(defect_potentials_negative.size() == 3);
        CHECK(defect_potentials_negative.at(0) == 0.0);
        CHECK(defect_potentials_negative.at(1) == 0.0);
        CHECK(defect_potentials_negative.at(2) == 0.0);
    }

    SECTION("assign defects with different screening lengths")
    {
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        charge_distribution_surface charge_layout{lyt, params, sidb_charge_state::NEUTRAL};
        charge_layout.assign_dependent_cell({0, 0, 0});

        REQUIRE(charge_layout.get_local_potential({0, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({3, 0, 0}).has_value());
        REQUIRE(charge_layout.get_local_potential({5, 0, 0}).has_value());

        CHECK(charge_layout.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({3, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_layout.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);

        charge_layout.add_sidb_defect_to_potential_landscape(
            {8, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout.get_simulation_params().epsilon_r,
                                   charge_layout.get_simulation_params().lambda_tf});

        auto loc_one_w_defect_normal_screening   = charge_layout.get_local_potential({0, 0, 0}).value();
        auto loc_two_w_defect_normal_screening   = charge_layout.get_local_potential({3, 0, 0}).value();
        auto loc_three_w_defect_normal_screening = charge_layout.get_local_potential({5, 0, 0}).value();

        charge_layout.add_sidb_defect_to_potential_landscape(
            {8, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, charge_layout.get_simulation_params().epsilon_r,
                                   charge_layout.get_simulation_params().lambda_tf * 20});

        auto loc_one_w_defec_strong_screening    = charge_layout.get_local_potential({0, 0, 0}).value();
        auto loc_two_w_defect_strong_screening   = charge_layout.get_local_potential({3, 0, 0}).value();
        auto loc_three_w_defect_strong_screening = charge_layout.get_local_potential({5, 0, 0}).value();

        auto defect_potentials_negative = charge_layout.get_local_defect_potentials();
        CHECK(!defect_potentials_negative.empty());

        CHECK(loc_one_w_defect_normal_screening > loc_one_w_defec_strong_screening);
        CHECK(loc_two_w_defect_normal_screening > loc_two_w_defect_strong_screening);
        CHECK(loc_three_w_defect_normal_screening > loc_three_w_defect_strong_screening);

        charge_layout.erase_defect({8, 0, 0});
        CHECK_THAT(charge_layout.get_local_potential({0, 0, 0}).value(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_local_potential({3, 0, 0}).value(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
        CHECK_THAT(charge_layout.get_local_potential({5, 0, 0}).value(),
                   Catch::Matchers::WithinAbs(0, constants::ERROR_MARGIN));
    }
}

TEST_CASE("Assign charge index", "[charge-distribution-surface]")
{
    using TestType = sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>;
    TestType lyt{};

    lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 1}, TestType::cell_type::NORMAL);

    SECTION("Base 2")
    {
        charge_distribution_surface<TestType> charge_lyt{lyt, sidb_simulation_parameters{2}};

        charge_lyt.assign_charge_index(0);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::NEGATIVE);

        charge_lyt.assign_charge_index(1);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::NEUTRAL);

        charge_lyt.assign_charge_index(3);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::NEUTRAL);

        charge_lyt.assign_charge_index(1);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::NEUTRAL);
    }

    SECTION("Base 3")
    {
        charge_distribution_surface<TestType> charge_lyt{lyt, sidb_simulation_parameters{3}};

        charge_lyt.assign_charge_index(0);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::NEGATIVE);

        charge_lyt.assign_charge_index(2);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::POSITIVE);

        charge_lyt.assign_charge_index(8);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::POSITIVE);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::POSITIVE);

        charge_lyt.assign_charge_index(5);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::POSITIVE);

        charge_lyt.assign_charge_index(2);
        CHECK(charge_lyt.get_charge_state({0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt.get_charge_state({0, 1}) == sidb_charge_state::POSITIVE);
    }
}

TEMPLATE_TEST_CASE("Charge distribution surface defect vs SiDB equivalence", "[charge-distribution-surface]",
                   sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    sidb_cell_clk_lyt_siqad lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({0, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({5, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    const auto                                 sim_params = sidb_simulation_parameters{};
    charge_distribution_surface<decltype(lyt)> charge_lyt{lyt, sim_params};

    charge_lyt.assign_charge_state({0, 1, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt.update_after_charge_change();

    REQUIRE(charge_lyt.is_physically_valid());
    REQUIRE(charge_lyt.get_local_potential_by_index(0).has_value());
    REQUIRE(charge_lyt.get_local_potential_by_index(1).has_value());
    REQUIRE(charge_lyt.get_local_potential_by_index(2).has_value());

    sidb_defect_surface<decltype(lyt)> defect_lyt{lyt};
    defect_lyt.assign_cell_type({5, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);
    defect_lyt.assign_sidb_defect({5, 1, 0},
                                  sidb_defect{sidb_defect_type::DB, -1, sim_params.epsilon_r, sim_params.lambda_tf});

    charge_distribution_surface<decltype(defect_lyt)> charge_lyt_defect{defect_lyt, sim_params};

    charge_lyt_defect.assign_charge_state({0, 1, 0}, sidb_charge_state::NEUTRAL);
    charge_lyt_defect.update_after_charge_change();

    CHECK(charge_lyt_defect.is_physically_valid());

    CHECK_THAT(charge_lyt_defect.get_electrostatic_potential_energy() - 0.082227626226473852,
               Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

    CHECK_THAT(charge_lyt.get_electrostatic_potential_energy() - charge_lyt_defect.get_electrostatic_potential_energy(),
               Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

    REQUIRE(charge_lyt_defect.get_local_potential_by_index(0).has_value());
    REQUIRE(charge_lyt_defect.get_local_potential_by_index(1).has_value());
    REQUIRE(charge_lyt_defect.get_local_defect_potential({5, 1, 0}).has_value());

    CHECK_THAT(charge_lyt.get_local_potential_by_index(0).value() -
                   charge_lyt_defect.get_local_potential_by_index(0).value(),
               Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

    CHECK_THAT(charge_lyt.get_local_potential_by_index(1).value() -
                   charge_lyt_defect.get_local_potential_by_index(1).value(),
               Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

    CHECK_THAT(charge_lyt.get_local_potential_by_index(2).value() -
                   charge_lyt_defect.get_local_defect_potential({5, 1, 0}).value(),
               Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
}

TEST_CASE("Tests for Si-111 lattice orientation", "[charge-distribution-surface]")
{
    sidb_111_cell_clk_lyt_siqad lyt{};

    SECTION("Electrostatic potential energy of the charge configuration")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_111_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0, 0}, sidb_111_cell_clk_lyt_siqad::cell_type::NORMAL);

        charge_distribution_surface charge_layout{lyt, sidb_simulation_parameters{}};

        CHECK_THAT(charge_layout.get_nm_distance_between_sidbs({0, 0, 0}, {2, 0, 0}),
                   Catch::Matchers::WithinAbs(1.33, constants::ERROR_MARGIN));

        charge_layout.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
        charge_layout.assign_charge_state({2, 0, 0}, sidb_charge_state::NEGATIVE);

        // system energy is zero when all SiDBs are positively charged.
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK_THAT(charge_layout.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.14818, constants::ERROR_MARGIN));

        // system energy is zero when all SiDBs are neutrally charged.
        charge_layout.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK_THAT(charge_layout.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

        // system energy is zero when all SiDBs are positively charged.
        charge_layout.assign_all_charge_states(sidb_charge_state::POSITIVE);
        charge_layout.update_local_internal_potential();
        charge_layout.recompute_electrostatic_potential_energy();
        CHECK_THAT(charge_layout.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.14818, constants::ERROR_MARGIN));
    }
}
