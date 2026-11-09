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

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp>

#include <stdexcept>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;

/** @brief Checks positive-charge outcomes for the same physical parameters. @param lyt Layout to analyze. */
void check_positive_charge_parameters(const layout& lyt)
{
    const auto [params, expected] = GENERATE(
        (Catch::Generators::table<simulation_parameters, bool>({{simulation_parameters{2, -0.32}, false},
                                                                {simulation_parameters{2, -0.32, 1, 1}, true},
                                                                {simulation_parameters{2, -0.32, 1, 10}, true}})));
    CAPTURE(params.epsilon_r, params.lambda_tf);
    CHECK(can_positive_charges_occur(lyt, params) == expected);
}

TEST_CASE("One BDL pair with one perturber", "[can-positive-charges-occur]")
{
    layout lyt{};

    lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({4, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);

    check_positive_charge_parameters(lyt);
}

TEST_CASE("Y-shaped SiDB OR gate with input 01", "[can-positive-charges-occur]")
{
    layout lyt{};

    lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({12, 3, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({14, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({10, 5, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 6, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({10, 8, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 1, 0}, dot_tag::NORMAL);

    check_positive_charge_parameters(lyt);
}

TEST_CASE("Positive-charge analysis propagates invalid lattice-basis errors", "[can-positive-charges-occur]")
{
    layout       lyt{};
    lattice_site invalid{};
    invalid.z = 2;
    lyt.assign_sidb(invalid, dot_tag::NORMAL);
    CHECK_THROWS_AS(can_positive_charges_occur(lyt, simulation_parameters{}), std::out_of_range);
}
