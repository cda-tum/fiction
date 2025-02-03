//
// Created by Jan Drewniok on 02.02.25.
//

#ifndef FICTION_MAP_UTILS_HPP
#define FICTION_MAP_UTILS_HPP

#include "fiction/technology/constants.hpp"

#include <algorithm>

namespace fiction
{

/**
 * This function searches for a floating-point value specified by the `key` in the provided map `map`, applying a
 * tolerance specified by `fiction::physical_constants::ERROR_MARGIN`. Each key in the map is compared to the
 * specified key within this tolerance.
 *
 * @tparam MapType The type of the map containing parameter points as keys.
 * @param map The map containing parameter points as keys and associated values.
 * @param key The parameter point to search for in the map.
 * @return An iterator to the found parameter point in the map, or `map.cend()` if not found.
 */
template <typename MapType>
typename MapType::const_iterator find_key_with_tolerance(const MapType& map, const typename MapType::key_type& key)
{
    static_assert(std::is_floating_point_v<typename MapType::key_type>, "Map key type must be floating-point");

    constexpr double tolerance = constants::ERROR_MARGIN;

    auto compare_keys = [&key, &tolerance](const auto& pair) { return std::abs(pair.first - key) < tolerance; };

    return std::find_if(map.cbegin(), map.cend(), compare_keys);
}

}  // namespace fiction

#endif  // FICTION_MAP_UTILS_HPP
