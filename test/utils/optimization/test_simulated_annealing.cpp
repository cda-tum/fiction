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
 * @brief Tests for `fiction/utils/optimization/simulated_annealing.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/utils/optimization/simulated_annealing.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>

using namespace fiction;
using namespace fiction::utils::optimization;

/** @brief Signed floating-point cost callable on a named lvalue. */
struct lvalue_annealing_cost
{
    /** @brief Returns the state's signed cost. @param state State to evaluate. @return State value. */
    double operator()(const int& state) &
    {
        return static_cast<double>(state);
    }
};

/** @brief Cost callable only as an rvalue, unlike the algorithm's named callable parameters. */
struct rvalue_annealing_cost
{
    /** @brief Returns a signed cost. @param state State to evaluate. @return State value. */
    double operator()(const int& state) &&
    {
        return static_cast<double>(state);
    }
};

/** @brief Generated state without a default constructor. */
struct nondefault_annealing_state
{
    /** @brief Requires an explicit initial state value. @param value Initial value. */
    explicit nondefault_annealing_state([[maybe_unused]] int value) {}
};

/**
 * @brief Checks the single-state annealing callable contract.
 * @tparam Cost Cost callable type.
 * @tparam Schedule Temperature schedule type.
 * @tparam Next Next-state callable type.
 */
template <typename Cost, typename Schedule, typename Next>
concept annealable = requires(Cost&& cost, Schedule&& schedule, Next&& next) {
    simulated_annealing(0, 2.0, 1.0, 1, std::forward<Cost>(cost), std::forward<Schedule>(schedule),
                        std::forward<Next>(next));
};

/**
 * @brief Checks the generated-state annealing callable contract.
 * @tparam Generator State generator type.
 * @tparam Cost Cost callable type.
 * @tparam Schedule Temperature schedule type.
 * @tparam Next Next-state callable type.
 */
template <typename Generator, typename Cost, typename Schedule, typename Next>
concept multi_annealable = requires(Generator&& generator, Cost&& cost, Schedule&& schedule, Next&& next) {
    multi_simulated_annealing(2.0, 1.0, 1, 1, std::forward<Generator>(generator), std::forward<Cost>(cost),
                              std::forward<Schedule>(schedule), std::forward<Next>(next));
};

TEST_CASE("Annealing accepts signed floating-point costs and actual callable forms", "[sim-anneal]")
{
    const auto cost      = [](const auto&) { return 0.0; };
    const auto schedule  = [](double) { return 1.0; };
    const auto next      = [](const auto& state) { return state; };
    const auto generator = [] { return 0; };
    CHECK((annealable<decltype(cost), decltype(schedule), decltype(next)>));
    CHECK((annealable<lvalue_annealing_cost, decltype(schedule), decltype(next)>));
    CHECK_FALSE((annealable<rvalue_annealing_cost, decltype(schedule), decltype(next)>));
    CHECK_FALSE((annealable<int, decltype(schedule), decltype(next)>));
    CHECK_FALSE((annealable<decltype([](int) { return 0u; }), decltype(schedule), decltype(next)>));
    CHECK_FALSE((annealable<decltype(cost), decltype([](double) { return 1.0f; }), decltype(next)>));
    CHECK_FALSE((annealable<decltype(cost), decltype(schedule), decltype([](int) { return 0.0; })>));
    CHECK((multi_annealable<decltype(generator), decltype(cost), decltype(schedule), decltype(next)>));
    CHECK((multi_annealable<decltype(generator), lvalue_annealing_cost, decltype(schedule), decltype(next)>));
    CHECK_FALSE((multi_annealable<decltype(generator), rvalue_annealing_cost, decltype(schedule), decltype(next)>));
    CHECK_FALSE(
        (multi_annealable<decltype(generator), decltype([](int) { return 0u; }), decltype(schedule), decltype(next)>));
    CHECK_FALSE(
        (multi_annealable<decltype(generator), decltype(cost), decltype([](double) { return 1.0f; }), decltype(next)>));
    CHECK_FALSE(
        (multi_annealable<decltype(generator), decltype(cost), decltype(schedule), decltype([](int) { return 0.0; })>));
    CHECK_FALSE((multi_annealable<int, decltype(cost), decltype(schedule), decltype(next)>));
    CHECK_FALSE((multi_annealable<decltype([] { return nondefault_annealing_state{0}; }), decltype(cost),
                                  decltype(schedule), decltype(next)>));

    const auto result =
        simulated_annealing(0, 2.0, 1.0, 1, lvalue_annealing_cost{}, schedule, [](int& state) { return state - 1; });
    CHECK(result.first == -1);
    CHECK(result.second == -1.0);
}

/**
 * The 1D Schwefel function is a non-convex function with many local minima. It is used to test optimization algorithms.
 * This definition is taken from: https://www.sfu.ca/~ssurjano/schwef.html
 *
 * @param x Value to evaluate.
 * @return Schwefel function value.
 */
double schwefel_function_1d(const double& x) noexcept
{
    return 418.9829 - x * std::sin(std::sqrt(std::abs(x)));
}
/**
 * This function implements a randomized initial state generator for the 1D Schwefel function. It returns a random value
 * in the range [-500, 500].
 */
double schwefel_1d_init_state_generator() noexcept
{
    static std::mt19937_64 generator(std::random_device{}());

    static std::uniform_real_distribution<double> distribution(-500.0, 500.0);

    return distribution(generator);
}
/**
 * This function implements a randomized adjacent state generator for the 1D Schwefel function. Within a range of
 * +/- 100 of the current state, a new state is randomly generated.
 *
 * @param x The current state.
 * @return A randomized adjacent state.
 */
double random_next_schwefel(const double& x) noexcept
{
    static std::mt19937_64 generator{std::random_device{}()};

    std::uniform_real_distribution<double> distribution{x - 100, x + 100};

    return std::clamp(distribution(generator), -500.0, 500.0);
}

// Test the Schwefel implementation
TEST_CASE("1D Schwefel function", "[sim-anneal]")
{
    const auto optimum = schwefel_function_1d(420.9687);

    REQUIRE_THAT(optimum, Catch::Matchers::WithinAbs(0.0, 0.0001));
}

/**
 * The 2D Drop-Wave function is a non-convex function with many local minima. It is used to test optimization
 * algorithms. This definition is taken from: https://www.sfu.ca/~ssurjano/drop.html.
 *
 * @param x The value to evaluate.
 * @return The Drop-Wave function value.
 */
double drop_wave_function_2d(const std::pair<double, double>& x) noexcept
{
    return -((1.0 + std::cos(12.0 * std::sqrt(x.first * x.first + x.second * x.second))) /
             (0.5 * (x.first * x.first + x.second * x.second) + 2.0));
}
/**
 * This function implements a randomized initial state generator for the 2D Drop-Wave function. It returns a random pair
 * in the range [-5.12, 5.12].
 */
std::pair<double, double> drop_wave_2d_init_state_generator() noexcept
{
    static std::mt19937_64 generator(std::random_device{}());

    static std::uniform_real_distribution<double> distribution(-5.12, 5.12);

    return {distribution(generator), distribution(generator)};
}
/**
 * This function implements a randomized adjacent state generator for the 2D Drop-Wave function. Within a range of
 * +/- 1 of each of the current state's dimensions, a new state is randomly generated.
 *
 * @param x The current state.
 * @return A randomized adjacent state.
 */
std::pair<double, double> random_next_drop_wave(const std::pair<double, double>& x) noexcept
{
    static std::mt19937_64 generator{std::random_device{}()};

    std::uniform_real_distribution<double> distribution_x{x.first - 1, x.first + 1};
    std::uniform_real_distribution<double> distribution_y{x.second - 1, x.second + 1};

    return {std::clamp(distribution_x(generator), -5.12, 5.12), std::clamp(distribution_y(generator), -5.12, 5.12)};
}

// Test the Drop-Wave implementation
TEST_CASE("2D Drop-Wave function", "[sim-anneal]")
{
    const auto optimum = drop_wave_function_2d({0.0, 0.0});

    REQUIRE_THAT(optimum, Catch::Matchers::WithinAbs(-1.0, 0.00001));
}

// Simulated Annealing tests

TEST_CASE("Simulated Annealing for optimizing the 1D Schwefel function", "[sim-anneal]")
{
    constexpr const auto init_state = 0.0;
    constexpr const auto init_temp  = 5000.0;
    constexpr const auto final_temp = 1.0;
    constexpr const auto cycles     = 10u;
    constexpr const auto instances  = 50u;

    const auto init_cost = schwefel_function_1d(init_state);

    SECTION("Linear temperature schedule")
    {
        const auto [single_result, single_cost] =
            simulated_annealing(init_state, init_temp, final_temp, cycles, schwefel_function_1d,
                                linear_temperature_schedule, random_next_schwefel);

        const auto [multi_result, multi_cost] =
            multi_simulated_annealing(init_temp, final_temp, cycles, instances, schwefel_1d_init_state_generator,
                                      schwefel_function_1d, linear_temperature_schedule, random_next_schwefel);

        CHECK(single_cost < init_cost);
        CHECK(multi_cost < init_cost);

        CHECK_THAT(schwefel_function_1d(single_result), Catch::Matchers::WithinAbs(single_cost, 0.00001));
        CHECK_THAT(schwefel_function_1d(multi_result), Catch::Matchers::WithinAbs(multi_cost, 0.00001));

        std::cout << fmt::format("Linear single_result: {}; Schwefel(x) = {}", single_result,
                                 schwefel_function_1d(single_result))
                  << std::endl;
        std::cout << fmt::format("Linear multi_result: {}; Schwefel(x) = {}", multi_result,
                                 schwefel_function_1d(multi_result))
                  << std::endl;
    }
    SECTION("Geometric temperature schedule")
    {
        const auto [single_result, single_cost] =
            simulated_annealing(init_state, init_temp, final_temp, cycles, schwefel_function_1d,
                                geometric_temperature_schedule, random_next_schwefel);

        const auto [multi_result, multi_cost] =
            multi_simulated_annealing(init_temp, final_temp, cycles, instances, schwefel_1d_init_state_generator,
                                      schwefel_function_1d, geometric_temperature_schedule, random_next_schwefel);

        CHECK(single_cost < init_cost);
        CHECK(multi_cost < init_cost);

        CHECK_THAT(schwefel_function_1d(single_result), Catch::Matchers::WithinAbs(single_cost, 0.00001));
        CHECK_THAT(schwefel_function_1d(multi_result), Catch::Matchers::WithinAbs(multi_cost, 0.00001));

        std::cout << fmt::format("Geometric single_result: {}; Schwefel(x) = {}", single_result,
                                 schwefel_function_1d(single_result))
                  << std::endl;

        std::cout << fmt::format("Geometric multi_result: {}; Schwefel(x) = {}", multi_result,
                                 schwefel_function_1d(multi_result))
                  << std::endl;
    }
}
TEST_CASE("Simulated Annealing for optimizing the 2D Drop-Wave function", "[sim-anneal]")
{
    constexpr const auto init_state = std::pair{2.5, -2.5};
    constexpr const auto init_temp  = 5000.0;
    constexpr const auto final_temp = 1.0;
    constexpr const auto cycles     = 10u;
    constexpr const auto instances  = 50u;

    const auto init_cost = drop_wave_function_2d(init_state);

    SECTION("Linear temperature schedule")
    {
        const auto [single_result, single_cost] =
            simulated_annealing(init_state, init_temp, final_temp, cycles, drop_wave_function_2d,
                                linear_temperature_schedule, random_next_drop_wave);

        const auto [multi_result, multi_cost] =
            multi_simulated_annealing(init_temp, final_temp, cycles, instances, drop_wave_2d_init_state_generator,
                                      drop_wave_function_2d, linear_temperature_schedule, random_next_drop_wave);

        CHECK(single_cost < init_cost);
        CHECK(multi_cost < init_cost);

        CHECK_THAT(drop_wave_function_2d(single_result), Catch::Matchers::WithinAbs(single_cost, 0.00001));
        CHECK_THAT(drop_wave_function_2d(multi_result), Catch::Matchers::WithinAbs(multi_cost, 0.00001));

        std::cout << fmt::format("Linear single_result: ({}, {}); Drop-Wave(x) = {}", single_result.first,
                                 single_result.second, drop_wave_function_2d(single_result))
                  << std::endl;

        std::cout << fmt::format("Linear multi_result: ({}, {}); Drop-Wave(x) = {}", multi_result.first,
                                 multi_result.second, drop_wave_function_2d(multi_result))
                  << std::endl;
    }
    SECTION("Geometric temperature schedule")
    {
        const auto [single_result, single_cost] =
            simulated_annealing(init_state, init_temp, final_temp, cycles, drop_wave_function_2d,
                                geometric_temperature_schedule, random_next_drop_wave);

        const auto [multi_result, multi_cost] =
            multi_simulated_annealing(init_temp, final_temp, cycles, instances, drop_wave_2d_init_state_generator,
                                      drop_wave_function_2d, geometric_temperature_schedule, random_next_drop_wave);

        CHECK(single_cost < init_cost);
        CHECK(multi_cost < init_cost);

        CHECK_THAT(drop_wave_function_2d(single_result), Catch::Matchers::WithinAbs(single_cost, 0.00001));
        CHECK_THAT(drop_wave_function_2d(multi_result), Catch::Matchers::WithinAbs(multi_cost, 0.00001));

        std::cout << fmt::format("Geometric single_result: ({}, {}); Drop-Wave(x) = {}", single_result.first,
                                 single_result.second, drop_wave_function_2d(single_result))
                  << std::endl;

        std::cout << fmt::format("Geometric multi_result: ({}, {}); Drop-Wave(x) = {}", multi_result.first,
                                 multi_result.second, drop_wave_function_2d(multi_result))
                  << std::endl;
    }
}

TEST_CASE("Multi-state annealing rejects zero instances", "[sim-anneal]")
{
    CHECK_THROWS_AS(multi_simulated_annealing(2.0, 1.0, 1, 0, schwefel_1d_init_state_generator, schwefel_function_1d,
                                              linear_temperature_schedule, random_next_schwefel),
                    std::invalid_argument);
}
