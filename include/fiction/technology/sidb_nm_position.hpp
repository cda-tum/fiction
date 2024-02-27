//
// Created by Jan Drewniok on 28.01.23.
//

#ifndef FICTION_NM_POSITION_HPP
#define FICTION_NM_POSITION_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/sidb_lattice_types.hpp"
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
template <typename Lyt>
[[nodiscard]] constexpr std::pair<double, double> sidb_nm_position(const cell<Lyt>& c) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(is_sidb_lattice_v<Lyt>, "Lyt is not an SiDB lattice layout");

    if constexpr (has_siqad_coord_v<Lyt>)
    {
        if (has_same_lattice_orientation_v<Lyt, sidb_100_lattice>)
        {
            const auto x = (c.x * sidb_100_lattice::LAT_A + c.z * sidb_100_lattice::LAT_C.first) * 0.1;
            const auto y = (c.y * sidb_100_lattice::LAT_B + c.z * sidb_100_lattice::LAT_C.second) * 0.1;
            return {x, y};
        }
        else if (has_same_lattice_orientation_v<Lyt, sidb_111_lattice>)
        {
            const auto x = (c.x * sidb_111_lattice::LAT_A + c.z * sidb_111_lattice::LAT_C.first) * 0.1;
            const auto y = (c.y * sidb_111_lattice::LAT_B + c.z * sidb_111_lattice::LAT_C.second) * 0.1;
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

        if (has_same_lattice_orientation_v<Lyt, sidb_100_lattice>)
        {
            const auto x = (cell_in_siqad.x * sidb_100_lattice::LAT_A + c.z * sidb_100_lattice::LAT_C.first) * 0.1;
            const auto y =
                (cell_in_siqad.y * sidb_100_lattice::LAT_B + cell_in_siqad.z * sidb_100_lattice::LAT_C.second) * 0.1;
            return {x, y};
        }
        else if (has_same_lattice_orientation_v<Lyt, sidb_111_lattice>)
        {
            const auto x = (cell_in_siqad.x * sidb_111_lattice::LAT_A + c.z * sidb_111_lattice::LAT_C.first) * 0.1;
            const auto y =
                (cell_in_siqad.y * sidb_111_lattice::LAT_B + cell_in_siqad.z * sidb_111_lattice::LAT_C.second) * 0.1;
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
