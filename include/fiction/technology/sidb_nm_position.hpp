//
// Created by Jan Drewniok on 28.01.23.
//

#ifndef FICTION_NM_POSITION_HPP
#define FICTION_NM_POSITION_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/traits.hpp"

#include <utility>

namespace fiction
{

/**
 * Computes the position of a cell in nanometers.
 *
 * @param c The cell to compute the position for.
 * @return A pair of double values representing the `(x,y)` position of the cell in nanometers.
 */
template <typename Lyt>
std::pair<double, double> sidb_nm_position(const sidb_simulation_parameters& sp, const cell<Lyt>& c) noexcept
{
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    const auto x = static_cast<double>(c.x * sp.lat_a);
    const auto y = static_cast<double>(c.y * sp.lat_b + c.z * sp.lat_c);

    return std::make_pair(x, y);
}

}  // namespace fiction

#endif  // FICTION_NM_POSITION_HPP
