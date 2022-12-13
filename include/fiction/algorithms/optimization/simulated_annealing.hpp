//
// Created by marcel on 12.12.22.
//

#ifndef FICTION_SIMULATED_ANNEALING_HPP
#define FICTION_SIMULATED_ANNEALING_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <utility>

namespace fiction
{

constexpr auto linear_temperature_schedule(const double t) noexcept
{
    return t - 10.0;
}

constexpr auto geometric_temperature_schedule(const double t) noexcept
{
    return t * 0.9;
}

constexpr auto slow_temperature_schedule(const double t) noexcept
{
    return t / 1 + 0.5 * t;
}

/**
 * Simulated annealing is a probabilistic optimization algorithm that is used to find a local minimum of a given
 * function.
 *
 * This implementation is based on:
 * https://codereview.stackexchange.com/questions/70310/simple-simulated-annealing-template-in-c11
 * @tparam State
 * @tparam CostFunc
 * @tparam TempFunc
 * @tparam NextFunc
 * @param init_state
 * @param init_temp
 * @param final_temp
 * @param cycles
 * @param cost
 * @param schedule
 * @param next
 * @return
 */
template <typename State, typename CostFunc, typename TempFunc, typename NextFunc>
State simulated_annealing(const State& init_state, const double init_temp, const double final_temp,
                          const std::size_t cycles, CostFunc&& cost, TempFunc&& schedule, NextFunc&& next) noexcept
{
    assert(std::isfinite(init_temp) && "init_temp must be a finite number");
    assert(std::isfinite(final_temp) && "final_temp must be a finite number");

    std::mt19937_64 generator{std::random_device{}()};

    auto current_cost  = cost(init_state);
    auto current_state = init_state;

    State best_state = current_state;
    auto  best_cost  = current_cost;

    auto temp = init_temp;

    std::uniform_real_distribution<decltype(current_cost)> random_functor(0, 1);

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

            temp = schedule(temp);

            const auto cost_delta = new_cost - current_cost;

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
    }

    return best_state;
}

}  // namespace fiction

#endif  // FICTION_SIMULATED_ANNEALING_HPP
