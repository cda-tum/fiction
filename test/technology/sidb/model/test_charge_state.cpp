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
 * @brief Tests for `fiction/technology/sidb/model/charge_state.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/model/charge_state.hpp>

#include <cstdint>
#include <limits>

using namespace fiction;

TEST_CASE("charge_state_to_sign returns the correct sign for each charge state", "[sidb-charge-state]")
{
    CHECK(sidb::model::charge_state_to_sign(sidb::model::charge_state::POSITIVE) == 1);
    CHECK(sidb::model::charge_state_to_sign(sidb::model::charge_state::NEUTRAL) == 0);
    CHECK(sidb::model::charge_state_to_sign(sidb::model::charge_state::NEGATIVE) == -1);
    CHECK(sidb::model::charge_state_to_sign(sidb::model::charge_state::NONE) == 0);
}

TEST_CASE("charge_state_to_sign is noexcept", "[sidb-charge-state]")
{
    CHECK(noexcept(sidb::model::charge_state_to_sign(sidb::model::charge_state::NEUTRAL)));
}

TEST_CASE("sign_to_charge_state returns the correct charge state for each sign", "[sidb-charge-state]")
{
    SECTION("sg is -1")
    {
        CHECK(sidb::model::sign_to_charge_state(-1) == sidb::model::charge_state::NEGATIVE);
    }

    SECTION("sg is 0")
    {
        CHECK(sidb::model::sign_to_charge_state(0) == sidb::model::charge_state::NEUTRAL);
    }

    SECTION("sg is +1")
    {
        CHECK(sidb::model::sign_to_charge_state(1) == sidb::model::charge_state::POSITIVE);
    }

    SECTION("sg is invalid")
    {
        CHECK(sidb::model::sign_to_charge_state(2) == sidb::model::charge_state::NONE);
        CHECK(sidb::model::sign_to_charge_state(3) == sidb::model::charge_state::NONE);
        CHECK(sidb::model::sign_to_charge_state(4) == sidb::model::charge_state::NONE);
        CHECK(sidb::model::sign_to_charge_state(std::numeric_limits<int8_t>::max()) == sidb::model::charge_state::NONE);

        CHECK(sidb::model::sign_to_charge_state(-2) == sidb::model::charge_state::NONE);
        CHECK(sidb::model::sign_to_charge_state(-3) == sidb::model::charge_state::NONE);
        CHECK(sidb::model::sign_to_charge_state(-4) == sidb::model::charge_state::NONE);
        CHECK(sidb::model::sign_to_charge_state(std::numeric_limits<int8_t>::min()) == sidb::model::charge_state::NONE);
    }
}

TEST_CASE("sign_to_charge_state is noexcept", "[sidb-charge-state]")
{
    CHECK(noexcept(sidb::model::sign_to_charge_state(0)));
}

TEST_CASE("charge_configuration_to_string builds correct strings", "[sidb-charge-state]")
{
    SECTION("empty charge configuration")
    {
        const std::vector<sidb::model::charge_state> charge_distribution{};

        CHECK(sidb::model::charge_configuration_to_string(charge_distribution).empty());
    }

    SECTION("charge configuration with one negative charge")
    {
        const std::vector<sidb::model::charge_state> charge_distribution{sidb::model::charge_state::NEGATIVE};

        CHECK(sidb::model::charge_configuration_to_string(charge_distribution) == "-");
    }

    SECTION("charge configuration with one neutral charge")
    {
        const std::vector<sidb::model::charge_state> charge_distribution{sidb::model::charge_state::NEUTRAL};

        CHECK(sidb::model::charge_configuration_to_string(charge_distribution) == "0");
    }

    SECTION("charge configuration with one positive charge")
    {
        const std::vector<sidb::model::charge_state> charge_distribution{sidb::model::charge_state::POSITIVE};

        CHECK(sidb::model::charge_configuration_to_string(charge_distribution) == "+");
    }

    SECTION("charge configuration with multiple charges")
    {
        const std::vector<sidb::model::charge_state> charge_distribution{sidb::model::charge_state::POSITIVE,
                                                                         sidb::model::charge_state::NEGATIVE,
                                                                         sidb::model::charge_state::NEUTRAL};

        CHECK(sidb::model::charge_configuration_to_string(charge_distribution) == "+-0");
    }
}

TEST_CASE("charge_configuration_to_string is noexcept", "[sidb-charge-state]")
{
    CHECK(noexcept(sidb::model::charge_configuration_to_string({})));
}
