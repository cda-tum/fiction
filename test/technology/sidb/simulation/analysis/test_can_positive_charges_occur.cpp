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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <stdexcept>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;

TEST_CASE("One BDL pair with one perturber", "[can-positive-charges-occur]")
{
    layout lyt{};

    lyt.assign_dot_tag({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({4, 0, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({6, 0, 0}, dot_tag::NORMAL);

    SECTION("Default values")
    {
        const simulation_parameters params{2, -0.32};
        CHECK(!can_positive_charges_occur(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 1")
    {
        const simulation_parameters params{2, -0.32, 1, 1};
        CHECK(can_positive_charges_occur(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 10")
    {
        const simulation_parameters params{2, -0.32, 1, 10};
        CHECK(can_positive_charges_occur(lyt, params));
    }
}

TEST_CASE("Y-shaped SiDB OR gate with input 01, using siqad coordinates", "[can-positive-charges-occur]")
{
    layout lyt{};

    lyt.assign_dot_tag({6, 2, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({8, 3, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({12, 3, 0}, dot_tag::NORMAL);

    lyt.assign_dot_tag({14, 2, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({10, 5, 0}, dot_tag::NORMAL);

    lyt.assign_dot_tag({10, 6, 1}, dot_tag::NORMAL);
    lyt.assign_dot_tag({10, 8, 1}, dot_tag::NORMAL);
    lyt.assign_dot_tag({16, 1, 0}, dot_tag::NORMAL);

    SECTION("Default values")
    {
        const simulation_parameters params{2, -0.32};
        CHECK(!can_positive_charges_occur(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 1")
    {
        const simulation_parameters params{2, -0.32, 1, 1};
        CHECK(can_positive_charges_occur(lyt, params));
    }

    SECTION("epsilon = 1, lambda = 10")
    {
        const simulation_parameters params{2, -0.32, 1, 10};
        CHECK(can_positive_charges_occur(lyt, params));
    }
}

TEST_CASE("Positive-charge analysis propagates invalid lattice-basis errors", "[can-positive-charges-occur]")
{
    layout       lyt{};
    lattice_site invalid{};
    invalid.z = 2;
    lyt.assign_dot_tag(invalid, dot_tag::NORMAL);
    CHECK_THROWS_AS(can_positive_charges_occur(lyt, simulation_parameters{}), std::out_of_range);
}
