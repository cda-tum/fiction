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
 * @brief Explicit coordinate and directed-connection obstructions.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/layout_base.hpp"

#include <phmap.h>

#include <functional>
#include <utility>

namespace fiction::layouts
{
/**
 * @brief Explicit obstructions stored by layouts or supplied to a routing search.
 * Copies are independent. This object contains no layout or occupancy information.
 */
class obstructions
{
  public:
    /**
     * Marks the given coordinate as obstructed.
     *
     * @param c Coordinate to obstruct.
     * @throws std::bad_alloc If allocation fails.
     */
    void obstruct_coordinate(const layout_base::coordinate& c)
    {
        obstructed_coordinates.insert(c);
    }
    /**
     * Marks the connection from coordinate `src` to coordinate `tgt` as obstructed.
     *
     * @note Coordinates marked this way will not be crossed with wires by path finding algorithms.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     * @throws std::bad_alloc If allocation fails.
     */
    void obstruct_connection(const layout_base::coordinate& src, const layout_base::coordinate& tgt)
    {
        obstructed_connections.insert({src, tgt});
    }
    /**
     * Clears the obstruction status of the given coordinate `c` if the obstruction was manually marked via
     * `obstruct_coordinate`.
     *
     * @param c Coordinate to clear.
     */
    void clear_obstructed_coordinate(const layout_base::coordinate& c) noexcept
    {
        obstructed_coordinates.erase(c);
    }
    /**
     * Clears the obstruction status of the connection from coordinate `src` to coordinate `tgt` if the obstruction was
     * manually marked via `obstruct_connection`.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     */
    void clear_obstructed_connection(const layout_base::coordinate& src, const layout_base::coordinate& tgt) noexcept
    {
        obstructed_connections.erase({src, tgt});
    }
    /**
     * Clears all obstructed coordinates that were manually marked via `obstruct_coordinate`.
     */
    void clear_obstructed_coordinates() noexcept
    {
        obstructed_coordinates.clear();
    }
    /**
     * Clears all obstructed connections that were manually marked via `obstruct_connection`.
     */
    void clear_obstructed_connections() noexcept
    {
        obstructed_connections.clear();
    }
    /**
     * Checks if the given coordinate is obstructed of some sort.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is obstructed.
     */
    [[nodiscard]] bool is_obstructed_coordinate(const layout_base::coordinate& c) const noexcept
    {
        return obstructed_coordinates.contains(c);
    }
    /**
     * Checks if the given coordinate-coordinate connection is obstructed of some sort.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     * @return `true` iff the connection from `src` to `tgt` is obstructed.
     */
    [[nodiscard]] bool is_obstructed_connection(const layout_base::coordinate& src,
                                                const layout_base::coordinate& tgt) const noexcept
    {
        return obstructed_connections.contains({src, tgt});
    }

    /**
     * Visits explicitly obstructed coordinates in unspecified order.
     * @tparam Fn Callable accepting one coordinate.
     * @param fn Callback for each manual obstruction.
     */
    template <typename Fn>
    void foreach_obstructed_coordinate(Fn&& fn) const
    {
        for (const auto& coordinate : obstructed_coordinates) std::invoke(fn, coordinate);
    }
    /**
     * Visits explicitly obstructed directed connections in unspecified order.
     * @tparam Fn Callable accepting source and target coordinates.
     * @param fn Callback for each manual obstruction.
     */
    template <typename Fn>
    void foreach_obstructed_connection(Fn&& fn) const
    {
        for (const auto& [source, target] : obstructed_connections) std::invoke(fn, source, target);
    }

  private:
    /** @brief Explicitly blocked positions. */
    phmap::parallel_flat_hash_set<layout_base::coordinate> obstructed_coordinates{};
    /** @brief Explicitly blocked directed connections. */
    phmap::parallel_flat_hash_set<std::pair<layout_base::coordinate, layout_base::coordinate>> obstructed_connections{};
};
}  // namespace fiction::layouts
