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
 * @brief Tests for `fiction/utils/math/math_utils.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/utils/execution_timeout.hpp>
#include <fiction/utils/math/combination_utils.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

using namespace fiction;
using namespace fiction::utils::math;

TEST_CASE("round_to_n_decimal_places should round an input number to n decimal places", "[round-to-n-decimal-places]")
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
                   "[integral-abs]", int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t)
{
    const auto x = static_cast<TestType>(-42);

    CHECK(integral_abs(TestType{0}) == TestType{0});
    CHECK(integral_abs(TestType{42}) == TestType{42});
    if constexpr (std::is_signed_v<TestType>)
    {
        CHECK(integral_abs(x) == TestType{42});
    }
    else
    {
        CHECK(integral_abs(x) == x);
    }
}

TEST_CASE("Combination enumeration honors a shared deadline", "[execution-timeout]")
{
    const auto expired = std::chrono::steady_clock::now();
    CHECK_THROWS_AS(determine_all_combinations_of_distributing_k_entities_on_n_positions(2, 5, expired),
                    fiction::utils::timeout_error);
    CHECK(determine_all_combinations_of_distributing_k_entities_on_n_positions(3, 25,
                                                                               expired + std::chrono::seconds{10}) ==
          determine_all_combinations_of_distributing_k_entities_on_n_positions(3, 25));
    // C(1000, 5) fits max_size but cannot be materialized during this budget.
    CHECK_THROWS_AS(determine_all_combinations_of_distributing_k_entities_on_n_positions(
                        5, 1000, std::chrono::steady_clock::now() + std::chrono::milliseconds{20}),
                    fiction::utils::timeout_error);
}

TEST_CASE("Binomial Coefficient Tests", "[binomial-coefficient]")
{
    /** @brief Population, selection size, and exact coefficient. */
    const auto [n, k, expected] =
        GENERATE(Catch::Generators::table<uint64_t, uint64_t, uint64_t>({{5, 2, 10},
                                                                         {10, 3, 120},
                                                                         {0, 0, 1},
                                                                         {8, 8, 1},
                                                                         {7, 10, 0},
                                                                         {15, 7, 6435},
                                                                         {20, 10, 184756},
                                                                         {30, 15, 155117520},
                                                                         {50, 25, 126410606437752}}));
    CAPTURE(n, k);
    CHECK(binomial_coefficient(n, k) == expected);
}

TEST_CASE("Test the computation of the cartesian combinations", "[cartesian_combinations]")
{
    SECTION("Single dimension")
    {
        const std::vector<std::vector<int>> input{{1, 2, 3}};
        const std::vector<std::vector<int>> expected{{1}, {2}, {3}};

        auto result = cartesian_combinations(input);

        REQUIRE(result == expected);
    }

    SECTION("Two dimensions")
    {
        const std::vector<std::vector<int>> input{{1, 2}, {3, 4}};
        const std::vector<std::vector<int>> expected{{1, 3}, {1, 4}, {2, 3}, {2, 4}};

        auto result = cartesian_combinations(input);

        REQUIRE(result == expected);
    }

    SECTION("Three dimensions")
    {
        const std::vector<std::vector<int>> input{{1, 2}, {3, 4}, {5, 6}};
        const std::vector<std::vector<int>> expected{{1, 3, 5}, {1, 3, 6}, {1, 4, 5}, {1, 4, 6},
                                                     {2, 3, 5}, {2, 3, 6}, {2, 4, 5}, {2, 4, 6}};

        auto result = cartesian_combinations(input);

        REQUIRE(result == expected);
    }

    SECTION("Empty input")
    {
        const std::vector<std::vector<int>> input{};
        const std::vector<std::vector<int>> expected{{}};

        auto result = cartesian_combinations(input);

        REQUIRE(result == expected);
    }

    SECTION("Empty dimension")
    {
        const std::vector<std::vector<int>> input{{1, 2}, {}};
        const std::vector<std::vector<int>> expected{};

        auto result = cartesian_combinations(input);

        REQUIRE(result == expected);
    }

    SECTION("Mixed types")
    {
        const std::vector<std::vector<std::string>> input{{"a", "b"}, {"x", "y"}};
        const std::vector<std::vector<std::string>> expected{{"a", "x"}, {"a", "y"}, {"b", "x"}, {"b", "y"}};

        auto result = cartesian_combinations(input);

        REQUIRE(result == expected);
    }
}

TEST_CASE("Test the determination of all combinations of distributing k entities on n positions",
          "[determine-all-combinations-of-distributing-k-entities-on-n-positions]")
{
    /** @brief Entity count, position count, and complete ordered combinations. */
    const auto [k, n, expected] =
        GENERATE(Catch::Generators::table<std::size_t, std::size_t, std::vector<std::vector<std::size_t>>>(
            {{0, 0, {}},
             {1, 1, {{0}}},
             {2, 3, {{0, 1}, {0, 2}, {1, 2}}},
             {3,
              5,
              {{0, 1, 2},
               {0, 1, 3},
               {0, 1, 4},
               {0, 2, 3},
               {0, 2, 4},
               {0, 3, 4},
               {1, 2, 3},
               {1, 2, 4},
               {1, 3, 4},
               {2, 3, 4}}},
             {0, 5, {}},
             {5, 5, {{0, 1, 2, 3, 4}}},
             {6, 5, {}}}));
    CAPTURE(k, n);
    CHECK(determine_all_combinations_of_distributing_k_entities_on_n_positions(k, n) == expected);
}

TEST_CASE("Combination enumeration rejects counts beyond vector capacity",
          "[determine-all-combinations-of-distributing-k-entities-on-n-positions]")
{
    CHECK_THROWS_AS(determine_all_combinations_of_distributing_k_entities_on_n_positions(34, 68), std::length_error);
}

TEST_CASE("Binomial coefficients avoid intermediate overflow and saturate", "[binomial-coefficient]")
{
    constexpr auto max_count = std::numeric_limits<uint64_t>::max();
    CHECK(binomial_coefficient(67, 33) == 14'226'520'737'620'288'370ULL);
    CHECK(binomial_coefficient(68, 34) == max_count);
    CHECK(binomial_coefficient(max_count, 1) == max_count);
    CHECK(binomial_coefficient(max_count, max_count) == 1);
}
