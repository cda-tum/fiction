//
// Created by marcel on 12.12.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/optimization/simulated_annealing.hpp>

#include <algorithm>
#include <random>
#include <iostream>

using namespace fiction;

constexpr double schwefel_function_1d(const double& x) noexcept
{
    return 418.9829 - x * std::sin(std::sqrt(std::abs(x)));
}

double random_next(const double& x) noexcept
{
    static std::mt19937_64 generator{std::random_device{}()};

    std::uniform_real_distribution<double> distribution{x - 10, x + 10};

    return distribution(generator);
}

TEST_CASE("Simulated Annealing for optimizing the 1D Schwefel function", "[sim-anneal]")
{
    const auto init_state = 0.0;
    const auto init_temp  = 100.0;
    const auto final_temp = 10.0;
    const auto cycles     = 1000u;

    SECTION("Linear temperature schedule")
    {
        const auto result = simulated_annealing(init_state, init_temp, final_temp, cycles, schwefel_function_1d,
                                                linear_temperature_schedule, random_next);

        CHECK(schwefel_function_1d(result) < schwefel_function_1d(init_state));

        std::cout << "Result: " << result << "with Schwefel(x) = " << schwefel_function_1d(result) << std::endl;
    }
//    SECTION("Geometric temperature schedule")
//    {
//        const auto result = simulated_annealing(init_state, init_temp, final_temp, cycles, schwefel_function_1d,
//                                                geometric_temperature_schedule, random_next);
//
//        CHECK(schwefel_function_1d(result) < schwefel_function_1d(init_state));
//    }
//    SECTION("Slow temperature schedule")
//    {
//        const auto result = simulated_annealing(init_state, init_temp, final_temp, cycles, schwefel_function_1d,
//                                                slow_temperature_schedule, random_next);
//
//        CHECK(schwefel_function_1d(result) < schwefel_function_1d(init_state));
//    }
}