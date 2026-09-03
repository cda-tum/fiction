/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/technology/sidb/charge_distribution.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;

TEST_CASE("Charge distribution over a layout", "[charge-distribution]")
{
    layout lyt{};
    lyt.assign_cell_type({3, 1, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 1}, sidb_technology::cell_type::NORMAL);

    SECTION("default state")
    {
        const charge_distribution cd{lyt};

        CHECK(cd.size() == 3);
        CHECK(!cd.empty());
        CHECK(cd.sites() == lyt.sidbs());
        CHECK(cd.num_negative_sidbs() == 3);
        CHECK(cd.num_neutral_sidbs() == 0);
        CHECK(cd.num_positive_sidbs() == 0);
        CHECK(cd.energy() == 0.0);
        CHECK(cd.charge_exists(charge_state::NEGATIVE));
        CHECK(!cd.charge_exists(charge_state::NEUTRAL));
        CHECK(cd.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(cd.get_charge_state({9, 9, 0}) == charge_state::NONE);
        CHECK(cd.get_charge_state_by_index(2) == charge_state::NEGATIVE);
        CHECK(cd.get_charge_state_by_index(3) == charge_state::NONE);
        CHECK(cd.index_of({1, 0, 1}) == 1);
        CHECK(!cd.index_of({1, 0, 0}).has_value());
    }
    SECTION("assignment by site and by index")
    {
        charge_distribution cd{lyt, charge_state::NEUTRAL};

        cd.assign_charge_state({3, 1, 0}, charge_state::POSITIVE);
        cd.assign_charge_state_by_index(0, charge_state::NEGATIVE);
        cd.assign_charge_state({9, 9, 0}, charge_state::POSITIVE);  // ignored

        CHECK(cd.charge_states() ==
              std::vector<charge_state>{charge_state::NEGATIVE, charge_state::NEUTRAL, charge_state::POSITIVE});
        CHECK(cd.num_negative_sidbs() == 1);
        CHECK(cd.num_neutral_sidbs() == 1);
        CHECK(cd.num_positive_sidbs() == 1);

        std::vector<lattice_site> visited{};
        cd.foreach_cell([&visited](const auto& s) { visited.push_back(s); });
        CHECK(visited == lyt.sidbs());

        cd.assign_all_charge_states(charge_state::NEGATIVE);
        CHECK(cd.num_negative_sidbs() == 3);
    }
    SECTION("charge index")
    {
        charge_distribution cd{lyt, charge_state::NEGATIVE};
        CHECK(cd.charge_index(2) == 0);
        CHECK(cd.charge_index(3) == 0);

        cd.assign_charge_state_by_index(2, charge_state::NEUTRAL);
        CHECK(cd.charge_index(2) == 1);
        CHECK(cd.charge_index(3) == 1);

        cd.assign_charge_state_by_index(0, charge_state::POSITIVE);
        CHECK(cd.charge_index(3) == (2 * 9) + 1);
    }
    SECTION("equality, hash, and sharing")
    {
        charge_distribution a{lyt};
        charge_distribution b{a.shared_sites(), charge_state::NEGATIVE};

        CHECK(a == b);
        CHECK(a.same_charge_states(b));
        CHECK(std::hash<charge_distribution>{}(a) == std::hash<charge_distribution>{}(b));
        CHECK(a.shared_sites() == b.shared_sites());

        b.assign_energy(1.0);
        CHECK(a != b);
        CHECK(a.same_charge_states(b));

        b.assign_charge_state_by_index(1, charge_state::NEUTRAL);
        CHECK(!a.same_charge_states(b));

        const std::unordered_set<charge_distribution> set{a, b, a};
        CHECK(set.size() == 2);
    }
    SECTION("explicit states")
    {
        const charge_distribution cd{std::make_shared<const std::vector<lattice_site>>(lyt.sidbs()),
                                     {charge_state::NEUTRAL, charge_state::NEGATIVE, charge_state::NEUTRAL},
                                     -0.5};
        CHECK(cd.energy() == -0.5);
        CHECK(cd.get_charge_state({1, 0, 1}) == charge_state::NEGATIVE);
    }
    SECTION("empty distribution")
    {
        const charge_distribution cd{};
        CHECK(cd.empty());
        CHECK(cd.sites().empty());
        CHECK(cd.get_charge_state({0, 0, 0}) == charge_state::NONE);
    }
}
