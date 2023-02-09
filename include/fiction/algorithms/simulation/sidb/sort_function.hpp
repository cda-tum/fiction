//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_SORT_FUNCTION_HPP
#define FICTION_SORT_FUNCTION_HPP

#include <algorithm>
#include <utility>

namespace fiction
{

template <typename Lyt>
bool local_sort_sidb_cells(const typename Lyt::cell& a, const typename Lyt::cell& b)
{
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
