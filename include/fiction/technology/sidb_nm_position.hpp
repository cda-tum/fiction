//
// Created by Jan Drewniok on 28.01.23.
//

#ifndef FICTION_NM_POSITION_HPP
#define FICTION_NM_POSITION_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/sidb_lattice_properties.hpp"
#include "fiction/traits.hpp"

#include <cassert>
#include <utility>

namespace fiction
{

/**
 * Computes the position of a cell in nanometers from the layout origin in an SiDB layout (unit: nm).
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param c The cell to compute the position for.
 * @param orientation The lattice orientation.
 * @param lattice_constants The lattice constants.
 * @return A pair representing the `(x,y)` position of `c` in nanometers from the layout origin.
 */
template <typename Lyt, typename LatticeOrientation = si_lattice_orientations>
[[nodiscard]] constexpr std::pair<double, double>
sidb_nm_position(const cell<Lyt>& c, const LatticeOrientation& orientation = si_lattice_orientations::SI_100,
                 const si_lattice_constants& lattice_constants = si_lattice_constants{}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(is_sidb_lattice_layout_v<Lyt, LatticeOrientation>, "Lyt is not an SiDB layout");

    if constexpr (has_siqad_coord_v<Lyt>)
    {
        if (orientation == si_lattice_orientations::SI_100)
        {
            const auto x = (c.x * lattice_constants.lat_a_100 + c.z * lattice_constants.lat_c_100.first) * 0.1;
            const auto y = (c.y * lattice_constants.lat_b_100 + c.z * lattice_constants.lat_c_100.second) * 0.1;
            return {x, y};
        }
        else if (orientation == si_lattice_orientations::SI_111)
        {
            const auto x = (c.x * lattice_constants.lat_a_111 + c.z * lattice_constants.lat_c_111.first) * 0.1;
            const auto y = (c.y * lattice_constants.lat_b_111 + c.z * lattice_constants.lat_c_111.second) * 0.1;
            return {x, y};
        }
        else
        {
            assert(false && "Unknown lattice orientation");
            return {};
        }
    }
    else
    {
        const auto cell_in_siqad = siqad::to_siqad_coord(c);

        if (orientation == si_lattice_orientations::SI_100)
        {
            const auto x =
                (cell_in_siqad.x * lattice_constants.lat_a_100 + c.z * lattice_constants.lat_c_100.first) * 0.1;
            const auto y =
                (cell_in_siqad.y * lattice_constants.lat_b_100 + cell_in_siqad.z * lattice_constants.lat_c_100.second) *
                0.1;
            return {x, y};
        }
        else if (orientation == si_lattice_orientations::SI_111)
        {
            const auto x =
                (cell_in_siqad.x * lattice_constants.lat_a_111 + c.z * lattice_constants.lat_c_111.first) * 0.1;
            const auto y =
                (cell_in_siqad.y * lattice_constants.lat_b_111 + cell_in_siqad.z * lattice_constants.lat_c_111.second) *
                0.1;
            return {x, y};
        }
        else
        {
            assert(false && "Unknown lattice orientation");
            return {};
        }
    }
}

}  // namespace fiction

#endif  // FICTION_NM_POSITION_HPP
