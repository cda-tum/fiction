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
 * @brief Simulated Annealing with linear and geometric temperature schedules.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <random>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction::utils::optimization
{

/**
 * A linearly decreasing temperature schedule. The temperature is altered in decrements of `10`.
 *
 * @param t The current temperature.
 * @return The next temperature, i.e. \f$\texttt{t} - 10\f$.
 */
constexpr auto linear_temperature_schedule(const double t) noexcept
{
    return t - 10.0;
}
/**
 * A logarithmically decreasing temperature schedule. The temperature is altered by multiplying it with `0.99`.
 *
 * @param t The current temperature.
 * @return The next temperature, i.e. \f$\texttt{t} \cdot 0.99\f$.
 */
constexpr auto geometric_temperature_schedule(const double t) noexcept
{
    return t * 0.99;
}

/**
 * Simulated Annealing (SA) is a probabilistic optimization algorithm that is used to find a local minimum of a given
 * function. SA was first proposed in \"Optimization by simulated annealing\" by S. Kirkpatrick, C. D. Gelatt Jr, and M.
 * P. Vecchi in Science 1983. It is a metaheuristic that is inspired by the annealing process in metallurgy. The
 * algorithm starts with a random state and iteratively improves the state by randomly selecting a neighboring state. If
 * the neighboring state is better than the current state, it is accepted. If the neighboring state is worse than the
 * current state, it is accepted with a probability that decreases over time. The algorithm stops when the temperature
 * reaches a certain threshold.
 *
 * Some pre-defined temperature schedules are provided in this header file.
 *
 * This implementation is based on:
 * https://codereview.stackexchange.com/questions/70310/simple-simulated-annealing-template-in-c11
 *
 * @tparam State The state type.
 * @tparam CostFunc Lvalue-invocable cost function returning a signed arithmetic value for const and mutable states.
 * @tparam TempFunc Lvalue-invocable temperature schedule returning exactly double.
 * @tparam NextFunc Lvalue-invocable next-state function returning exactly the state type.
 * @param init_state The initial state to optimize.
 * @param init_temp The initial temperature.
 * @param final_temp The final temperature.
 * @param cycles The number of cycles for each temperature value.
 * @param cost The cost function to minimize.
 * @param schedule The temperature schedule.
 * @param next The next state function that determines an adjacent state given a current one.
 * @return A pair of the optimized state and its cost value.
 */
// NOLINTBEGIN(cppcoreguidelines-missing-std-forward): the callables are invoked once per annealing
// cycle, so forwarding them would move from them on the first call
template <typename State, typename CostFunc, typename TempFunc, typename NextFunc>
    requires std::invocable<CostFunc&, const State&> && std::invocable<CostFunc&, State&> &&
             std::invocable<TempFunc&, double&> && std::invocable<NextFunc&, State&> &&
             std::is_signed_v<std::invoke_result_t<CostFunc&, const State&>> &&
             std::is_signed_v<std::invoke_result_t<CostFunc&, State&>> &&
             std::same_as<std::invoke_result_t<TempFunc&, double&>, double> &&
             std::same_as<std::invoke_result_t<NextFunc&, State&>, State>
std::pair<State, std::invoke_result_t<CostFunc&, const State&>>
simulated_annealing(const State& init_state, const double init_temp, const double final_temp, const std::size_t cycles,
                    CostFunc&& cost, TempFunc&& schedule, NextFunc&& next) noexcept
// NOLINTEND(cppcoreguidelines-missing-std-forward)
{
    assert(std::isfinite(init_temp) && "init_temp must be a finite number");
    assert(std::isfinite(final_temp) && "final_temp must be a finite number");

    static std::mt19937_64 generator{std::random_device{}()};

    static std::uniform_real_distribution<double> random_functor(0, 1);

    auto current_cost  = cost(init_state);
    auto current_state = init_state;

    State best_state = current_state;
    auto  best_cost  = current_cost;

    auto temp = init_temp;

    while (temp > final_temp)
    {
        for (std::size_t c = 0; c < cycles; ++c)
        {
            State new_state = next(current_state);
            auto  new_cost  = cost(new_state);

            if (new_cost < best_cost)
            {
                best_state    = new_state;
                best_cost     = new_cost;
                current_state = std::move(new_state);
                current_cost  = std::move(new_cost);

                continue;
            }

            const auto cost_delta = static_cast<double>(new_cost - current_cost);

            // shortcut to skip the expensive std::exp call
            if (cost_delta > 10.0 * temp)
            {
                continue;  // as std::exp(-10.0) is a very small number
            }

            // if the new state is worse, accept it with a probability of exp(-energy_delta/temp)
            if (cost_delta <= 0.0 || std::exp(-cost_delta / temp) > random_functor(generator))
            {
                current_state = std::move(new_state);
                current_cost  = std::move(new_cost);
            }
        }

        // update temperature
        temp = std::clamp(schedule(temp), final_temp, init_temp);
    }

    return {best_state, best_cost};
}
/**
 * This variation of Simulated Annealing (SA) does not start from just one provided initial state, but generates a
 * number of random initial states using a provided random state generator. SA as specified above is then run on all
 * these random initial states where the best result of all generated states is finally returned.
 *
 * Each initial state runs in its own thread.
 *
 * @note The State type must be default constructible.
 *
 * @tparam RandStateFunc Lvalue-invocable generator returning a default-constructible state.
 * @tparam CostFunc Cost callable accepted by single-state annealing.
 * @tparam TempFunc Lvalue-invocable temperature schedule returning exactly double.
 * @tparam NextFunc Lvalue-invocable next-state function returning exactly the state type.
 * @param init_temp The initial temperature.
 * @param final_temp The final temperature.
 * @param cycles The number of cycles for each temperature value.
 * @param instances The number of random initial states to generate.
 * @param rand_state The random state generator function.
 * @param cost The cost function to minimize.
 * @param schedule The temperature schedule.
 * @param next The next state function that determines an adjacent state given a current one.
 * @return A pair of the overall best optimized state and its cost value.
 */
// NOLINTBEGIN(cppcoreguidelines-missing-std-forward): the callables are shared by every annealing
// instance, so forwarding them would move from them on the first one
template <typename RandStateFunc, typename CostFunc, typename TempFunc, typename NextFunc>
    requires std::invocable<RandStateFunc&> && std::is_default_constructible_v<std::invoke_result_t<RandStateFunc&>> &&
             requires(RandStateFunc& rand_state, CostFunc& cost, TempFunc& schedule, NextFunc& next) {
                 simulated_annealing(rand_state(), 0.0, 0.0, std::size_t{}, cost, schedule, next);
             }
std::pair<std::invoke_result_t<RandStateFunc&>,
          std::invoke_result_t<CostFunc&, const std::invoke_result_t<RandStateFunc&>&>>
multi_simulated_annealing(const double init_temp, const double final_temp, const std::size_t cycles,
                          const std::size_t instances, RandStateFunc&& rand_state, CostFunc&& cost, TempFunc&& schedule,
                          NextFunc&& next) noexcept
// NOLINTEND(cppcoreguidelines-missing-std-forward)
{
    using state_t = std::invoke_result_t<RandStateFunc&>;
    using cost_t  = std::invoke_result_t<CostFunc&, const state_t&>;

    assert(std::isfinite(init_temp) && "init_temp must be a finite number");
    assert(std::isfinite(final_temp) && "final_temp must be a finite number");

    std::vector<std::pair<state_t, cost_t>> results(instances);
    std::vector<std::thread>                threads{};
    threads.reserve(instances);

    // Function to perform simulated annealing and store the result in the results vector
    const auto perform_simulated_annealing =
        [&results, &init_temp, &final_temp, &cycles, &rand_state, &cost, &schedule, &next](const std::size_t index)
    { results[index] = simulated_annealing(rand_state(), init_temp, final_temp, cycles, cost, schedule, next); };

    // Start threads
    for (std::size_t i = 0; i < instances; i++)
    {
        threads.emplace_back(perform_simulated_annealing, i);
    }

    // Join threads (wait for all threads to finish)
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Find the minimum result
    return *std::min_element(results.cbegin(), results.cend(),
                             [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
}

}  // namespace fiction::utils::optimization
