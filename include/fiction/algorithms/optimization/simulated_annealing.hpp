//
// Created by marcel on 12.12.22.
//

#ifndef FICTION_SIMULATED_ANNEALING_HPP
#define FICTION_SIMULATED_ANNEALING_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/execution_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction
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
 * @tparam CostFunc The cost function type (specifies the cost type via its return value).
 * @tparam TempFunc The temperature schedule function type.
 * @tparam NextFunc The next state function type.
 * @param init_state The initial state to optimize.
 * @param init_temp The initial temperature.
 * @param final_temp The final temperature.
 * @param cycles The number of cycles for each temperature value.
 * @param cost The cost function to minimize.
 * @param schedule The temperature schedule.
 * @param next The next state function that determines an adjacent state given a current one.
 * @return A pair of the optimized state and its cost value.
 */
template <typename State, typename CostFunc, typename TempFunc, typename NextFunc>
std::pair<State, std::invoke_result_t<CostFunc, State>>
simulated_annealing(const State& init_state, const double init_temp, const double final_temp, const std::size_t cycles,
                    CostFunc&& cost, TempFunc&& schedule, NextFunc&& next) noexcept
{
    static_assert(std::is_invocable_v<CostFunc, State>, "CostFunc must be invocable with objects of type State");
    static_assert(std::is_invocable_v<TempFunc, double>, "TempFunc must be invocable with double");
    static_assert(std::is_invocable_v<NextFunc, State>, "NextFunc must be invocable with objects of type State");
    static_assert(std::is_signed_v<std::invoke_result_t<CostFunc, State>>, "CostFunc must return a signed value");
    static_assert(std::is_same_v<std::invoke_result_t<TempFunc, double>, double>, "TempFunc must return a double");
    static_assert(std::is_same_v<State, std::invoke_result_t<NextFunc, State>>,
                  "NextFunc must return an object of type State");

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
                best_state = new_state;
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
 * @note If compiler support for C++17's execution policies is available, the algorithm is parallelized and/or
 * vectorized using `std::execution::par_unseq`.
 *
 * @note The State type must be default constructible.
 *
 * @tparam RandStateFunc The random state generator function type (specifies the State type via its return value).
 * @tparam CostFunc The cost function type (specifies the cost value via its return value).
 * @tparam TempFunc The temperature schedule function type.
 * @tparam NextFunc The next state function type.
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
template <typename RandStateFunc, typename CostFunc, typename TempFunc, typename NextFunc>
std::pair<std::invoke_result_t<RandStateFunc>, std::invoke_result_t<CostFunc, std::invoke_result_t<RandStateFunc>>>
multi_simulated_annealing(const double init_temp, const double final_temp, const std::size_t cycles,
                          const std::size_t instances, RandStateFunc&& rand_state, CostFunc&& cost, TempFunc&& schedule,
                          NextFunc&& next) noexcept
{
    using state_t = std::invoke_result_t<RandStateFunc>;
    using cost_t  = std::invoke_result_t<CostFunc, state_t>;

    static_assert(std::is_invocable_v<RandStateFunc>, "RandStateFunc must be invocable");
    static_assert(std::is_invocable_v<CostFunc, state_t>, "CostFunc must be invocable with objects of type State");
    static_assert(std::is_default_constructible_v<state_t>, "State must be default-constructible");

    assert(std::isfinite(init_temp) && "init_temp must be a finite number");
    assert(std::isfinite(final_temp) && "final_temp must be a finite number");

    std::vector<std::pair<state_t, cost_t>> results(instances);
    std::vector<std::thread>                threads{};
    threads.reserve(instances);

    // Function to perform simulated annealing and store the result in the results vector
    auto perform_simulated_annealing =
        [&results, &init_temp, &final_temp, &cycles, &rand_state, &cost, &schedule, &next](std::size_t index)
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
    return *std::min_element(FICTION_EXECUTION_POLICY_PAR_UNSEQ results.cbegin(), results.cend(),
                             [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
}

}  // namespace fiction

#endif  // FICTION_SIMULATED_ANNEALING_HPP
