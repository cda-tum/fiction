//
// Created by Jan Drewniok on 17.01.23.
//

#ifndef FICTION_ENERGY_DISTRIBUTION_HPP
#define FICTION_ENERGY_DISTRIBUTION_HPP

#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/utils/math_utils.hpp"

#include <cmath>
#include <cstdint>
#include <map>
#include <vector>

namespace fiction
{

/**
 *  Data type to collect electrostatic potential energies of charge distributions with corresponding degeneracy (i.e.
 * how often a certain energy value occurs).
 */
using sidb_energy_distribution = std::map<double, uint64_t>;

/**
 * This function takes in a vector of charge_distribution_surface objects and returns a map containing the system energy
 * and the number of occurrences of that energy in the input vector.
 *
 * @tparam Lyt Cell-level layout type.
 * @param input_vec A vector of charge_distribution_surface objects for which statistics are to be computed.
 * @return A map containing the system energy as the key and the number of occurrences of that energy in the input
 * vector as the value.
 */
template <typename Lyt>
[[nodiscard]] sidb_energy_distribution
energy_distribution(const std::vector<charge_distribution_surface<Lyt>>& input_vec) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    std::map<double, uint64_t> distribution{};

    for (const auto& lyt : input_vec)
    {
        const auto energy = round_to_n_decimal_places(lyt.get_system_energy(), 6);  // rounding to 6 decimal places.

        distribution[energy]++;
    }

    return distribution;
}

}  // namespace fiction

#endif  // FICTION_ENERGY_DISTRIBUTION_HPP
