//
// Created by Jan Drewniok on 17.01.23.
//

#ifndef FICTION_ENERGY_DISTRIBUTION_HPP
#define FICTION_ENERGY_DISTRIBUTION_HPP

#include "fiction/technology/charge_distribution_surface.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <cmath>

namespace fiction
{

/**
 * This function takes in a vector of charge_distribution_surface objects and returns a map containing the system energy
 * and the number of occurrences of that energy in the input vector.
 * @tparam Lyt The type of the charge_distribution_surface objects in the input vector.
 * @param input_vec A vector of charge_distribution_surface objects for which statistics are to be computed.
 * @return A map containing the system energy as the key and the number of occurrences of that energy in the input
 * vector as the value.
 */
template <typename Lyt>
std::map<double, uint64_t> energy_distribution(const std::vector<charge_distribution_surface<Lyt>>& input_vec)
{
    std::map<double, uint64_t> statistics;
    for (auto& lyt : input_vec)
    {
        const auto energy = std::round(lyt.get_system_energy() * 1000000) / 1000000; // rounding to 6 decimal place.

        statistics[energy]++;
    }
    return statistics;
}
}  // namespace fiction

#endif  // FICTION_ENERGY_DISTRIBUTION_HPP
