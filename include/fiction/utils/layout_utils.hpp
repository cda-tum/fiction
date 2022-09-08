//
// Created by marcel on 14.10.21.
//

#ifndef FICTION_LAYOUT_UTILS_HPP
#define FICTION_LAYOUT_UTILS_HPP

#include "fiction/traits.hpp"

#include <cstdint>
#include <set>

namespace fiction
{

/**
 * Returns the number of adjacent coordinates of a given one. This is not a constant value because c could be located at
 * a layout border.
 *
 * @tparam Lyt Layout type.
 * @param lyt Layout.
 * @param c Coordinate whose number of adjacencies are required.
 * @return Number of c's adjacent coordinates.
 */
template <typename Lyt>
[[nodiscard]] uint8_t num_adjacent_coordinates(const Lyt& lyt, const coordinate<Lyt>& c) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    return static_cast<uint8_t>(lyt.adjacent_coordinates(c).size());
}

}  // namespace fiction

#endif  // FICTION_LAYOUT_UTILS_HPP
