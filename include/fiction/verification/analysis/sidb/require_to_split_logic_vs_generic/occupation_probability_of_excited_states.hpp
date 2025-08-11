//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_PROBABILITY_OF_EXCITED_STATES_HPP
#define FICTION_OCCUPATION_PROBABILITY_OF_EXCITED_STATES_HPP

#include "fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/utils/math_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <numeric>
#include <utility>

namespace fiction
{

/**
 * This function computes the occupation probability of erroneous charge distributions (output charge does not match the
 * expected output according the truth table) at a given temperature.
 *
 * @param energy_and_state_type This contains the energies of all possible charge distributions together with the
 * information if the charge distribution (state) is transparent or erroneous.
 * @param temperature System temperature to assume (unit: K).
 * @return The occupation probability of all erroneous states is returned.
 */
[[nodiscard]] inline double occupation_probability_gate_based(const sidb_energy_and_state_type& energy_and_state_type,
                                                              const double                      temperature) noexcept
{
    assert((temperature > 0.0) && "temperature should be slightly above 0 K");

    if (energy_and_state_type.empty())
    {
        return 0.0;
    }

    // Determine the minimal energy.
    const auto [energy, state_type] = *std::min_element(energy_and_state_type.cbegin(), energy_and_state_type.cend(),
                                                        [](const auto& a, const auto& b) { return a.first < b.first; });
    const auto min_energy           = energy;

    // The partition function is obtained by summing up all the Boltzmann factors.
    const double partition_function = std::accumulate(
        energy_and_state_type.cbegin(), energy_and_state_type.cend(), 0.0, [&](const double sum, const auto& it)
        { return sum + std::exp(-((it.first - min_energy) * 12'000 / temperature)); });

    // All Boltzmann factors of the erroneous states are summed.
    double p = 0;

    // The Boltzmann factors of all erroneous excited states are accumulated.
    for (const auto& [energies, state_transparent_erroneous] : energy_and_state_type)
    {
        if (!state_transparent_erroneous)
        {
            p += std::exp(-((energies - min_energy) * 12'000 / temperature));
        }
    }

    return p / partition_function;  // Occupation probability of the erroneous states.
}

/**
 * This function computes the occupation probability of excited states (charge distributions with energy higher than the
 * ground state) at a given temperature.
 *
 * @param energy_distribution This contains the energies in eV of all possible charge distributions with the degeneracy.
 * @param temperature System temperature to assume (unit: K).
 * @return The total occupation probability of all excited states is returned.
 */
[[nodiscard]] inline double occupation_probability_non_gate_based(const sidb_energy_distribution& energy_distribution,
                                                                  const double                    temperature) noexcept
{
    assert((temperature > 0.0) && "Temperature should be slightly above 0 K");

    if (energy_distribution.empty())
    {
        return 0.0;
    }

    const auto& [energy, degeneracy] = *(energy_distribution.begin());
    const auto min_energy            = energy;  // unit: eV

    // The partition function is obtained by summing up all the Boltzmann factors.
    const double partition_function = std::accumulate(
        energy_distribution.cbegin(), energy_distribution.cend(), 0.0, [&](const double sum, const auto& it)
        { return sum + std::exp(-((it.first - min_energy) * 12'000 / temperature)); });

    // All Boltzmann factors of the excited states are summed.
    const double p = std::accumulate(
        energy_distribution.cbegin(), energy_distribution.cend(), 0.0,
        [&](const double sum, const auto& it)
        {
            // round the energy value of the given valid_layout to six decimal places and check if they are different
            if (std::abs(round_to_n_decimal_places(it.first, 6) - round_to_n_decimal_places(min_energy, 6)) >
                physical_constants::POP_STABILITY_ERR)
            {
                return sum + std::exp(-((it.first - min_energy) * 12'000 / temperature));
            }
            return sum;
        });

    return p / partition_function;  // Occupation probability of the excited states.
}

}  // namespace fiction

#endif  // FICTION_OCCUPATION_PROBABILITY_OF_EXCITED_STATES_HPP
