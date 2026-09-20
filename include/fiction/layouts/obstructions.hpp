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
 */
#pragma once
#include <phmap.h>

#include <utility>
namespace fiction::layouts
{
/**
 * @brief Explicit obstructions stored by layouts or supplied to a routing search.
 * Copies are independent. This object contains no layout or occupancy information.
 * @tparam Coordinate Coordinate identifying a position.
 */
template <typename Coordinate>
class obstructions
{
  public:
    /**
     * Marks the given coordinate as obstructed.
     *
     * @param c Coordinate to obstruct.
     */
    void obstruct_coordinate(const Coordinate& c) noexcept
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
     */
    void obstruct_connection(const Coordinate& src, const Coordinate& tgt) noexcept
    {
        obstructed_connections.insert({src, tgt});
    }
    /**
     * Clears the obstruction status of the given coordinate `c` if the obstruction was manually marked via
     * `obstruct_coordinate`.
     *
     * @param c Coordinate to clear.
     */
    void clear_obstructed_coordinate(const Coordinate& c) noexcept
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
    void clear_obstructed_connection(const Coordinate& src, const Coordinate& tgt) noexcept
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
    [[nodiscard]] bool is_obstructed_coordinate(const Coordinate& c) const noexcept
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
    [[nodiscard]] bool is_obstructed_connection(const Coordinate& src, const Coordinate& tgt) const noexcept
    {
        return obstructed_connections.contains({src, tgt});
    }

  private:
    /** @brief Explicitly blocked positions. */
    phmap::parallel_flat_hash_set<Coordinate> obstructed_coordinates{};
    /** @brief Explicitly blocked directed connections. */
    phmap::parallel_flat_hash_set<std::pair<Coordinate, Coordinate>> obstructed_connections{};
};
}  // namespace fiction::layouts
