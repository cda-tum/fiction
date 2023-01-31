//
// Created by Jan Drewniok on 18.01.23.
//

#ifndef FICTION_MINIMUM_ENERGY_HPP
#define FICTION_MINIMUM_ENERGY_HPP

#include "fiction/technology/charge_distribution_surface.hpp"
#include <vector>
#include <algorithm>
#include <limits>

namespace fiction
{

/**
 * Computes the minimum energy of a vector of charge_distribution_surface objects.
 *
 * @tparam Lyt template parameter for the charge_distribution_surface class.
 * @param result vector of charge_distribution_surface objects.
 * @return double value of the minimum energy found in the input vector.
 */
template <typename Lyt>
double minimum_energy(const std::vector<charge_distribution_surface<Lyt>>& charge_lyts)
{
    return std::accumulate(charge_lyts.begin(), charge_lyts.end(), std::numeric_limits<double>::max(),
                           [](double a, const auto& lyt) { return std::min(a, lyt.get_system_energy()); });
}

}  // namespace fiction

#endif  // FICTION_MINIMUM_ENERGY_HPP
