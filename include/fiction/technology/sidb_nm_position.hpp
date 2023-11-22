//
// Created by Jan Drewniok on 28.01.23.
//

#ifndef FICTION_NM_POSITION_HPP
#define FICTION_NM_POSITION_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"

#include <utility>

namespace fiction
{

/**
 * Computes the position of a cell in nanometers from the layout origin in an SiDB layout (unit: nm).
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param sp The simulation parameters (required for the lattice constants).
 * @param c The cell to compute the position for.
 * @return A pair representing the `(x,y)` position of `c` in nanometers from the layout origin.
 */
template <typename Lyt>
[[nodiscard]] constexpr std::pair<double, double> sidb_nm_position(const sidb_simulation_parameters& sp,
                                                                   const cell<Lyt>&                  c) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if constexpr (has_siqad_coord_v<Lyt>)
    {
        const auto x = (c.x * sp.lat_a) * 0.1;
        const auto y = (c.y * sp.lat_b + c.z * sp.lat_c) * .1;
        return std::make_pair(x, y);
    }
    else
    {
        const auto cell_in_siqad = siqad::to_siqad_coord(c);
        const auto x             = (cell_in_siqad.x * sp.lat_a) * 0.1;
        const auto y             = (cell_in_siqad.y * sp.lat_b + cell_in_siqad.z * sp.lat_c) * .1;
        return std::make_pair(x, y);
    }
}
}  // namespace fiction

#endif  // FICTION_NM_POSITION_HPP
