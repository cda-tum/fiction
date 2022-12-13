//
// Created by marcel on 12.12.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/optimization/simulated_annealing.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <iostream>
#include <random>

using namespace fiction;

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
    REQUIRE((optimum <= 0.00001 || optimum >= -0.00001));
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
    CHECK((optimum <= -0.99999 || optimum >= -1.00001));
}

// Simulated Annealing tests

TEST_CASE("Simulated Annealing for optimizing the 1D Schwefel function", "[sim-anneal]")
{
    const auto init_state = 0.0;
    const auto init_temp  = 5000.0;
    const auto final_temp = 1.0;
    const auto cycles     = 10u;

    SECTION("Linear temperature schedule")
    {
        const auto result = simulated_annealing(init_state, init_temp, final_temp, cycles, schwefel_function_1d,
                                                linear_temperature_schedule, random_next_schwefel);

        CHECK(schwefel_function_1d(result) < schwefel_function_1d(init_state));

        std::cout << fmt::format("Linear result: {}; Schwefel(x) = {}", result, schwefel_function_1d(result))
                  << std::endl;
    }
    SECTION("Geometric temperature schedule")
    {
        const auto result = simulated_annealing(init_state, init_temp, final_temp, cycles, schwefel_function_1d,
                                                geometric_temperature_schedule, random_next_schwefel);

        CHECK(schwefel_function_1d(result) < schwefel_function_1d(init_state));

        std::cout << fmt::format("Geometric result: {}; Schwefel(x) = {}", result, schwefel_function_1d(result))
                  << std::endl;
    }
}
TEST_CASE("Simulated Annealing for optimizing the 2D Drop-Wave function", "[sim-anneal]")
{
    const auto init_state = std::pair{2.5, -2.5};
    const auto init_temp  = 5000.0;
    const auto final_temp = 1.0;
    const auto cycles     = 10u;

    SECTION("Linear temperature schedule")
    {
        const auto result = simulated_annealing(init_state, init_temp, final_temp, cycles, drop_wave_function_2d,
                                                linear_temperature_schedule, random_next_drop_wave);

        CHECK(drop_wave_function_2d(result) < drop_wave_function_2d(init_state));

        std::cout << fmt::format("Linear result: ({}, {}); Drop-Wave(x) = {}", result.first, result.second,
                                 drop_wave_function_2d(result))
                  << std::endl;
    }
    SECTION("Geometric temperature schedule")
    {
        const auto result = simulated_annealing(init_state, init_temp, final_temp, cycles, drop_wave_function_2d,
                                                geometric_temperature_schedule, random_next_drop_wave);

        CHECK(drop_wave_function_2d(result) < drop_wave_function_2d(init_state));

        std::cout << fmt::format("Geometric result: ({}, {}); Drop-Wave(x) = {}", result.first, result.second,
                                 drop_wave_function_2d(result))
                  << std::endl;
    }
}
