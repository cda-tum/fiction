//
// Created by marcel on 29.03.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb_charge_state.hpp>

#include <cstdint>
#include <limits>

using namespace fiction;

TEST_CASE("charge_state_to_sign returns the correct sign for each charge state", "[sidb-charge-state]")
{
    CHECK(charge_state_to_sign(sidb_charge_state::POSITIVE) == 1);
    CHECK(charge_state_to_sign(sidb_charge_state::NEUTRAL) == 0);
    CHECK(charge_state_to_sign(sidb_charge_state::NEGATIVE) == -1);
    CHECK(charge_state_to_sign(sidb_charge_state::NONE) == 0);
}

TEST_CASE("charge_state_to_sign is noexcept", "[sidb-charge-state]")
{
    CHECK(noexcept(charge_state_to_sign(sidb_charge_state::NEUTRAL)));
}

TEST_CASE("sign_to_charge_state returns the correct charge state for each sign", "[sidb-charge-state]")
{
    SECTION("sg is -1")
    {
        CHECK(sign_to_charge_state(-1) == sidb_charge_state::NEGATIVE);
    }

    SECTION("sg is 0")
    {
        CHECK(sign_to_charge_state(0) == sidb_charge_state::NEUTRAL);
    }

    SECTION("sg is +1")
    {
        CHECK(sign_to_charge_state(1) == sidb_charge_state::POSITIVE);
    }

    SECTION("sg is invalid")
    {
        CHECK(sign_to_charge_state(2) == sidb_charge_state::NONE);
        CHECK(sign_to_charge_state(3) == sidb_charge_state::NONE);
        CHECK(sign_to_charge_state(4) == sidb_charge_state::NONE);
        CHECK(sign_to_charge_state(std::numeric_limits<int8_t>::max()) == sidb_charge_state::NONE);

        CHECK(sign_to_charge_state(-2) == sidb_charge_state::NONE);
        CHECK(sign_to_charge_state(-3) == sidb_charge_state::NONE);
        CHECK(sign_to_charge_state(-4) == sidb_charge_state::NONE);
        CHECK(sign_to_charge_state(std::numeric_limits<int8_t>::min()) == sidb_charge_state::NONE);
    }
}

TEST_CASE("sign_to_charge_state is noexcept", "[sidb-charge-state]")
{
    CHECK(noexcept(sign_to_charge_state(0)));
}

TEST_CASE("charge_configuration_to_string builds correct strings", "[sidb-charge-state]")
{
    SECTION("empty charge configuration")
    {
        const std::vector<sidb_charge_state> charge_distribution{};

        CHECK(charge_configuration_to_string(charge_distribution).empty());
    }

    SECTION("charge configuration with one negative charge")
    {
        const std::vector<sidb_charge_state> charge_distribution{sidb_charge_state::NEGATIVE};

        CHECK(charge_configuration_to_string(charge_distribution) == "-");
    }

    SECTION("charge configuration with one neutral charge")
    {
        const std::vector<sidb_charge_state> charge_distribution{sidb_charge_state::NEUTRAL};

        CHECK(charge_configuration_to_string(charge_distribution) == "0");
    }

    SECTION("charge configuration with one positive charge")
    {
        const std::vector<sidb_charge_state> charge_distribution{sidb_charge_state::POSITIVE};

        CHECK(charge_configuration_to_string(charge_distribution) == "+");
    }

    SECTION("charge configuration with multiple charges")
    {
        const std::vector<sidb_charge_state> charge_distribution{
            sidb_charge_state::POSITIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL};

        CHECK(charge_configuration_to_string(charge_distribution) == "+-0");
    }
}

TEST_CASE("charge_configuration_to_string is noexcept", "[sidb-charge-state]")
{
    CHECK(noexcept(charge_configuration_to_string({})));
}
