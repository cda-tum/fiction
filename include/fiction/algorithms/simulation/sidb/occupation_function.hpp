//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_FUNCTION_HPP
#define FICTION_OCCUPATION_FUNCTION_HPP

#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <vector>

namespace fiction
{

/**
 * This function computes the occupation probability of all erroneous charge distribution states at a given temperature
 * if flag is set to true. If not, the occupation probability of all excited states is computed.
 *
 * @param energy_distribution This vector contains the energies of all possible charge
 * distributions with the information if state is transparent or erroneous.
 * @param temperature System temperature
 * @param erroneous_excited Flag to indicate that the critical temperature is determined for a logic gate. `True` is
 * used (recommended) for gates. `False` is required for arbitrary layouts with no underlying logic.
 * @return The occupation probability of all erroneous or excited states is returned.
 */
double occupation_propability(const std::vector<std::pair<double, bool>>& energy_distribution,
                              const double& temperature, bool erroneous_excited = true)
{

    assert(!energy_distribution.empty() && "vector is empty");
    assert((temperature > static_cast<double>(0)) && "temperature should be slightly above 0 K");

    double min_energy = energy_distribution.begin()->first;

    // The partition function is obtained by summing up all the Boltzmann factors.
    double part_func = std::accumulate(energy_distribution.begin(), energy_distribution.end(), 0.0,
                                       [&](double sum, const auto& it)
                                       { return sum + std::exp(-(it.first - min_energy) * 12000 / temperature); });

    // All Boltzmann factors of the erroneous states are summed.
    double p = 0;

    if (erroneous_excited)
    {
        for (const auto& [energies, state_transparent_erroneous] : energy_distribution)
        {
            if (!state_transparent_erroneous)
            {
                p += std::exp(-(energies - min_energy) * 12000 / temperature);
            }
        }
        return p / part_func;  // occupation probability of the erroneous states.
    }

    for (const auto& [energies, state_transparent_erroneous] : energy_distribution)
    {
        if ((std::round(energies * 1'000'000) / 1'000'000) != ((std::round(min_energy * 1'000'000)) / 1'000'000))
        {
            p += std::exp(-(energies - min_energy) * 12000 / temperature);
        }
    }
    return p / part_func;  // occupation probability of the erroneous states.
}

}  // namespace fiction

#endif  // FICTION_OCCUPATION_FUNCTION_HPP
