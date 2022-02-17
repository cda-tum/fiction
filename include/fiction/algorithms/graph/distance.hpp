//
// Created by marcel on 17.02.22.
//

#ifndef FICTION_DISTANCE_HPP
#define FICTION_DISTANCE_HPP

#include "fiction/traits.hpp"

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace fiction
{

/**
 * The Manhattan distance between two layout coordinates given by
 *
 *  |x1 - x2| + |y1 - y2|
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral type for the distance.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Manhattan distance between source and target.
 */
template <typename Lyt, typename Dist = uint64_t>
Dist manhattan_distance(const coordinate<Lyt>& source, const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_integral_v<Dist>, "Dist is not an integral type");

    return static_cast<Dist>(std::abs(static_cast<int64_t>(source.x) - static_cast<int64_t>(target.x)) +
                             std::abs(static_cast<int64_t>(source.y) - static_cast<int64_t>(target.y)));
}
/**
 * The Euclidean distance between two layout coordinates given by
 *
 *  sqrt((x1 - x2)^2 + (y1 - y2)^2)
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Floating-point type for the distance.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Euclidean distance between source and target.
 */
template <typename Lyt, typename Dist = double>
Dist euclidean_distance(const coordinate<Lyt>& source, const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_floating_point_v<Dist>, "Dist is not a floating-point type");

    const auto x = static_cast<double>(source.x) - static_cast<double>(target.x);
    const auto y = static_cast<double>(source.y) - static_cast<double>(target.y);

    // do not use std::pow as it is supposedly slower if not compiled with -ffast-math and -O3 according to
    // https://stackoverflow.com/questions/2940367/what-is-more-efficient-using-pow-to-square-or-just-multiply-it-with-itself
    return static_cast<Dist>(std::sqrt((x * x) + (y * y)));
}

}  // namespace fiction

#endif  // FICTION_DISTANCE_HPP
