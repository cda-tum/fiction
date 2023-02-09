//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_FUNCTION_HPP
#define FICTION_OCCUPATION_FUNCTION_HPP

#include <map>
#include <cmath>
#include <iterator>
#include <iostream>
#include <vector>
#include <numeric>

namespace fiction {

/**
* This function computes the occupation probability of all erroneous charge distribution states at a given temperature.
*
* @param energy_distribution_transparent_erroneous This map holds the energies of all possible charge distributions as keys
* and the values are a pairs with the corresponding degeneracies and the type of excited states (transparent = false, erroneous = true)
* @param ps Physical parameters. They are material-specific and may vary from experiment to experiment.
* @param pst Statistics. They store the simulation results.
* @param confidence_level Confidence level for the presence of a working gate.
* @param max_temperature The maximal critical temperature is set at 400 K.
* @return The criticaltemperature is returned. -10 as return says that either no charge distribution satisfies logic, or at least not the ground state as it should be.
* Changing the physical parameter µ_ might help.
*/
    double occupation_propability_erroneous(
            const std::vector<std::pair<double, bool>> &energy_distribution_transparent_erroneous,
            const double &temperature) {

        assert(!energy_distribution_transparent_erroneous.empty() && "vector is empty");
        assert((temperature > 0) && "temperature should be slightly above 0 K");

        double min_energy = energy_distribution_transparent_erroneous.begin()->first;

        // The partition function is obtained by summing up all the Boltzmann factors.
        double part_func = std::accumulate(energy_distribution_transparent_erroneous.begin(),
                                           energy_distribution_transparent_erroneous.end(), 0.0,
                                           [&](double sum, const auto &it) {
                                               return sum + std::exp(-(it.first - min_energy) * 12000 / temperature);
                                           });

        // All Boltzmann factors of the erroneous states are summed.
        double p = 0;
        for (const auto &[energies, state_transparent_erroneous]: energy_distribution_transparent_erroneous) {
            if (!state_transparent_erroneous) {
                p += std::exp(-(energies - min_energy) * 12000 / temperature);
            }
        }
        return p / part_func; // occupation probability of the erroneous states.
    }

} // namespace fiction

#endif  // FICTION_OCCUPATION_FUNCTION_HPP
