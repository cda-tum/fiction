//
// Created by Jan Drewniok on 11.02.23.
//

#ifndef FICTION_LAYOUT_SIQAD_COORDINATES_TO_FICTION_HPP
#define FICTION_LAYOUT_SIQAD_COORDINATES_TO_FICTION_HPP

#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

namespace fiction
{
/**
 * The cell coordinates of a given layout are converted to Fiction coordinates. A new layout with Fiction coordinates is
 * returned.
 *
 * @tparam Lyt Cell-level layout based on SiQAD coordinates.
 * @param lyt The given layout which is converted to a new layout based on Fiction coordinates.
 * @return New layout based on Fiction coordinates.
 */
template <typename Lyt>
sidb_cell_clk_lyt lyt_coordinates_to_fiction(const Lyt& lyt)
{
    sidb_cell_clk_lyt lyt_new{};
    lyt.foreach_cell(
        [&lyt_new, &lyt](const auto& c)
        { lyt_new.assign_cell_type(siqad::to_fiction_coord<sidb_cell_clk_lyt::cell>(c), lyt.get_cell_type(c)); });
    return lyt_new;
}

}  // namespace fiction

#endif  // FICTION_LAYOUT_SIQAD_COORDINATES_TO_FICTION_HPP
