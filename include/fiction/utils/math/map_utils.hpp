/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/utils/math/math_utils.hpp"

#include <algorithm>
#include <concepts>

namespace fiction::utils::math
{

/**
 * This function searches for a floating-point value specified by the `key` in the provided map `map`, applying a
 * tolerance specified by `fiction::constants::ERROR_MARGIN`. Each key in the map is compared to the
 * specified key within this tolerance.
 *
 * @tparam MapType The type of the map containing parameter points as keys.
 * @param map The map containing parameter points as keys and associated values.
 * @param key The parameter point to search for in the map.
 * @return An iterator to the found parameter point in the map, or `map.cend()` if not found.
 */
template <typename MapType>
    requires std::floating_point<typename MapType::key_type>
typename MapType::const_iterator find_key_with_tolerance(const MapType& map, const typename MapType::key_type& key)
{
    constexpr double tolerance = ERROR_MARGIN;

    auto compare_keys = [&key, &tolerance](const auto& pair) { return std::abs(pair.first - key) < tolerance; };

    return std::ranges::find_if(map, compare_keys);
}

}  // namespace fiction::utils::math
