//
// Created by Jan Drewniok on 18.01.23.
//

#ifndef FICTION_MINIMUM_ENERGY_HPP
#define FICTION_MINIMUM_ENERGY_HPP

#include "fiction/technology/charge_distribution_surface.hpp"

#include <algorithm>
#include <limits>
#include <vector>

namespace fiction
{

/**
 * Computes the minimum energy of a vector of charge_distribution_surface objects.
 *
 * @tparam Lyt Cell-level layout type.
 * @param charge_lyts Vector of charge_distribution_surface objects.
 * @return Value of the minimum energy found in the input vector.
 */
template <typename Lyt>
[[nodiscard]] double minimum_energy(const std::vector<charge_distribution_surface<Lyt>>& charge_lyts) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    return std::accumulate(charge_lyts.cbegin(), charge_lyts.cend(), std::numeric_limits<double>::max(),
                           [](const double a, const auto& lyt) { return std::min(a, lyt.get_system_energy()); });
}

}  // namespace fiction

#endif  // FICTION_MINIMUM_ENERGY_HPP
