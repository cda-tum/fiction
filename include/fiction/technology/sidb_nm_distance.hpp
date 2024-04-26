//
// Created by Jan Drewniok on 26.03.24.
//

#ifndef FICTION_SIDB_NM_DISTANCE_HPP
#define FICTION_SIDB_NM_DISTANCE_HPP

#include "fiction/technology/sidb_nm_position.hpp"
#include "fiction/traits.hpp"

#include <cmath>

namespace fiction
{
/**
 * Computes the distance between two SiDB cells in nanometers (unit: nm).
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param source The source cell.
 * @param target The target cell.
 * @return The distance between the two cells in nanometers (unit: nm).
 */
template <typename Lyt>
[[nodiscard]] constexpr double sidb_nm_distance(const Lyt& lyt, const coordinate<Lyt>& source,
                                                const coordinate<Lyt>& target) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not based on SiDB technology");

    const auto pos_c1 = sidb_nm_position<Lyt>(lyt, source);
    const auto pos_c2 = sidb_nm_position<Lyt>(lyt, target);

    const auto x = pos_c1.first - pos_c2.first;
    const auto y = pos_c1.second - pos_c2.second;

    return std::hypot(x, y);
}

}  // namespace fiction

#endif  // FICTION_SIDB_NM_DISTANCE_HPP
