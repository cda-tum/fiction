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

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp>
#include <fiction/technology/sidb/technology.hpp>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;

TEST_CASE("One BDL pair with one perturber", "[can-positive-charges-occur]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);

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

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, sidb_technology::cell_type::NORMAL);

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
