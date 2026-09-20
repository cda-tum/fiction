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
 * @brief Enumerates every loop-less path between two coordinates in a layout.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/obstructions.hpp"
#include "fiction/physical_design/routing_utils.hpp"
#include "fiction/traits.hpp"

#include <phmap.h>

#include <cassert>

namespace fiction::physical_design::path_finding
{

/**
 * Parameters for the algorithm that enumerates all paths in a layout.
 */
struct enumerate_all_paths_params
{
    /**
     * Allow paths to cross over obstructed tiles if they are occupied by wire segments.
     */
    bool crossings = false;
};

namespace detail
{

template <typename Path, typename Lyt>
class enumerate_all_paths_impl
{
  public:
    enumerate_all_paths_impl(const Lyt& lyt, const routing_objective<Lyt>& obj, const enumerate_all_paths_params& p,
                             const layouts::obstructions<coordinate<Lyt>>& extra) :
            layout{lyt},
            search_obstructions{extra},
            objective{obj},
            params{p}
    {}

    /**
     * Enumerate all possible paths in a layout that start at `objective.source` and lead to `objective.target`.
     *
     * @return A collection of all unique paths in `layout` from `objective.source` to `objective.target`.
     */
    [[nodiscard]] path_collection<Path> run() noexcept
    {
        assert(!objective.source.is_dead() && !objective.target.is_dead() &&
               "Neither source nor target coordinate can be dead");

        assert(layout.is_within_bounds(objective.source) && layout.is_within_bounds(objective.target) &&
               "Both source and target coordinate have to be within the layout bounds");

        recursively_enumerate_all_paths(objective.source, objective.target, Path{});

        return collection;
    }

