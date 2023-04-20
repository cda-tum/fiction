//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_PROBABILITY_EXCITED_STATES_HPP
#define FICTION_OCCUPATION_PROBABILITY_EXCITED_STATES_HPP

#include "fiction/algorithms/simulation/sidb/calculating_energy_and_state_type.hpp"
#include "fiction/utils/math_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <numeric>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This function computes the occupation probability of erroneous charge distributions (output charge does not match the
 * expected output according the truth table) at a given temperature.
 *
 * @param energy_distribution This contains the energies of all possible charge distributions with the
 * information if corresponding state is transparent or erroneous.
 * @param temperature System temperature to assume.
 * @return The occupation probability of all erroneous states is returned.
 */
[[nodiscard]] inline double occupation_probability_gate_based(const energy_and_state_type& energy_distribution,
                                                              const double                 temperature) noexcept
{
    assert(!energy_distribution.empty() && "vector is empty");
    assert((temperature > static_cast<double>(0)) && "temperature should be slightly above 0 K");

    auto min_energy = std::numeric_limits<double>::infinity();

    // Determine the minimal energy.
    for (const auto& [energy, degeneracy] : energy_distribution)
    {
        if (energy < min_energy)
        {
            min_energy = energy;
        }
    }

    // The partition function is obtained by summing up all the Boltzmann factors.
    const double partition_function =
        std::accumulate(energy_distribution.begin(), energy_distribution.end(), 0.0,
                        [&](const double sum, const auto& it)
                        { return sum + std::exp(-(it.first - min_energy) * 12'000 / temperature); });

    // All Boltzmann factors of the erroneous states are summed.
    double p = 0;

    // The Boltzmann factors of all erroneous excited states are accumulated.
    for (const auto& [energies, state_transparent_erroneous] : energy_distribution)
    {
        if (!state_transparent_erroneous)
        {
            p += std::exp(-(energies - min_energy) * 12'000 / temperature);
        }
    }

    return p / partition_function;  // Occupation probability of the erroneous states.
}

/**
 * This function computes the occupation probability of excited states (charge distributions with energy higher than the
 * ground state) at a given temperature.
 *
 * @param energy_distribution This contains the energies of all possible charge distributions with the degeneracy.
 * @param temperature System temperature to assume.
 * @return The total occupation probability of all excited states is returned.
 */
[[nodiscard]] inline double occupation_probability_non_gate_based(const std::map<double, uint64_t>& energy_distribution,
                                                                  const double temperature) noexcept
{
    assert(!energy_distribution.empty() && "Vector is empty");
    assert((temperature > static_cast<double>(0)) && "Temperature should be slightly above 0 K");

    auto min_energy = std::numeric_limits<double>::infinity();

    for (auto it = energy_distribution.begin(); it != energy_distribution.end(); it++)
    {
        if (it->first < min_energy)
        {
            min_energy = it->first;
        }
    }

    // The partition function is obtained by summing up all the Boltzmann factors.
    const double partition_function =
        std::accumulate(energy_distribution.begin(), energy_distribution.end(), 0.0,
                        [&](const double sum, const auto& it)
                        { return sum + std::exp(-(it.first - min_energy) * 12'000 / temperature); });

    // All Boltzmann factors of the excited states are summed.
    double p = 0;

    for (auto it = energy_distribution.begin(); it != energy_distribution.end(); it++)
    {
        // All excited states are collected. States with an energy difference of more than 10E-6 with respect to the
        // ground state are considered as "excited states".
        if (round_to_n_decimal_places(it->first, 6) != round_to_n_decimal_places(min_energy, 6))
        {
            p += std::exp(-(it->first - min_energy) * 12'000 / temperature);
        }
    }

    return p / partition_function;  // Occupation probability of the excited states.
}

}  // namespace fiction

#endif  // FICTION_OCCUPATION_PROBABILITY_EXCITED_STATES_HPP
