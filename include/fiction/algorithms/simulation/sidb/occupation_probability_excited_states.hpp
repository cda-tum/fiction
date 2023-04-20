//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_PROBABILITY_EXCITED_STATES_HPP
#define FICTION_OCCUPATION_PROBABILITY_EXCITED_STATES_HPP

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
 * This function computes the occupation probability of all erroneous charge distribution states at a given temperature
 * if `gate_based_simulation` is set to `true` (default). If not, the occupation probability of all excited states is
 * computed.
 *
 * @param energy_distribution This vector contains the energies of all possible charge distributions with the
 * information if state is transparent or erroneous.
 * @param temperature System temperature to assume.
 * @param erroneous_excited Flag to indicate that the critical temperature is determined for a logic gate. `true` is
 * used (recommended) for gates. `false` is required for arbitrary layouts with no underlying logic.
 * @return The occupation probability of all erroneous or excited states is returned.
 */
[[nodiscard]] inline double
occupation_probability_gate_based(const std::vector<std::pair<double, bool>>& energy_distribution,
                                  const double                                temperature) noexcept
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

    // All Boltzmann factors of the (erroneous) excited states are summed.
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

[[nodiscard]] inline double occupation_probability_non_gate_based(const std::map<double, uint64_t>& energy_distribution,
                                                                  const double temperature) noexcept
{
    assert(!energy_distribution.empty() && "vector is empty");
    assert((temperature > static_cast<double>(0)) && "temperature should be slightly above 0 K");

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

    // All Boltzmann factors of the (erroneous) excited states are summed.
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
