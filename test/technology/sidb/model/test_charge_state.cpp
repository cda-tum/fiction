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

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/technology/sidb/model/charge_state.hpp>

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::sidb::model;

TEST_CASE("charge_state_to_sign returns the correct sign for each charge state", "[sidb-charge-state]")
{
    CHECK(charge_state_to_sign(charge_state::POSITIVE) == 1);
    CHECK(charge_state_to_sign(charge_state::NEUTRAL) == 0);
    CHECK(charge_state_to_sign(charge_state::NEGATIVE) == -1);
    CHECK(charge_state_to_sign(charge_state::NONE) == 0);
}

TEST_CASE("charge_state_to_sign is noexcept", "[sidb-charge-state]")
{
    STATIC_REQUIRE(noexcept(charge_state_to_sign(charge_state::NEUTRAL)));
}

TEST_CASE("sign_to_charge_state returns the correct charge state for each sign", "[sidb-charge-state]")
{
    /** @brief Valid signs and invalid values on both sides of the valid range. */
    const auto [sign, expected] =
        GENERATE(table<int8_t, charge_state>({{int8_t{-1}, charge_state::NEGATIVE},
                                              {int8_t{0}, charge_state::NEUTRAL},
                                              {int8_t{1}, charge_state::POSITIVE},
                                              {int8_t{2}, charge_state::NONE},
                                              {int8_t{3}, charge_state::NONE},
                                              {int8_t{4}, charge_state::NONE},
                                              {std::numeric_limits<int8_t>::max(), charge_state::NONE},
                                              {int8_t{-2}, charge_state::NONE},
                                              {int8_t{-3}, charge_state::NONE},
                                              {int8_t{-4}, charge_state::NONE},
                                              {std::numeric_limits<int8_t>::min(), charge_state::NONE}}));
    CAPTURE(static_cast<int>(sign));
    CHECK(sign_to_charge_state(sign) == expected);
}

TEST_CASE("sign_to_charge_state is noexcept", "[sidb-charge-state]")
{
    STATIC_REQUIRE(noexcept(sign_to_charge_state(0)));
}

TEST_CASE("charge_configuration_to_string builds correct strings", "[sidb-charge-state]")
{
    /** @brief Independent configurations and their exact serialized charge order. */
    const auto [name, states, expected] = GENERATE(table<const char*, std::vector<charge_state>, std::string>(
        {{"empty", {}, ""},
         {"negative", {charge_state::NEGATIVE}, "-"},
         {"neutral", {charge_state::NEUTRAL}, "0"},
         {"positive", {charge_state::POSITIVE}, "+"},
         {"mixed", {charge_state::POSITIVE, charge_state::NEGATIVE, charge_state::NEUTRAL}, "+-0"},
         {"empty sites",
          {charge_state::NONE, charge_state::NEUTRAL, charge_state::NONE, charge_state::NEGATIVE, charge_state::NONE},
          "0-"}}));
    CAPTURE(name);
    CHECK(charge_configuration_to_string(states) == expected);
}

TEST_CASE("charge_configuration_to_string is noexcept", "[sidb-charge-state]")
{
    STATIC_REQUIRE(noexcept(charge_configuration_to_string({})));
}
