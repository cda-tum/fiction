//
// Created by marcel on 07.08.19.
//

#ifndef FICTION_MAGCAD_MAGNET_COUNT_HPP
#define FICTION_MAGCAD_MAGNET_COUNT_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"

#include <cstdint>

namespace fiction
{
/**
 * Calculates the number of magnets for an iNML layout the way MagCAD (https://topolinano.polito.it/) would do it.
 * That is, counting chains of 4 inverters as a single entity.
 *
 * @tparam Lyt iNML cell-level layout type.
 * @param lyt The iNML cell-level layout whose area is desired.
 * @return Number of magnets as counted by MagCAD.
 */
template <typename Lyt>
uint64_t magcad_magnet_count(const Lyt& lyt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_inml_technology_v<Lyt>, "Lyt is not an iNML layout");
    static_assert(has_foreach_cell_v<Lyt>, "Lyt does not implement the foreach_cell function");

    uint64_t num_inv_cells{0ull};

    // count inverter cells
    lyt.foreach_cell(
        [&lyt, &num_inv_cells](const auto& c)
        {
            if (lyt.get_cell_type(c) == inml_technology::cell_type::INVERTER_MAGNET)
            {
                ++num_inv_cells;
            }
        });

    return lyt.num_cells() + (num_inv_cells / 4);
}

}  // namespace fiction

#endif  // FICTION_MAGCAD_MAGNET_COUNT_HPP
