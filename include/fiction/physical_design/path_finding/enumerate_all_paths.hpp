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

#include "fiction/layouts/gate_layout.hpp"
#include "fiction/layouts/layout_base.hpp"
#include "fiction/layouts/obstructions.hpp"
#include "fiction/physical_design/routing_utils.hpp"

#include <phmap.h>

#include <cassert>
#include <concepts>
#include <stdexcept>
#include <utility>

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

/** @brief Implements loop-free path enumeration. @tparam Lyt Layout type. */
template <typename Lyt>
class enumerate_all_paths_impl
{
  public:
    /**
     * @brief Initializes path enumeration.
     * @param lyt Layout to search.
     * @param obj Routing endpoints.
     * @param p Search parameters.
     * @param extra Additional routing obstructions.
     */
    enumerate_all_paths_impl(const Lyt& lyt, const routing_objective obj, const enumerate_all_paths_params p,
                             const layouts::obstructions& extra) :
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
    [[nodiscard]] path_collection run()
    {
        if (!layout.contains_coordinate(objective.source) || !layout.contains_coordinate(objective.target))
        {
            throw std::invalid_argument("Routing endpoints must lie within the layout extent");
        }

        recursively_enumerate_all_paths(objective.source, objective.target, layout_coordinate_path{});

        // The public search consumes this implementation's result.
        return std::move(collection);
    }

  private:
    /**
     * The layout whose paths are to be enumerated.
     */
    const Lyt& layout;
    /** @brief Additional constraints owned by the caller. */
    const layouts::obstructions& search_obstructions;
    /**
     * The source-target coordinate pair.
     */
    const routing_objective objective;
    /**
     * Routing parameters.
     */
    const enumerate_all_paths_params params;
    /**
     * Set of visited coordinates.
     */
    phmap::flat_hash_set<fiction::layouts::layout_base::coordinate> visited{};
    /**
     * Collection of all enumerated paths.
     */
    path_collection collection{};
    /**
     * Mark a coordinate as visited.
     *
     * @param c Coordinate to mark as visited.
     */
    void mark_visited(const fiction::layouts::layout_base::coordinate& c)
    {
        visited.insert(c);
    }
    /**
     * Mark a coordinate as unvisited.
     *
     * @param c Coordinate to mark as unvisited.
     */
    void mark_unvisited(const fiction::layouts::layout_base::coordinate& c)
    {
        visited.erase(c);
    }
    /**
     * Check if a coordinate has been visited.
     *
     * @param c Coordinate to check.
     * @return `true` if the coordinate has been visited, `false` otherwise.
     */
    [[nodiscard]] bool is_visited(const fiction::layouts::layout_base::coordinate& c) const
    {
        return visited.count(c) > 0;
    }
    /**
     * Recursively enumerate all paths from `src` to `tgt` in the given layout. This function is called recursively
     * until the target coordinate is reached. Along each path, each coordinate can occur at maximum once. This function
     * does not generate duplicate or looping paths. Obstructions and crossings follow `enumerate_all_paths`.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     * @param p Current path to extend.
     */
    void recursively_enumerate_all_paths(const fiction::layouts::layout_base::coordinate& src,
                                         const fiction::layouts::layout_base::coordinate& tgt, layout_coordinate_path p)
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
            const auto explore_successor = [&, this](const auto& adjacent)
            {
                const auto next = physical_design::detail::routing_successor(layout, src, adjacent, tgt,
                                                                             params.crossings, search_obstructions);
                if (!next.has_value())
                {
                    return;  // skip the obstructed step and keep looping
                }

                // if the successor has not yet been visited
                if (!is_visited(*next))
                {
                    // recurse
                    recursively_enumerate_all_paths(*next, tgt, p);
                }

                return;  // keep looping
            };

            if constexpr (requires {
                              requires std::same_as<typename Lyt::object_id, fiction::layouts::layout_object_id>;
                          })
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

        // allow the current coordinate in other paths after recursion
        mark_unvisited(src);
    }
};

}  // namespace detail

/**
 * Enumerates all possible paths in a layout that start at a given source coordinate and lead to given target
 * coordinate. This function automatically detects whether the given layout implements a clocking interface (see
 * `gate_layout`) and respects the underlying information flow imposed by `layout`'s clocking scheme. This
 * algorithm does neither generate duplicate nor looping paths, even in a cyclic clocking scheme. That is, along each
 * path, each coordinate can occur at maximum once.
 *
 * Paths do not pass obstructed coordinates or connections, except that the target is never obstructed. A coordinate
 * or connection is obstructed if the `obstructions` argument marks it or if the layout's `is_obstructed_coordinate`
 * or `is_obstructed_connection` reports it. Gate-level layouts report their occupied tiles and existing signal
 * connections, so paths in gate-level layouts avoid all placed gates and wires.
 *
 * If crossings are enabled in the parameters, paths in gate-level layouts may cross other wires on the crossing layer.
 * Wire crossings are only allowed over other wires and only if the crossing layer is not obstructed. Furthermore, it
 * is ensured that crossings do not run along another wire but cross only in a single point (orthogonal crossings +
 * knock-knees/double wires).
 *
 * In certain cases it might be desirable to enumerate regular coordinate paths even if the layout implements a clocking
 * interface. This can be achieved by static-casting the layout to a coordinate layout when calling this function:
 * @code{.cpp}
 * fiction::layouts::cartesian_gate_layout layout = ...;
 * auto all_paths = enumerate_all_paths(static_cast<cartesian_layout>(layout), {source, target});
 * @endcode
 *
 * @tparam Lyt Type of the layout to perform path finding on.
 * @param layout The layout whose paths are to be enumerated.
 * @param objective Source-target coordinate pair.
 * @param params Parameters.
 * @param obstructions Additional coordinate and connection constraints; the search does not modify them.
 * @return A collection of all unique paths in `layout` from `objective.source` to `objective.target`.
 */
template <typename Lyt>
    requires std::derived_from<Lyt, layouts::layout_base>
[[nodiscard]] path_collection enumerate_all_paths(const Lyt& layout, const routing_objective& objective,
                                                  const enumerate_all_paths_params& params       = {},
                                                  const layouts::obstructions&      obstructions = {})
{
    return detail::enumerate_all_paths_impl<Lyt>{layout, objective, params, obstructions}.run();
}

}  // namespace fiction::physical_design::path_finding
