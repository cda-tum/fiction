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
 * @brief Tests for `fiction/technology/sidb/simulation/potential_landscape.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cstddef>
#include <unordered_map>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace Catch::Matchers;

TEST_CASE("Potential landscape of a three-SiDB layout", "[potential-landscape]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.32};
    const potential_landscape   land{lyt, params};

    SECTION("geometry and potentials")
    {
        CHECK(land.num_sidbs() == 3);
        CHECK(*land.sites() == lyt.sidbs());
        CHECK(land.params().base == 2);
        CHECK(land.get_layout() == lyt);

        CHECK_THAT(land.nm_distance(0, 1), WithinAbs(1.92, 1E-5));
        CHECK_THAT(land.nm_distance(1, 2), WithinAbs(0.768, 1E-5));
        CHECK(land.nm_distance(1, 1) == 0.0);
        CHECK(land.chargeless_potential(1, 1) == 0.0);
        CHECK(land.chargeless_potential(0, 1) == land.chargeless_potential(1, 0));
        CHECK(land.chargeless_potential(1, 2) > land.chargeless_potential(0, 1));
        CHECK_THAT(land.chargeless_potential(1, 2), WithinAbs(land.chargeless_potential_at_distance(0.768), 1E-12));
        CHECK(land.local_external_potential(0) == 0.0);
        CHECK(land.local_potential_caused_by_defects(0) == 0.0);
        CHECK(land.defects().empty());

        const auto& t = land.effective_charge_transition_thresholds(0);
        CHECK_THAT(t[static_cast<std::size_t>(charge_transition_threshold_bounds::NEGATIVE_UPPER_BOUND)],
                   WithinAbs(0.32, 1E-5));
        CHECK_THAT(t[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)],
                   WithinAbs(0.91, 1E-5));
    }
    SECTION("energies and validity")
    {
        const auto all_negative = land.evaluate(charge_distribution{lyt, charge_state::NEGATIVE});
        const auto all_neutral  = land.evaluate(charge_distribution{lyt, charge_state::NEUTRAL});

        CHECK(all_neutral.energy() == 0.0);
        CHECK(all_negative.energy() > 0.0);
        CHECK(!land.is_physically_valid(all_negative));  // the two close SiDBs cannot both be negative
        CHECK(!land.is_physically_valid(all_neutral));   // the isolated SiDB has to be negative

        charge_distribution ground{lyt, charge_state::NEGATIVE};
        ground.assign_charge_state({5, 0, 0}, charge_state::NEUTRAL);
        ground = land.evaluate(ground);

        CHECK(land.is_physically_valid(ground));
        CHECK(land.is_population_stable(ground, land.local_internal_potentials(ground)));
        CHECK(land.is_configuration_stable(ground, land.local_internal_potentials(ground)));
        CHECK_THAT(ground.energy(), WithinAbs(land.energy(ground, land.local_internal_potentials(ground)), 1E-12));

        const auto pot = land.local_potentials(ground);
        REQUIRE(pot.size() == 3);
        CHECK(pot[1] < 0.0);  // the neutral SiDB sits between two negative ones
    }
    SECTION("external potentials")
    {
        const potential_landscape shifted{lyt, params, {{lattice_site{0, 0, 0}, 0.1}}, 0.2};

        CHECK_THAT(shifted.local_external_potential(0), WithinAbs(0.3, 1E-12));
        CHECK_THAT(shifted.local_external_potential(1), WithinAbs(0.2, 1E-12));

        const auto all_negative = charge_distribution{lyt, charge_state::NEGATIVE};

        CHECK(shifted.energy(all_negative) < land.energy(all_negative));
    }
}

TEST_CASE("Potential landscape with a charged defect", "[potential-landscape]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_defect({1, 1, 0}, defect{defect_type::SI_VACANCY, -1, 5.6, 5.0});
    lyt.assign_defect({6, 1, 0}, defect{defect_type::SILOXANE, 0});  // neutral type, does not enter

    const potential_landscape land{lyt, simulation_parameters{2, -0.32}};

    REQUIRE(land.defects().size() == 1);
    CHECK(land.local_potential_caused_by_defects(0) < 0.0);
    CHECK(land.local_potential_caused_by_defects(0) < land.local_potential_caused_by_defects(1));

    const auto neutral = charge_distribution{lyt, charge_state::NEUTRAL};
    const auto pot     = land.local_internal_potentials(neutral);
    CHECK_THAT(pot[0], WithinAbs(land.local_potential_caused_by_defects(0), 1E-12));

    // a negative defect makes the neutral distribution more attractive than the negative one
    CHECK(land.energy(charge_distribution{lyt, charge_state::NEGATIVE}) > land.energy(neutral));
}
