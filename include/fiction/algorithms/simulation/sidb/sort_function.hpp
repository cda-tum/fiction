//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_SORT_FUNCTION_HPP
#define FICTION_SORT_FUNCTION_HPP

#include <fiction/traits.hpp>

namespace fiction
{

template <typename Lyt>
[[nodiscard]] bool local_sort_sidb_cells(const cell<Lyt>& a, const cell<Lyt>& b) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt must be a cell level layout");

    if (a.y != b.y)
    {
        return a.y < b.y;
    }
    if (a.z != b.z)
    {
        return a.z < b.z;
    }

    return a.x < b.x;
}

}  // namespace fiction

#endif  // FICTION_SORT_FUNCTION_HPP