  private:
    /**
     * The layout whose paths are to be enumerated.
     */
    const Lyt& layout;
    /** @brief Additional constraints owned by the caller. */
    const layouts::obstructions<coordinate<Lyt>>& search_obstructions;
    /**
     * The source-target coordinate pair.
     */
    const routing_objective<Lyt> objective;
    /**
     * Routing parameters.
     */
    const enumerate_all_paths_params params;
    /**
     * Set of visited coordinates.
     */
    phmap::flat_hash_set<coordinate<Lyt>> visited{};
    /**
     * Collection of all enumerated paths.
     */
    path_collection<Path> collection{};
    /**
     * Mark a coordinate as visited.
     *
     * @param c Coordinate to mark as visited.
     */
    void mark_visited(const coordinate<Lyt>& c) noexcept
    {
        visited.insert(c);
    }
    /**
     * Mark a coordinate as unvisited.
     *
     * @param c Coordinate to mark as unvisited.
     */
    void mark_unvisited(const coordinate<Lyt>& c) noexcept
    {
        visited.erase(c);
    }
    /**
     * Check if a coordinate has been visited.
     *
     * @param c Coordinate to check.
     * @return `true` if the coordinate has been visited, `false` otherwise.
     */
    [[nodiscard]] bool is_visited(const coordinate<Lyt>& c) const noexcept
    {
        return visited.count(c) > 0;
    }
    /**
     * Recursively enumerate all paths from `src` to `tgt` in the given layout. This function is called recursively
     * until the target coordinate is reached. Along each path, each coordinate can occur at maximum once. This function
     * does not generate duplicate or looping paths. If the given layout implements the obstruction interface (see
     * `obstructions`), paths will not be routed via obstructed coordinates or connections. If the given layout is
     * a gate-level layout and implements the obstruction interface (see `obstructions`), paths may contain wire
     * crossings if specified in the parameters. Wire crossings are only allowed over other wires and only if the
     * crossing layer is not obstructed. Furthermore, it is ensured that crossings do not run along another wire but
     * cross only in a single point (orthogonal crossings + knock-knees/double wires).
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     * @param p Current path to extend.
     */
    void recursively_enumerate_all_paths(const coordinate<Lyt>& src, const coordinate<Lyt>& tgt, Path p) noexcept
    {
        // mark source coordinate as visited and append it to the path
        mark_visited(src);
        p.append(src);

        // if source and target are identical, a path has been found
        if (src == tgt)
        {
            collection.add(p);
        }
        else  // destination is not reached yet
        {
            const auto explore_successor = [&, this](auto successor)  // make a copy
                noexcept
            {
                // return to ground layer to avoid getting stuck in crossing layer
                successor = layout.below(successor);

                // check if successor is obstructed
                if (physical_design::detail::routing_coordinate_obstructed(layout, successor, search_obstructions) &&
                    successor != tgt)
                {
                    // if crossings are enabled, check if it is possible to switch to the crossing layer
                    if (params.crossings &&
                        (is_crossable_wire(layout, src, successor) || layout.above(successor) == tgt))
                    {
                        // if the crossing layer is not obstructed
                        if (const auto above_successor = layout.above(successor);
                            above_successor != successor && (!physical_design::detail::routing_coordinate_obstructed(
                                                                 layout, above_successor, search_obstructions) ||
                                                             above_successor == tgt))
                        {
                            // allow exploring the crossing layer
                            successor = above_successor;
                        }
                        else
                        {
                            return;  // skip the obstructed coordinate and keep looping
                        }
                    }
                    else
                    {
                        return;  // skip the obstructed coordinate and keep looping
                    }
                }

                // check if the connection to the successor is obstructed
                if (physical_design::detail::routing_connection_obstructed(layout, src, successor, search_obstructions))
                {
                    return;  // skip the obstructed connection and keep looping
                }

                // if the successor has not yet been visited
                if (!is_visited(successor))
                {
                    // recurse
                    recursively_enumerate_all_paths(successor, tgt, p);
                }

                return;  // keep looping
            };

            if constexpr (has_foreach_outgoing_clocked_zone_v<Lyt>)
            {
                // recurse for all outgoing clock zones
                layout.foreach_outgoing_clocked_zone(src, explore_successor);
            }
            else
            {
                // recurse for all adjacent coordinates
                layout.foreach_adjacent_coordinate(src, explore_successor);
            }
        }

        // after recursion, remove current coordinate from path and mark it as unvisited to allow it in other paths
        p.pop_back();
        mark_unvisited(src);
    }
};

}  // namespace detail

/**
 * Enumerates all possible paths in a layout that start at a given source coordinate and lead to given target
 * coordinate. This function automatically detects whether the given layout implements a clocking interface (see
 * `gate_level_layout`) and respects the underlying information flow imposed by `layout`'s clocking scheme. This
 * algorithm does neither generate duplicate nor looping paths, even in a cyclic clocking scheme. That is, along each
 * path, each coordinate can occur at maximum once.
 *
 * If the given layout implements the obstruction interface (see `obstructions`), paths will not be routed via
 * obstructed coordinates or connections.
 *
 * If the given layout is a gate-level layout and implements the obstruction interface (see `obstructions`), paths
 * may contain wire crossings if specified in the parameters. Wire crossings are only allowed over other wires and only
 * if the crossing layer is not obstructed. Furthermore, it is ensured that crossings do not run along another wire but
 * cross only in a single point (orthogonal crossings + knock-knees/double wires).
 *
 * In certain cases it might be desirable to enumerate regular coordinate paths even if the layout implements a clocking
 * interface. This can be achieved by static-casting the layout to a coordinate layout when calling this function:
 * @code{.cpp}
 * using clk_lyt = gate_level_layout<cartesian_layout<>>;
 * using path = layout_coordinate_path<cartesian_layout<>>;
 * clk_lyt layout = ...;
 * auto all_paths = enumerate_all_paths<path>(static_cast<cartesian_layout<>>(layout), {source, target});
 * @endcode
 *
 * @tparam Path Type of the returned individual paths.
 * @tparam Lyt Type of the layout to perform path finding on.
 * @param layout The layout whose paths are to be enumerated.
 * @param objective Source-target coordinate pair.
 * @param params Parameters.
 * @param obstructions Additional coordinate and connection constraints; the search does not modify them.
 * @return A collection of all unique paths in `layout` from `objective.source` to `objective.target`.
 */
template <typename Path, typename Lyt>
[[nodiscard]] path_collection<Path>
enumerate_all_paths(const Lyt& layout, const routing_objective<Lyt>& objective,
                    const enumerate_all_paths_params&             params       = {},
                    const layouts::obstructions<coordinate<Lyt>>& obstructions = {}) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    return detail::enumerate_all_paths_impl<Path, Lyt>{layout, objective, params, obstructions}.run();
}

}  // namespace fiction::physical_design::path_finding
