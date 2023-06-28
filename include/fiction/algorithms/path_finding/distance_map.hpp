//
// Created by marcel on 18.06.23.
//

#ifndef FICTION_DISTANCE_MAP_HPP
#define FICTION_DISTANCE_MAP_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/traits.hpp"

#include <phmap.h>

#include <cstdint>
#include <functional>
#include <vector>

namespace fiction
{

template <typename Dist>
using distance_map = std::vector<std::vector<Dist>>;

template <typename Lyt, typename Dist>
using sparse_distance_map = phmap::parallel_flat_hash_map<coordinate<Lyt>, Dist>;

template <typename Lyt, typename Dist>
[[nodiscard]] distance_map<Dist> initialize_distance_map(const Lyt&                         lyt,
                                                         const distance_functor<Lyt, Dist>& dist_fn) noexcept
{
    distance_map<Dist> dist_map(std::vector<Dist>(lyt.area(), Dist{}), lyt.area());

    lyt.foreach_coordinate(
        [&lyt, &dist_fn, &dist_map](const auto& c1, const unsigned y)
        {
            lyt.foreach_coordinate([&lyt, &dist_fn, &dist_map, &c1, y](const auto& c2, const unsigned x)
                                   { dist_map[x][y] = dist_fn(lyt, c1, c2); });
        });

    return dist_map;
}

template <typename Lyt, typename Dist>
[[nodiscard]] sparse_distance_map<Lyt, Dist>
initialize_sparse_distance_map(const Lyt& lyt, const distance_functor<Lyt, Dist>& dist_fn) noexcept
{
    sparse_distance_map<Lyt, Dist> dist_map(lyt.area() * 2);

    lyt.foreach_coordinate(
        [&lyt, &dist_fn, &dist_map](const auto& c1) {
            lyt.foreach_coordinate([&lyt, &dist_fn, &dist_map, &c1](const auto& c2)
                                   { dist_map[c1][c2] = dist_fn(lyt, c1, c2); });
        });

    return dist_map;
}

}  // namespace fiction

#endif  // FICTION_DISTANCE_MAP_HPP
