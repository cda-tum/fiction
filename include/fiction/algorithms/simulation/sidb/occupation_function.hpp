//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_FUNCTION_HPP
#define FICTION_OCCUPATION_FUNCTION_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This function computes the occupation probability of all erroneous charge distribution states at a given temperature
 * if flag is set to true (default). If not, the occupation probability of all excited states is computed.
 *
 * @param energy_distribution This vector contains the energies of all possible charge
 * distributions with the information if state is transparent or erroneous.
 * @param temperature System temperature
 * @param erroneous_excited Flag to indicate that the critical temperature is determined for a logic gate. `True` is
 * used (recommended) for gates. `False` is required for arbitrary layouts with no underlying logic.
 * @return The occupation probability of all erroneous or excited states is returned.
 */
[[nodiscard]] inline double occupation_probability(const std::vector<std::pair<double, bool>>& energy_distribution,
                                                   const double temperature, bool erroneous_excited = true) noexcept
{
    assert(!energy_distribution.empty() && "vector is empty");
    assert((temperature > static_cast<double>(0)) && "temperature should be slightly above 0 K");

    auto min_energy = std::numeric_limits<double>::max();
    // Determine the minimal energy.
    for (const auto& [energy, degeneracy] : energy_distribution)
    {
        if (energy < min_energy)
        {
            min_energy = energy;
        }
    }

    // The partition function is obtained by summing up all the Boltzmann factors.
    const double partition_function = std::accumulate(
        energy_distribution.begin(), energy_distribution.end(), 0.0,
        [&](double sum, const auto& it) { return sum + std::exp(-(it.first - min_energy) * 12000 / temperature); });

    // All Boltzmann factors of the (erroneous) excited states are summed.
    double p = 0;

    if (erroneous_excited)
    {

        for (const auto& [energies, state_transparent_erroneous] : energy_distribution)
        {
            if (!state_transparent_erroneous)  // The Boltzmann factors of all erroneous excited states are collected
                                               // and summed up.
            {
                p += std::exp(-(energies - min_energy) * 12000 / temperature);
            }
        }

        return p / partition_function;  // Occupation probability of the erroneous states.
    }

    for (const auto& [energies, state_transparent_erroneous] : energy_distribution)
    {
        // All excited states are collected. States with an energy difference of more than 10E-6 with respect to the
        // ground state are considered as "excited states".
        if ((std::round(energies * 1'000'000) / 1'000'000) != ((std::round(min_energy * 1'000'000)) / 1'000'000))
        {
            p += std::exp(-(energies - min_energy) * 12000 / temperature);
        }
    }

    return p / partition_function;  // Occupation probability of the excited states.
}

}  // namespace fiction

#endif  // FICTION_OCCUPATION_FUNCTION_HPP
