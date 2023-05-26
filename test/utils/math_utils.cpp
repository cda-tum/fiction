//
// Created by Jan Drewniok on 19.04.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/utils/math_utils.hpp>

#include <cstdint>
#include <limits>

using namespace fiction;

TEST_CASE("round_to_n_decimal_places should round an input number to n decimal places", "[round_to_n_decimal_places]")
{
    SECTION("int64_t")
    {
        CHECK(round_to_n_decimal_places(-1LL, 0) == -1LL);
        CHECK(round_to_n_decimal_places(-1LL, 10) == -1LL);
        CHECK(round_to_n_decimal_places(1LL, 0) == 1LL);
        CHECK(round_to_n_decimal_places(1LL, 10) == 1LL);
    }

    SECTION("double")
    {
        const double value_positive = 3.145926;
        CHECK(round_to_n_decimal_places(value_positive, 0) == 3);
        CHECK(round_to_n_decimal_places(value_positive, 1) == 3.1);
        CHECK(round_to_n_decimal_places(value_positive, 2) == 3.15);
        CHECK(round_to_n_decimal_places(value_positive, 3) == 3.146);
        CHECK(round_to_n_decimal_places(value_positive, 4) == 3.1459);
        CHECK(round_to_n_decimal_places(value_positive, 5) == 3.14593);
        CHECK(round_to_n_decimal_places(value_positive, 6) == 3.145926);

        const double value_negative = -3.145926;
        CHECK(round_to_n_decimal_places(value_negative, 0) == -3);
        CHECK(round_to_n_decimal_places(value_negative, 1) == -3.1);
        CHECK(round_to_n_decimal_places(value_negative, 2) == -3.15);
        CHECK(round_to_n_decimal_places(value_negative, 3) == -3.146);
        CHECK(round_to_n_decimal_places(value_negative, 4) == -3.1459);
        CHECK(round_to_n_decimal_places(value_negative, 5) == -3.14593);
        CHECK(round_to_n_decimal_places(value_negative, 6) == -3.145926);
    }

    SECTION("Edge cases")
    {
        CHECK(round_to_n_decimal_places(1.005, 2) == 1.0);
        CHECK(round_to_n_decimal_places(0.000001, 6) == 0.000001);
        CHECK(round_to_n_decimal_places(0.0000001, 6) == 0);
        CHECK(round_to_n_decimal_places(-0.000001, 6) == -0.000001);
        CHECK(round_to_n_decimal_places(-0.0000001, 6) == 0);
    }
}

TEMPLATE_TEST_CASE("integral_abs should compute the absolute value of a number of different integral types",
                   "[integral_abs]", int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t)
{
    const auto x = static_cast<TestType>(-42);

    CHECK(integral_abs(integral_abs(x) - (x < 0 ? -x : x)) == 0);
}
